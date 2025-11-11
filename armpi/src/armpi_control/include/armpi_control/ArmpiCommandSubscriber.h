#pragma once
#include <ros/ros.h>
#include <armpi_operation_msgs/RobotCommand.h>
#include <functional>
#include <geometry_msgs/Twist.h>

class ArmpiCommandSubscriber
{
public:
  ArmpiCommandSubscriber(ros::NodeHandle& nh, std::function<void(const geometry_msgs::Twist&)> drive_function,std::function<bool(double,double,double,double,double,double,double)> arm_function);
private:
  std::function<void(const geometry_msgs::Twist&)> drive_function_;
  std::function<bool(double,double,double,double,double,double,double)> arm_function_; 

  ros::NodeHandle nh_;
  ros::Subscriber sub_cmd_;

  void cmdCallback(const armpi_operation_msgs::RobotCommand::ConstPtr &msg);
};
