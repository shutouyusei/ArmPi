#include <armpi_control/ArmpiControl.h>
#include <ros/ros.h>

ArmpiControl::ArmpiControl(ros::NodeHandle& nh):nh_(nh),armpi_chassis_(nh),armpi_servo_(nh) {
  command_subscriber_ = new ArmpiCommandSubscriber(
    nh,
    [this](const geometry_msgs::Twist& base_velocity){armpi_chassis_.publishChassisCommand(base_velocity);},
    [this](double x,double y,double z,double gripper,double alpha,double alpha1,double alpha2){return armpi_servo_.requestArmMove(x,y,z,gripper,alpha,alpha1,alpha2);}
  );
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
