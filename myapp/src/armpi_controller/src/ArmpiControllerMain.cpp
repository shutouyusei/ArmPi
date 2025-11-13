#include <ros/ros.h>
#include <armpi_controller/KeyboardController.h>
#include <armpi_controller/ArmpiController.h>
#include <memory>

int main(int argc, char **argv) {
  if (argc = 0 ){
    ROS_ERROR("Please set task name");
    return 0;
  }
  ros::init(argc, argv, "generic_robot_controller");
  ros::NodeHandle nh;

  std::unique_ptr<ArmpiController> controller;

  std::string controller_type;
  nh.param<std::string>("controller_type", controller_type, "keyboard");

  if (controller_type == "keyboard") {
    controller = std::make_unique<KeyboardController>(nh,argv[1]);
  } else {
    ROS_ERROR("Unknown controller type: %s. Using default (keyboard).", controller_type.c_str());
    controller = std::make_unique<KeyboardController>(nh,argv[1]);
  }

  if (controller) {
    controller->start();
    ros::spin(); 
  }

  return 0;
}
