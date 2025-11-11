#include <collect_data/CollectJoint.h>
#include <boost/make_shared.hpp>

CollectJoint::CollectJoint(ros::NodeHandle& nh): nh_(nh) {
  collected_data_.reserve(30000);
  ROS_INFO("Setup CollectJoint");
}

CollectJoint::~CollectJoint() {}

void CollectJoint::start() {
  ROS_INFO("Collecting joint data...");
  collected_data_.clear();
  joint_state_queue_.clear();
  shutdown_requested_ = false;

  worker_thread_ = std::thread(&CollectJoint::processingThreadLoop, this);
  sub_ = nh_.subscribe("/joint_states", 1, &CollectJoint::jointStateCallback,this);
}

void CollectJoint::finish(){
  if (shutdown_requested_) return;
  ROS_INFO("Shutting down CollectCommand(Async)...");
  sub_.shutdown();

  shutdown_requested_ = true;
  queue_cv_.notify_one(); 
  if (worker_thread_.joinable()) {
    worker_thread_.join();
  }

  ROS_INFO("CollectCommand shutdown complete.");
}

void CollectJoint::jointStateCallback(const sensor_msgs::JointState::ConstPtr& msg) {
  {
    std::lock_guard<std::mutex> lock(queue_mutex_);
    joint_state_queue_.push_back(msg);
  }

  queue_cv_.notify_one();
}

void CollectJoint::processingThreadLoop(){
  while (ros::ok() && !shutdown_requested_) {
    sensor_msgs::JointState::Ptr joint_state = boost::make_shared<sensor_msgs::JointState>();
    { 
      std::unique_lock<std::mutex> lock(queue_mutex_);
      queue_cv_.wait(lock, [this] { return shutdown_requested_ || !joint_state_queue_.empty(); });
      if (shutdown_requested_) break;

      *joint_state = *(joint_state_queue_.front());
      joint_state_queue_.pop_front();
    }
    collected_data_.push_back(joint_state);
  }
}

void CollectJoint::getCollectedData(std::vector<sensor_msgs::JointState::Ptr>& data) {
  data = collected_data_;
}
