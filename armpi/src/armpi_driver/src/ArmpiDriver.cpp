#include <armpi_driver/ArmpiDriver.h>
#include <iostream>

void ArmpiDriver::publishChassisCommand(const geometry_msgs::Twist& base_velocity) {
    chassis_control::SetVelocity cmd;

    cmd.velocity = base_velocity.linear.x; // Vx
    cmd.angular = base_velocity.angular.z; // Wz
    cmd.direction = 90.0;
    
    ROS_INFO("velocity: %f", cmd.velocity);
    ROS_INFO("angular: %f", cmd.angular);
    ROS_INFO("direction: %f", cmd.direction);
    pub_chassis_velocity_.publish(cmd);
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
