#include <armpi_control/ArmpiCommandSubscriber.h>
#include <vector>
#include <string>

ArmpiCommandSubscriber::ArmpiCommandSubscriber(ros::NodeHandle& nh,std::function<void(const geometry_msgs::Twist&)> drive_function,std::function<bool(double,double,double,double,double,double,double)> arm_function):
  nh_(nh),drive_function_(drive_function),arm_function_(arm_function) {
  sub_cmd_ = nh_.subscribe("armpi_command", 1, &ArmpiCommandSubscriber::cmdCallback, this);

  pub_= nh_.advertise<armpi_operation_msgs::RobotCommand>("get_command", 1);

  ROS_INFO("ArmpiCommandSubscriber init");
}

void ArmpiCommandSubscriber::cmdCallback(const armpi_operation_msgs::RobotCommand::ConstPtr& msg)
{
  armpi_operation_msgs::RobotCommand new_msg = *msg;
  new_msg.header.stamp = ros::Time::now();
  new_msg.header.frame_id = "base_link";
  pub_.publish(new_msg);

  ROS_INFO("armpi_command received");
  drive_function_(msg->base_velocity);
  arm_function_(msg->arm_x,msg->arm_y,msg->arm_z,msg->gripper,msg->arm_alpha,msg->arm_alpha1,msg->arm_alpha2);
}
