#include <armpi_camera/ArmpiCamera.h>

ArmpiCamera::ArmpiCamera(ros::NodeHandle& nh):nh_(nh),it_(nh){
  collected_images_.reserve(30000);
  ROS_INFO("Setup Subscriber for image");
}

ArmpiCamera::~ArmpiCamera(){}

void ArmpiCamera::imageCallback(const sensor_msgs::ImageConstPtr& msg) {
  // save images
  collected_images_.push_back(msg);
}

void ArmpiCamera::start() {
  ROS_INFO("Starting ArmpiCamera...");
  sub_ = it_.subscribe("/usb_cam/image_raw", 1, &ArmpiCamera::imageCallback,this);
}

void ArmpiCamera::finish() {
  ROS_INFO("Shutting down ArmpiCamera...");
  sub_.shutdown();
  // clear buffer
  collected_images_.clear();
}

void ArmpiCamera::getCollectedImages(std::vector<sensor_msgs::ImageConstPtr>& images) {
  images = collected_images_;
}
