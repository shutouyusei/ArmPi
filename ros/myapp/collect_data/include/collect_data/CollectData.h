#pragma once
#include <ros/ros.h>
#include <collect_data/CollectCamera.h>
#include <collect_data/CollectData.h>
#include <collect_data/CollectJoint.h>
#include <collect_data/CollectCommand.h>
#include <armpi_operation_msgs/RobotCommand.h>
#include <vector>
#include <string>

class CollectData
{
public:
  CollectData(ros::NodeHandle& nh,const std::string& task_name);
  ~CollectData();
  void start();
  void finish(bool save);
  void collectComand(armpi_operation_msgs::RobotCommand& cmd);
  bool is_running_=false; 

private:
  void saveToRosbag(const std::string& bag_filename, const std::vector<sensor_msgs::ImageConstPtr>& images, const std::vector<sensor_msgs::JointState::Ptr>& joint_data, const std::vector<armpi_operation_msgs::RobotCommand::Ptr>& cmd_data);
  void saveCollectedData();
  static std::string generateBagFilename(const std::string task_name);
private:
  ros::NodeHandle nh_;
  ros::Subscriber sub_;
  const std::string task_name_;

  //data collecting modules
  ArmpiCamera armpi_camera_;
  CollectJoint collect_joint_;
  CollectCommand collect_command_;
};
