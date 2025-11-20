#pragma once
#include <ros/ros.h>
#include <armpi_operation_msgs/RobotCommand.h>
#include <vector>
#include <deque>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>

class CollectCommand {
public:
  CollectCommand(ros::NodeHandle& nh);
  ~CollectCommand();
  void start();
  void finish();
  void getCollectedData(std::vector<armpi_operation_msgs::RobotCommand::Ptr>& data);

private:
  void cmdCallback(const armpi_operation_msgs::RobotCommand::ConstPtr& msg);
  void processingThreadLoop();
private:
  ros::NodeHandle nh_;
  ros::Subscriber sub_;
  std::vector<armpi_operation_msgs::RobotCommand::Ptr> collected_data_;

  std::deque<armpi_operation_msgs::RobotCommand::ConstPtr> cmd_queue_;
  std::mutex queue_mutex_;
  std::condition_variable queue_cv_;
  std::thread worker_thread_;
  std::atomic<bool> shutdown_requested_;
};
