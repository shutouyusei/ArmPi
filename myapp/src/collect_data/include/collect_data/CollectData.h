#pragma once
#include <ros/ros.h>

class CollectData
{
public:
  CollectData(ros::NodeHandle& nh);
  ~CollectData();
  void start();
  void finish();
  bool is_running_=false; 

private:
  ros::NodeHandle nh_;
  ros::Subscriber sub_;
};
