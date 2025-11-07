#include <collect_data/CollectData.h>

CollectData::CollectData(ros::NodeHandle& nh): nh_(nh) {
}

CollectData::~CollectData() {
  this->finish();
}

void CollectData::start() {
  sub_ = nh_.subscribe("/joint_states", 10, &CollectData::jointStateCallback,this);
}

void CollectData::finish(){
  sub_.shutdown();
}

void CollectData::jointStateCallback(const sensor_msgs::JointState::ConstPtr& msg) {
  collected_data_.push_back(*msg);
}
