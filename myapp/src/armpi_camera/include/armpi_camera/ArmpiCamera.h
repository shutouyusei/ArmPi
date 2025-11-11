#pragma once
#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <vector>


class ArmpiCamera {
public:
  ArmpiCamera(ros::NodeHandle& nh);
  ~ArmpiCamera();

  void start();
  void finish();

  void getCollectedImages(std::vector<sensor_msgs::ImageConstPtr>& images);

private:
  void imageCallback(const sensor_msgs::ImageConstPtr& msg);
private:
  std::vector<sensor_msgs::ImageConstPtr> collected_images_;
  ros::NodeHandle nh_;
  image_transport::ImageTransport it_;
  image_transport::Subscriber sub_;
};
