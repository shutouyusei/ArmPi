#pragma once
#include <armpi_operation_msgs/RobotCommand.h>
#include <ros/ros.h>

class ArmpiController {
public:
  ArmpiController(ros::NodeHandle &nh, const std::string &node_name);
  virtual ~ArmpiController();

  void start();
  void resetServo();
  virtual void finish();
protected:
  virtual void getCommand() = 0;
  void reset();

private:
  void controllerLoop(const ros::TimerEvent &e);

//----
protected:
  ros::NodeHandle nh_;
  std::string node_name_;

  // robot command
  armpi_operation_msgs::RobotCommand cmd_;

private:
  ros::Publisher pub_;   
  ros::Publisher pub_reset_servo_;   
  ros::Timer controller_timer_;
};
