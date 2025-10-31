#pragma once
#include <ros/ros.h>
#include <armpi_control/ArmpiCommandSubscriber.h>
#include <armpi_chassis/ArmpiChassis.h>
#include <functional>

class ArmpiControl{
public:
  ArmpiControl(ros::NodeHandle& nh);
  ~ArmpiControl();
private:
  ros::NodeHandle nh_;
  ArmpiCommandSubscriber *command_subscriber_;
  ArmpiChassis armpi_chassis_;
};
