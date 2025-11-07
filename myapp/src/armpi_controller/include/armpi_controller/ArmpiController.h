#pragma once
#include <ros/ros.h>
#include <armpi_command_publisher/ArmpiCommandPublisher.h>
#include <collect_data/CollectData.h>
#include <thread>

class ArmpiController {
public:
  ArmpiController(ros::NodeHandle& nh, const std::string& node_name);

  virtual ~ArmpiController();

  void start(); 

protected:
  virtual void getCommand() = 0;

private:
  void controllerLoop();
protected:
  ros::NodeHandle nh_;
  std::string node_name_;
  CollectData collect_data_;

  // armpi controller constants
  const float MAX_SPEED = 100.0; 
  const float MAX_TURN = 0.5; 
  const float IK_STEP = 0.005;
  const float GRIPPER_STEP = 10;
  armpi_operation_msgs::RobotCommand cmd_;
private:
  ArmpiCommandPublisher command_publisher_;
  bool running_;
  std::thread input_thread_;
};
