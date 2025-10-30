#include <armpi_driver/ArmpiDriver.h>
#include <iostream>

void ArmpiDriver::publishChassisCommand(const geometry_msgs::Twist& base_velocity) {
    geometry_msgs::Twist twist_cmd;

    twist_cmd.linear.x = base_velocity.linear.x; // Vx
    twist_cmd.angular.z = base_velocity.angular.z; // Wz
    
    pub_chassis_velocity_.publish(twist_cmd);
}

ArmpiDriver::ArmpiDriver(ros::NodeHandle& nh) : nh_(nh) {
    pub_chassis_velocity_ = nh_.advertise<chassis_control::SetVelocity>("/chassis_control/set_velocity", 10); 
    
    ROS_INFO("ArmpiDriver Node Ready. Publishing to /cmd_vel.");
}

ArmpiDriver::~ArmpiDriver() {
    geometry_msgs::Twist stop_cmd;
    stop_cmd.linear.x = 0.0;
    stop_cmd.angular.z = 0.0;
    pub_chassis_velocity_.publish(stop_cmd);
    
    ROS_WARN("ArmpiDriver shutting down. Zero velocity published.");
}
