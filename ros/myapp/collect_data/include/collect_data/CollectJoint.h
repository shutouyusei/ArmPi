#pragma once
#include <ros/ros.h>
#include <sensor_msgs/JointState.h>
#include <vector>
#include <deque>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>

class CollectJoint
{
public:
  CollectJoint(ros::NodeHandle& nh);
  ~CollectJoint();
  void start();
  void finish();
  void getCollectedData(std::vector<sensor_msgs::JointState::Ptr>& data);

private:
  void jointStateCallback(const sensor_msgs::JointState::ConstPtr& msg);
  void processingThreadLoop();
private:
  ros::NodeHandle nh_;
  ros::Subscriber sub_;
  std::vector<sensor_msgs::JointState::Ptr> collected_data_;

  std::deque<sensor_msgs::JointState::ConstPtr> joint_state_queue_;
  std::mutex queue_mutex_;
  std::condition_variable queue_cv_;
  std::thread worker_thread_;
  std::atomic<bool> shutdown_requested_;
};
