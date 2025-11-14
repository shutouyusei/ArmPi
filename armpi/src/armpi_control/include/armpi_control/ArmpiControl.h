#pragma once
#include <armpi_chassis/ArmpiChassis.h>
#include <armpi_servo/ArmpiServo.h>
#include <ros/ros.h>

class ArmpiControl {
public:
  ArmpiControl(ros::NodeHandle &nh);

private:
  ros::NodeHandle nh_;
  ros::Subscriber sub_cmd_;
  ros::Publisher pub_;
  ArmpiChassis armpi_chassis_;
  ArmpiServo armpi_servo_;

private:
  void cmdCallback(const armpi_operation_msgs::RobotCommand::ConstPtr &msg);
};
