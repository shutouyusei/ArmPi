#include <armpi_controller/ArmpiController.h>
#include <std_msgs/Empty.h>
#include <ros/callback_queue.h>
#include <boost/bind.hpp>

ArmpiController::ArmpiController(ros::NodeHandle &nh, const std::string &node_name, const std::string &task_name) : nh_(nh), collect_data_(nh, task_name), node_name_(node_name){
  pub_= nh_.advertise<armpi_operation_msgs::RobotCommand>("armpi_command", 1);
  pub_reset_servo_ = nh_.advertise<std_msgs::Empty>("reset_servo", 1);
  ROS_INFO_STREAM("ArmpiController for task " << task_name << " is started");
}

ArmpiController::~ArmpiController() {
}

void ArmpiController::finish(){
  controller_timer_.stop();
  resetServo();
  pub_.shutdown();
  pub_reset_servo_.shutdown();
}

void ArmpiController::reset() {
  // initialize armpi
  cmd_.chassis_move_forward = 0;
  cmd_.chassis_move_right = 0;
  cmd_.angular_right = 0;
  cmd_.arm_x = 0;
  cmd_.arm_y = 0;
  cmd_.arm_z = 0;
  cmd_.arm_alpha = 0;
  cmd_.rotation= 0;
  cmd_.gripper_close = 0;
}

void ArmpiController::resetServo(){
  ROS_INFO_STREAM("resetServo");
  pub_reset_servo_.publish(std_msgs::Empty());
  reset();
  pub_.publish(cmd_);
  ros::Duration(1.0).sleep();
}

void ArmpiController::start() {
  resetServo();
  ROS_INFO_STREAM("ArmpiController for task " << node_name_ << " is started");
  controller_timer_ = nh_.createTimer(ros::Duration(0.05),&ArmpiController::controllerLoop, this);
}

void ArmpiController::controllerLoop(const ros::TimerEvent& e) {
  reset();
  this->getCommand();
  pub_.publish(cmd_);
}
