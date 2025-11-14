#include <armpi_controller/AIController.h>

AIController::AIController(ros::NodeHandle &nh, const std::string &model_name)
    : ArmpiController(nh, "armpi_controller", model_name), it_(nh) {
  ROS_INFO("AIController Initialized.");
  client_predict_ =
      nh.serviceClient<ai_model_service::PredictAction>("predict_action");

  client_predict_.waitForExistence();

  sub_image_ = it_.subscribe("/usb_cam/image_raw", 1,
                             &AIController::imageCallback, this);
  sub_joint_state_ = nh_.subscribe("/joint_states", 1,
                                   &AIController::jointStateCallback, this);
}

AIController::~AIController() {}

void AIController::getCommand() {
  if (!received_image_ || !received_joint_state_) {
    return;
  }

  ai_model_service::PredictAction srv;

  {
    std::lock_guard<std::mutex> lock(image_mutex_);
    srv.request.current_image = *latest_image_msg_;
  }
  {
    std::lock_guard<std::mutex> lock(joint_mutex_);
    srv.request.current_joint_state = *latest_joint_state_msg_;
  }

  if (client_predict_.call(srv)) {
    cmd_ = srv.response.predicted_command;
  } else {
    ROS_ERROR(
        "[AIController] サービス 'predict_action' の呼び出しに失敗しました。");
    cmd_ = armpi_operation_msgs::RobotCommand();
  }
}

void AIController::imageCallback(const sensor_msgs::ImageConstPtr &msg) {
  std::lock_guard<std::mutex> lock(image_mutex_);
  latest_image_msg_ = msg;
  received_image_ = true;
}

void AIController::jointStateCallback(
    const sensor_msgs::JointStateConstPtr &msg) {
  std::lock_guard<std::mutex> lock(joint_mutex_);
  latest_joint_state_msg_ = msg;
  received_joint_state_ = true;
}
