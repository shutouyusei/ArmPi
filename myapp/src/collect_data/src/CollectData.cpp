#include <collect_data/CollectData.h>

CollectData::CollectData(ros::NodeHandle& nh): nh_(nh) {
}

CollectData::~CollectData() {
  this->finish();
}

void CollectData::start() {
  is_running_ = true;
  sub_ = nh_.subscribe("/joint_states", 10, &CollectData::jointStateCallback,this);
}

void CollectData::finish(){
  is_running_ = false;
  sub_.shutdown();
  //save
  //delete buffer
  collected_data_.clear();
}

void CollectData::jointStateCallback(const sensor_msgs::JointState::ConstPtr& msg) {
  collected_data_.push_back(*msg);
}
