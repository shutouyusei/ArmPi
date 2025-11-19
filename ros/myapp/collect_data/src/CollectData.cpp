#include <collect_data/CollectData.h>
#include <rosbag/bag.h>
#include <filesystem>
#include <string>
#include <sstream>
#include <iomanip>
#include <ctime>

CollectData::CollectData(ros::NodeHandle& nh,const std::string& task_name): nh_(nh),armpi_camera_(nh),collect_joint_(nh),collect_command_(nh),task_name_(task_name) {
}

CollectData::~CollectData() {
}

void CollectData::start() {
  ROS_INFO("Collecting data...");
  is_running_ = true;
  armpi_camera_.start();
  collect_joint_.start();
  collect_command_.start();
}

void CollectData::finish(bool save){
  ROS_INFO("Finished collecting data.");
  is_running_ = false;
  //---save ----
  if (save) {
    //get data
    std::vector<sensor_msgs::ImageConstPtr> images;
    std::vector<sensor_msgs::JointState::Ptr> joint_data;
    std::vector<armpi_operation_msgs::RobotCommand::Ptr> cmd_data;
    armpi_camera_.getCollectedImages(images);
    collect_joint_.getCollectedData(joint_data);
    collect_command_.getCollectedData(cmd_data);

    // save to rosbag
    if (images.empty() && joint_data.empty() && cmd_data.empty()) {
      ROS_WARN("No data collected. Skipping save");
    }else{
      std::string bag_path = generateBagFilename();
      saveToRosbag(bag_path, images, joint_data, cmd_data);
    }
  }else{
    ROS_INFO("Not saving data.");
  }

  //delete buffer
  armpi_camera_.finish();
  collect_joint_.finish();
  collect_command_.finish();
}

std::string CollectData::generateBagFilename(){
    std::time_t t = std::time(nullptr);
    std::tm tm = *std::localtime(&t);
    std::stringstream ss;

    ss << "/home/rosuser/ros_ws/datasets/" + task_name_ + "/data_" 
       << std::put_time(&tm, "%Y%m%d_%H%M%S")
       << ".bag";
    std::string full_path_str = ss.str();
    
    try {
        std::filesystem::path bag_path(full_path_str);
        std::filesystem::path dir_path = bag_path.parent_path();
        if (!std::filesystem::exists(dir_path)) {
            std::filesystem::create_directories(dir_path);
        }
    } catch (const std::filesystem::filesystem_error& e) {
        ROS_ERROR("Failed to create directory: %s", e.what());
    }
    return full_path_str;
}

void CollectData::saveToRosbag(const std::string& bag_filename, const std::vector<sensor_msgs::ImageConstPtr>& images, const std::vector<sensor_msgs::JointState::Ptr>& joint_data, const std::vector<armpi_operation_msgs::RobotCommand::Ptr>& cmd_data){
  rosbag::Bag bag;
  try {
    bag.open(bag_filename, rosbag::bagmode::Write);

    ROS_INFO("Saving data to %s ...", bag_filename.c_str());

    for (const auto& msg : images) {
      bag.write("/collected/image", msg->header.stamp, msg);
    }

    for (const auto& msg : joint_data) {
      bag.write("/collected/joint_states", msg->header.stamp, msg);
    }

    for (const auto& msg : cmd_data) {
      bag.write("/collected/command", msg->header.stamp, msg);
    }

    bag.close();
    ROS_INFO("... Save complete.");

  } catch (rosbag::BagException& e) {
    ROS_ERROR("Failed to write to rosbag: %s", e.what());
  }
}
