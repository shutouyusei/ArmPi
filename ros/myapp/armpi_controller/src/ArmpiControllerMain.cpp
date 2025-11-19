#include <ros/ros.h>
#include <armpi_controller/ArmpiController.h>
#include <armpi_controller/keyboard/KeyboardController.h>
#include <armpi_controller/ai/AIController.h>
#include <memory>
#include <string>
#include <signal.h>

std::unique_ptr<ArmpiController> controller;

void controllerSignalHandler(int signal) {
  ROS_INFO("Received signal to stop the controller.");
  controller->finish();
  ros::shutdown();
}

int main(int argc, char **argv) {
  if (argc < 2) { 
    ROS_ERROR("Usage: %s <task_name>", argv[0]);
    ROS_ERROR("Please set task name as a command-line argument.");
    return 1; // エラー終了
  }

  // 引数（タスク名）を変数に格納
  std::string task_name = argv[1];

  ros::init(argc, argv, "generic_robot_controller",ros::init_options::NoSigintHandler);
  signal(SIGINT, controllerSignalHandler);
  ros::NodeHandle nh_private("~"); 
  ros::NodeHandle nh_public;     


  std::string controller_type;
  nh_private.param<std::string>("controller_type", controller_type, "keyboard"); 

  ROS_INFO("Selected controller type: %s", controller_type.c_str());

  if (controller_type == "keyboard") {
    ROS_INFO("Initializing KeyboardController...");
    controller = std::make_unique<KeyboardController>(nh_public, task_name);

  } else if (controller_type == "ai") {
    ROS_INFO("Initializing AIController...");
    controller = std::make_unique<AIController>(nh_public, task_name);

  } else {
    ROS_ERROR("Unknown controller type: '%s'. Using default (keyboard).", controller_type.c_str());
    controller = std::make_unique<KeyboardController>(nh_public, task_name);
  }

  if (controller) {
    ROS_INFO("starting controller");
    controller->start();
    ros::spin();
  } else {
    ROS_FATAL("Controller initialization failed.");
    return 1;
  }

  return 0;
}
