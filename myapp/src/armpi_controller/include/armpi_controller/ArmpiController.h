#pragma once
#include <ros/ros.h>
#include <armpi_command_publisher/ArmpiCommandPublisher.h>

class ArmpiController {
public:
  ArmpiController(ros::NodeHandle& nh, const std::string& node_name) 
  : nh_(nh), command_publisher_(nh), node_name_(node_name), running_(false) {}

  virtual ~ArmpiController() = default;

  virtual void start() = 0; 

protected:
  ros::NodeHandle nh_;
  ArmpiCommandPublisher command_publisher_;
  std::string node_name_;
  bool running_;
};
