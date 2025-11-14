#include <armpi_control/ArmpiControl.h>
#include <ros/ros.h>

ArmpiControl::ArmpiControl(ros::NodeHandle &nh)
    : nh_(nh), armpi_chassis_(nh), armpi_servo_(nh) {
  sub_cmd_ =
      nh_.subscribe("armpi_command", 1, &ArmpiControl::cmdCallback, this);

  pub_ = nh_.advertise<armpi_operation_msgs::RobotCommand>("get_command", 1);

  ROS_INFO("ArmpiCommandSubscriber init");
}

void ArmpiControl::cmdCallback(
    const armpi_operation_msgs::RobotCommand::ConstPtr &msg) {
  armpi_operation_msgs::RobotCommand new_msg = *msg;
  new_msg.header.stamp = ros::Time::now();
  new_msg.header.frame_id = "base_link";
  pub_.publish(new_msg);

  // chassis
  const ChassisCommand chassis_cmd = {msg->move_forward, msg->move_right,
                                      msg->angular};
  armpi_chassis_.publishChassisCommand(chassis_cmd);

  // servo
  const ArmCommand arm_command = {
      msg->arm_x,      msg->arm_y,      msg->arm_z,        msg->arm_alpha,
      msg->arm_alpha1, msg->arm_alpha2, msg->gripper_close};
  armpi_servo_.requestArmMove(arm_command);
}

int main(int argc, char **argv) {
  ros::init(argc, argv, "armpi_control");
  ros::NodeHandle nh;
  ArmpiControl armpi_control(nh);
  ROS_INFO("Robot chassis Initilized. Waiting for commands ...");

  ros::spin();

  return 0;
}
