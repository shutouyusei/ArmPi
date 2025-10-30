#pragma once
#include <ros/ros.h>
#include <armpi_operation_msgs/RobotCommand.h>
#include <functional>
#include <geometry_msgs/Twist.h>

class ArmpiCommandSubscriber
{
public:
  ArmpiCommandSubscriber(ros::NodeHandle& nh, std::function<void(const geometry_msgs::Twist&)> drive_function,std::function<void()> arm_function);
private:
  std::function<void(const geometry_msgs::Twist&)> drive_function_;
  std::function<void()> arm_function_; 

  ros::NodeHandle nh_;
  ros::Subscriber sub_cmd_;
  int robot_state_ = 0;

  void cmdCallback(const armpi_operation_msgs::RobotCommand::ConstPtr &msg);
  std::string velocity2String(const std::vector<double>& velocity);
};
