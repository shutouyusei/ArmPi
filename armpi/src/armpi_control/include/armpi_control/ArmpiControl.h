#pragma once
#include <ros/ros.h>
#include <armpi_control/ArmpiCommandSubscriber.h>
#include <armpi_driver/ArmpiDriver.h>
#include <functional>

class ArmpiControl{
public:
  ArmpiControl(ros::NodeHandle& nh);
  ~ArmpiControl();
private:
  ros::NodeHandle nh_;
  ArmpiCommandSubscriber *command_subscriber_;
  ArmpiDriver armpi_driver_;
};
