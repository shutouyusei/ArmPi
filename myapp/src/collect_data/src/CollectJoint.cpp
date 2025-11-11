#include <collect_data/CollectJoint.h>

CollectJoint::CollectJoint(ros::NodeHandle& nh): nh_(nh) {
  ROS_INFO("Setup CollectJoint");
}

CollectJoint::~CollectJoint() {}

void CollectJoint::start() {
  ROS_INFO("Collecting joint data...");
  sub_ = nh_.subscribe("/joint_states", 10, &CollectJoint::jointStateCallback,this);
}

void CollectJoint::finish(){
  ROS_INFO("Finished collecting joint data.");
  sub_.shutdown();
  //save
  //delete buffer
  collected_data_.clear();
}

void CollectJoint::jointStateCallback(const sensor_msgs::JointState::ConstPtr& msg) {
  collected_data_.push_back(*msg);
}
