#include <collect_data/CollectJoint.h>

CollectJoint::CollectJoint(ros::NodeHandle& nh): nh_(nh) {
  collected_data_.reserve(30000);
  ROS_INFO("Setup CollectJoint");
}

CollectJoint::~CollectJoint() {}

void CollectJoint::start() {
  ROS_INFO("Collecting joint data...");
  sub_ = nh_.subscribe("/joint_states", 1, &CollectJoint::jointStateCallback,this);
}

void CollectJoint::finish(){
  ROS_INFO("Finished collecting joint data.");
  sub_.shutdown();
  //delete buffer
  collected_data_.clear();
}

void CollectJoint::jointStateCallback(const sensor_msgs::JointState::ConstPtr& msg) {
  collected_data_.push_back(*msg);
}

void CollectJoint::getCollectedData(std::vector<sensor_msgs::JointState>& data) {
  data = collected_data_;
}
