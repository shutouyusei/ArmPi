#include <collect_data/CollectData.h>

CollectData::CollectData(ros::NodeHandle& nh): nh_(nh) {
}

CollectData::~CollectData() {
}

void CollectData::start() {
  ROS_INFO("Collecting data...");
  is_running_ = true;
}

void CollectData::finish(){
  ROS_INFO("Finished collecting data.");
  is_running_ = false;
  //save
  //delete buffer
}
