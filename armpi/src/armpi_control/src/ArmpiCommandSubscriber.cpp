#include <armpi_control/ArmpiCommandSubscriber.h>
#include <vector>
#include <string>

ArmpiCommandSubscriber::ArmpiCommandSubscriber(ros::NodeHandle& nh,std::function<void(const geometry_msgs::Twist&)> drive_function,std::function<void()> arm_function):
  nh_(nh),drive_function_(drive_function),arm_function_(arm_function) {
  sub_cmd_ = nh_.subscribe("armpi_command", 10, &ArmpiCommandSubscriber::cmdCallback, this);

  ROS_INFO("ArmpiCommandSubscriber init");
}

void ArmpiCommandSubscriber::cmdCallback(const armpi_operation_msgs::RobotCommand::ConstPtr& msg)
{
  ROS_INFO("armpi_command received");
  drive_function_(msg->base_velocity);
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
