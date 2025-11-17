#include <armpi_controller/ArmpiController.h>

ArmpiController::ArmpiController(ros::NodeHandle &nh, const std::string &node_name, const std::string &task_name) : nh_(nh), collect_data_(nh, task_name), node_name_(node_name), running_(false) {
  pub_= nh_.advertise<armpi_operation_msgs::RobotCommand>("armpi_command", 1);
  ROS_INFO_STREAM("ArmpiController for task " << task_name << " is started");
  reset();
}

ArmpiController::~ArmpiController() {
  pub_.shutdown();
  running_ = false;
  if (input_thread_.joinable()) {
    input_thread_.join();
  }
}

void ArmpiController::reset() {
  // initialize armpi
  cmd_.chassis_move_forward = 0;
  cmd_.chassis_move_right = 0;
  cmd_.angular_right = 0;
  cmd_.arm_x = 0;
  cmd_.arm_y = 0;
  cmd_.arm_z = 0;
  cmd_.arm_alpha = 0;
  cmd_.arm_alpha1 = 0;
  cmd_.arm_alpha2 = 0;
  cmd_.gripper_close = 0;
}


void ArmpiController::start() {
  running_ = true;
  input_thread_ = std::thread(&ArmpiController::controllerLoop, this);
}

void ArmpiController::controllerLoop() {
  while (running_ && ros::ok()) {
    reset();
    this->getCommand();
    pub_.publish(cmd_);
    ros::Duration(0.05).sleep();
  }
}
