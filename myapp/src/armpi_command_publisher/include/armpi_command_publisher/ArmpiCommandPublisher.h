#pragma once
#include <ros/ros.h>
#include <armpi_operation_msgs/RobotCommand.h>

class ArmpiCommandPublisher
{
public:
  ArmpiCommandPublisher(ros::NodeHandle& nh);
  void sendCommand(const armpi_operation_msgs::RobotCommand& msg);
private:
  // create node handle
  ros::NodeHandle nh_;
  // create publisher
  ros::Publisher pub_;   
};
