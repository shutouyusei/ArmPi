#include <armpi_controller/ArmpiController.h>

ArmpiController::ArmpiController(ros::NodeHandle& nh,const std::string& node_name):
  nh_(nh),command_publisher_(nh),collect_data_(nh),node_name_(node_name),running_(false) {
  // initialize armpi
  cmd_.arm_x = 0.0;
  cmd_.arm_y = 0.12;
  cmd_.arm_z = 0.15;
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
    if(collect_data_.is_running_ == true) collect_data_.start();
  }
}
