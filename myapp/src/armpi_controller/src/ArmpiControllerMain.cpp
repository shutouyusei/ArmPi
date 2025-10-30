#include <ros/ros.h>
#include <armpi_controller/KeyboardController.h>
#include <armpi_controller/ArmpiController.h>
#include <memory>

int main(int argc, char **argv) {
  ros::init(argc, argv, "generic_robot_controller");
  ros::NodeHandle nh;

  std::unique_ptr<ArmpiController> controller;

  std::string controller_type;
  nh.param<std::string>("controller_type", controller_type, "keyboard");

  if (controller_type == "keyboard") {
    controller = std::make_unique<KeyboardController>(nh);
  } else {
    ROS_ERROR("Unknown controller type: %s. Using default (keyboard).", controller_type.c_str());
    controller = std::make_unique<KeyboardController>(nh);
  }

  if (controller) {
    controller->start();
    ROS_INFO("Controller Node Running: Type=%s", controller_type.c_str());
    ros::spin(); 
  }

  return 0;
}
