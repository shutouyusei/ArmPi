#pragma once
#include <ros/ros.h>
#include <sensor_msgs/JointState.h>
#include <vector>

class CollectJoint
{
public:
  CollectJoint(ros::NodeHandle& nh);
  ~CollectJoint();
  void start();
  void finish();

private:
  void jointStateCallback(const sensor_msgs::JointState::ConstPtr& msg);
  ros::NodeHandle nh_;
  ros::Subscriber sub_;
  std::vector<sensor_msgs::JointState> collected_data_;
};
