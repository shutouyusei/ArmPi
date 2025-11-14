#pragma once
#include <armpi_command_publisher/ArmpiCommandPublisher.h>
#include <collect_data/CollectData.h>
#include <ros/ros.h>
#include <thread>

class ArmpiController {
public:
  ArmpiController(ros::NodeHandle &nh, const std::string &node_name,
                  const std::string &task_name);

  virtual ~ArmpiController();

  void start();

protected:
  virtual void getCommand() = 0;
  void reset();

private:
  void controllerLoop();

protected:
  ros::NodeHandle nh_;
  std::string node_name_;
  CollectData collect_data_;

  armpi_operation_msgs::RobotCommand cmd_;

private:
  ArmpiCommandPublisher command_publisher_;
  bool running_;
  std::thread input_thread_;
};
