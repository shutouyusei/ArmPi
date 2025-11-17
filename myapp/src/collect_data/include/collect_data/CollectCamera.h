#pragma once
#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <vector>
#include <deque>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>

class ArmpiCamera {
public:
  ArmpiCamera(ros::NodeHandle& nh);
  ~ArmpiCamera();

  void start();
  void finish();

  void getCollectedImages(std::vector<sensor_msgs::ImageConstPtr>& images);

private:
  void imageCallback(const sensor_msgs::ImageConstPtr& msg);
  void processingThreadLoop();
private:
  std::vector<sensor_msgs::ImageConstPtr> collected_images_;
  ros::NodeHandle nh_;
  image_transport::ImageTransport it_;
  image_transport::Subscriber sub_;

  std::deque<sensor_msgs::ImageConstPtr> image_queue_;
  std::mutex queue_mutex_;
  std::condition_variable queue_cv_;
  std::thread worker_thread_;
  std::atomic<bool> shutdown_requested_;
};
