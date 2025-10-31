#include <armpi_control/ArmpiControl.h>
#include <ros/ros.h>

ArmpiControl::ArmpiControl(ros::NodeHandle& nh):nh_(nh),armpi_chassis_(nh) {
  command_subscriber_ = new ArmpiCommandSubscriber(
    nh,
    [this](const geometry_msgs::Twist& base_velocity){armpi_chassis_.publishChassisCommand(base_velocity);},
    []{ROS_INFO("Arm Function is not implemented.");});
}

ArmpiControl::~ArmpiControl() {
  delete command_subscriber_;
}

int main(int argc, char **argv)
{
  ros::init(argc, argv, "armpi_control");
  ros::NodeHandle nh;
  ArmpiControl armpi_control(nh);
  ROS_INFO("Robot chassis Initilized. Waiting for commands ...");

  ros::spin();
  
  return 0;
}
