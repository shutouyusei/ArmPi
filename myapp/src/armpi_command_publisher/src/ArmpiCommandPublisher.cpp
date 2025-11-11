#include <armpi_command_publisher/ArmpiCommandPublisher.h>

ArmpiCommandPublisher::ArmpiCommandPublisher(ros::NodeHandle& nh): nh_(nh) {
  pub_= nh_.advertise<armpi_operation_msgs::RobotCommand>("armpi_command", 1);


  ROS_INFO("ArmpiCommandPublisher initialized on /armpi_command.");
  
  ROS_INFO("Waiting for subscriber connection...");
  ros::Rate connect_rate(1);
  while (pub_.getNumSubscribers() < 1 && ros::ok()) {
      ros::spinOnce(); 
      connect_rate.sleep();
  }
  ROS_INFO("Subscriber connected.");
}

void ArmpiCommandPublisher::sendCommand(armpi_operation_msgs::RobotCommand& msg){
  pub_.publish(msg);
}
