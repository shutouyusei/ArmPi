#pragma once
#include <ros/ros.h>
#include <armpi_operation_msgs/RobotCommand.h>

class ArmpiCommandSubscriber
{
public:
  ArmpiCommandSubscriber();
private:
  ros::NodeHandle nh_;
  ros::Subscriber sub_cmd_;
  int robot_state_ = 0;

  void cmdCallback(const armpi_operation_msgs::RobotCommand::ConstPtr &msg);
  std::string velocity2String(const std::vector<double>& velocity);
};
