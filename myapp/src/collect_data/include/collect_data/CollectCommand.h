#pragma once
#include <ros/ros.h>
#include <armpi_operation_msgs/RobotCommand.h>
#include <vector>

class CollectCommand
{
public:
  CollectCommand(ros::NodeHandle& nh);
  ~CollectCommand();
  void start();
  void finish();
  void getCollectedData(std::vector<armpi_operation_msgs::RobotCommand>& data);

private:
  void cmdCallback(const armpi_operation_msgs::RobotCommand::ConstPtr& msg);
  ros::NodeHandle nh_;
  ros::Subscriber sub_;
  std::vector<armpi_operation_msgs::RobotCommand> collected_data_;
};
