#include <armpi_controller/ai/AIController.h>
#include <armpi_controller/ai/UserInput.h>

AIController::AIController(ros::NodeHandle &nh, const std::string &model_name) : ArmpiController(nh, "armpi_controller", model_name), it_(nh) {
  ROS_INFO("AIController Initialized.");
  client_predict_ = nh.serviceClient<ai_model_service::PredictAction>("predict_action");

  client_predict_.waitForExistence();

  sub_image_ = it_.subscribe("/usb_cam/image_raw", 1, &AIController::imageCallback, this);
  sub_joint_state_ = nh_.subscribe("/joint_states", 1, &AIController::jointStateCallback, this);
  user_input_ = new UserInput(this);
}

AIController::~AIController() {
  delete user_input_;
}

void AIController::finish(){
  sub_image_.shutdown(); 
  sub_joint_state_.shutdown();
  ArmpiController::finish();
}

void AIController::getCommand() {
  user_input_->keyInput();
  if (!(user_input_->is_running_)){
    return;
  }
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
    ROS_INFO_THROTTLE(1.0, "[AIController] Command Received:\n"
                      "  Chassis (fwd/right/ang): %d, %d, %d\n"
                      "  Arm (x/y/z):         %d, %d, %d\n"
                      "  Arm (alpha/rot/grip):  %d, %d, %d", 
                      cmd_.chassis_move_forward,
                      cmd_.chassis_move_right,
                      cmd_.angular_right,
                      cmd_.arm_x,
                      cmd_.arm_y,
                      cmd_.arm_z,
                      cmd_.arm_alpha,
                      cmd_.rotation,
                      cmd_.gripper_close);
  } else {
    ROS_ERROR( "[AIController] サービス 'predict_action' の呼び出しに失敗しました。");
    cmd_ = armpi_operation_msgs::RobotCommand();
  }
}

void AIController::imageCallback(const sensor_msgs::ImageConstPtr &msg) {
  std::lock_guard<std::mutex> lock(image_mutex_);
  latest_image_msg_ = msg;
  received_image_ = true;
}

void AIController::jointStateCallback( const sensor_msgs::JointStateConstPtr &msg) {
  std::lock_guard<std::mutex> lock(joint_mutex_);
  latest_joint_state_msg_ = msg;
  received_joint_state_ = true;
}
