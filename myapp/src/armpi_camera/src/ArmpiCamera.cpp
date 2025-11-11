#include <armpi_camera/ArmpiCamera.h>

ArmpiCamera::ArmpiCamera(ros::NodeHandle& nh):nh_(nh),it_(nh){
  ROS_INFO("Setup Subscriber for image");
}

ArmpiCamera::~ArmpiCamera(){}

void ArmpiCamera::imageCallback(const sensor_msgs::ImageConstPtr& msg) {
  try {
    cv_bridge::CvImageConstPtr cv_ptr = cv_bridge::toCvShare(msg, sensor_msgs::image_encodings::BGR8);
    // save images
    collected_images_.push_back(cv_ptr);
  } catch (cv_bridge::Exception& e) {
    ROS_ERROR("cv_bridge exception: %s", e.what());
    return;
  }
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

void ArmpiCamera::getCollectedImages(std::vector<cv_bridge::CvImageConstPtr>& images) {
  images = collected_images_;
}
