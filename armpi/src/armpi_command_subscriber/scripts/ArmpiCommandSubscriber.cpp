#include "ArmpiCommandSubscriber.h"
#include <vector>
#include <string>

ArmpiCommandSubscriber::ArmpiCommandSubscriber(){
  sub_cmd_ = nh_.subscribe("armpi_command", 10, &ArmpiCommandSubscriber::cmdCallback, this);

  ROS_INFO("ArmpiCommandSubscriber init");
}

void ArmpiCommandSubscriber::cmdCallback(const armpi_operation_msgs::RobotCommand::ConstPtr& msg)
{
  ROS_INFO("I heard: [%s]", velocity2String(msg->arm_joint_velocities).c_str());
  //send armpi_status
}

std::string ArmpiCommandSubscriber::velocity2String(const std::vector<double>& velocity) {
  std::string joint_velocities_str = "";
  for (size_t i = 0; i < velocity.size(); ++i)
  {
      joint_velocities_str += std::to_string(velocity[i]);
      if (i < velocity.size() - 1)
      {
          joint_velocities_str += ", ";
      }
  }
  return joint_velocities_str;
}

int main(int argc, char **argv)
{
  ros::init(argc, argv, "armpi_command_listner");

  ArmpiCommandSubscriber armpi_command_subscriber;

  ros::spin();

  return 0;
}
