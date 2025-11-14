#include <armpi_controller/AIController.h>

AIController::AIController(ros::NodeHandle& nh,const std::string& model_name):ArmpiController(nh,"armpi_controller",model_name) {
  ROS_INFO("AIController Initialized.");
}

AIController::~AIController() {}

void AIController::getCommand() {
  // cmd_ = 
}
