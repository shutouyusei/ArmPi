#include <armpi_controller/ArmpiController.h>

ArmpiController::ArmpiController(ros::NodeHandle& nh,const std::string& node_name,const std::string& task_name):
  nh_(nh),command_publisher_(nh),collect_data_(nh,task_name),node_name_(node_name),running_(false) {
  reset();
}

void ArmpiController::reset(){
  // initialize armpi
  cmd_.arm_x = 0.0;
  cmd_.arm_y = 0.12;
  cmd_.arm_z = 0.8;
  cmd_.arm_alpha = -90.0;
  cmd_.arm_alpha1 = -180.0;
  cmd_.arm_alpha2 = 0.0;
  cmd_.gripper = 200;

}

ArmpiController::~ArmpiController() {
  running_ = false;
  if (input_thread_.joinable()) {
    input_thread_.join();
  }
}

void ArmpiController::start() {
  running_ = true;
  input_thread_ = std::thread(&ArmpiController::controllerLoop, this);
}

void ArmpiController::controllerLoop(){
  while (running_ && ros::ok()) {
    cmd_.base_velocity.linear.x = 0.0;
    cmd_.base_velocity.angular.z = 0.0;
    this->getCommand();
    command_publisher_.sendCommand(cmd_);
    ros::Duration(0.05).sleep();
  }
}
