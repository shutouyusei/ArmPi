#include <collect_data/CollectCommand.h>
#include <boost/make_shared.hpp>

CollectCommand::CollectCommand(ros::NodeHandle& nh): nh_(nh) {
  collected_data_.reserve(30000);
  ROS_INFO("Setup CollectCommand");
}

CollectCommand::~CollectCommand() {}

void CollectCommand::start() {
  if (shutdown_requested_ == false && worker_thread_.joinable()) {
    ROS_WARN("ArmpiCamera::start() called while already running. Ignoring.");
    return; 
  }
  ROS_INFO("Collecting joint data...");
  collected_data_.clear();
  cmd_queue_.clear();
  shutdown_requested_ = false;

  worker_thread_ = std::thread(&CollectCommand::processingThreadLoop, this);
  sub_ = nh_.subscribe("armpi_command", 1, &CollectCommand::cmdCallback, this);
}

void CollectCommand::finish(){
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

void CollectCommand::cmdCallback(const armpi_operation_msgs::RobotCommand::ConstPtr& msg){
  {
    std::lock_guard<std::mutex> lock(queue_mutex_);
    cmd_queue_.push_back(msg);
  }

  queue_cv_.notify_one();
}

void CollectCommand::getCollectedData(std::vector<armpi_operation_msgs::RobotCommand::Ptr>& data){
  data = collected_data_;
}

void CollectCommand::processingThreadLoop(){
  while (ros::ok() && !shutdown_requested_) {
    armpi_operation_msgs::RobotCommand::Ptr cmd = boost::make_shared<armpi_operation_msgs::RobotCommand>();
    { 
      std::unique_lock<std::mutex> lock(queue_mutex_);
      queue_cv_.wait(lock, [this] { return shutdown_requested_ || !cmd_queue_.empty(); });
      if (shutdown_requested_) break;

      *cmd = *(cmd_queue_.front());
      cmd_queue_.pop_front();
    }
    collected_data_.push_back(cmd);
  }
}
