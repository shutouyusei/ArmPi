#include <armpi_command_publisher/ArmpiCommandPublisher.h>

ArmpiCommandPublisher::ArmpiCommandPublisher(){
  pub_= nh_.advertise<armpi_operation_msgs::RobotCommand>("armpi_command", 10);

  ROS_INFO("ArmpiCommandPublisher initialized on /armpi_command.");
  
  ROS_INFO("Waiting for subscriber connection...");
  ros::Rate connect_rate(1);
  while (pub_.getNumSubscribers() < 1 && ros::ok()) { // 修正: ros::ok() を追加
      ros::spinOnce(); 
      connect_rate.sleep();
  }
  ROS_INFO("Subscriber connected.");
}

void ArmpiCommandPublisher::sendCommand(const armpi_operation_msgs::RobotCommand& msg){
  pub_.publish(msg);
  ROS_INFO("Command sent: Base Velo(x)=%.2f", msg.base_velocity.linear.x);
}
