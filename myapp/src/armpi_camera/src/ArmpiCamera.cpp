#include <armpi_camera/ArmpiCamera.h>
#include <boost/make_shared.hpp>

ArmpiCamera::ArmpiCamera(ros::NodeHandle& nh):nh_(nh),it_(nh){
  ROS_INFO("Setup Subscriber for image");
}

ArmpiCamera::~ArmpiCamera(){}

void ArmpiCamera::imageCallback(const sensor_msgs::ImageConstPtr& msg) {
  {
    std::lock_guard<std::mutex> lock(queue_mutex_);
    image_queue_.push_back(msg);
  }

  queue_cv_.notify_one();
}

void ArmpiCamera::start() {
  if (shutdown_requested_ == false && worker_thread_.joinable()) {
    ROS_WARN("ArmpiCamera::start() called while already running. Ignoring.");
    return; 
  }
  ROS_INFO("Starting ArmpiCamera...");
  collected_images_.clear();
  image_queue_.clear();
  collected_images_.reserve(30000);
  shutdown_requested_ = false;

  worker_thread_ = std::thread(&ArmpiCamera::processingThreadLoop, this);
  sub_ = it_.subscribe("/usb_cam/image_raw", 1, &ArmpiCamera::imageCallback,this);
}

void ArmpiCamera::finish() {
  if (shutdown_requested_) return;
  ROS_INFO("Shutting down ArmpiCamera (Async)...");
  sub_.shutdown();

  shutdown_requested_ = true;
  queue_cv_.notify_one(); 
  if (worker_thread_.joinable()) {
    worker_thread_.join();
  }

  ROS_INFO("ArmpiCamera shutdown complete.");
}


void ArmpiCamera::processingThreadLoop() {
  while (ros::ok() && !shutdown_requested_) {
    sensor_msgs::ImagePtr msg_to_process = boost::make_shared<sensor_msgs::Image>();
    {
      std::unique_lock<std::mutex> lock(queue_mutex_);
      queue_cv_.wait(lock, [this]{ return !image_queue_.empty() || shutdown_requested_; });
      if (shutdown_requested_ && image_queue_.empty()) break;

      *msg_to_process = *(image_queue_.front());
      image_queue_.pop_front();
    }
    collected_images_.push_back(msg_to_process);
  }
}

void ArmpiCamera::getCollectedImages(std::vector<sensor_msgs::ImageConstPtr>& images) {
  images = collected_images_;
}
