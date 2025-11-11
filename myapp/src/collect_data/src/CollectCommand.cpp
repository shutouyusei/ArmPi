#include <collect_data/CollectCommand.h>

CollectCommand::CollectCommand(ros::NodeHandle& nh): nh_(nh) {
  collected_data_.reserve(30000);
  ROS_INFO("Setup CollectCommand");
}

CollectCommand::~CollectCommand() {}

void CollectCommand::start() {
  ROS_INFO("Collecting joint data...");
  sub_ = nh_.subscribe("armpi_command", 1, &CollectCommand::cmdCallback, this);
}

void CollectCommand::finish(){
  ROS_INFO("Finished collecting joint data.");
  sub_.shutdown();
  //delete buffer
  collected_data_.clear();
}

void CollectCommand::cmdCallback(const armpi_operation_msgs::RobotCommand::ConstPtr& msg){
  collected_data_.push_back(*msg);
}

void CollectCommand::getCollectedData(std::vector<armpi_operation_msgs::RobotCommand>& data){
  data = collected_data_;
}
