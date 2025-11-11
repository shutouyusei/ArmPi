#include <armpi_chassis/ArmpiChassis.h>
#include <iostream>

void ArmpiChassis::publishChassisCommand(const geometry_msgs::Twist& base_velocity) {
    chassis_control::SetVelocity cmd;

    cmd.velocity = base_velocity.linear.x; // Vx
    cmd.angular = base_velocity.angular.z; // Wz
    cmd.direction = 90.0;
    
    ROS_INFO("velocity: %f", cmd.velocity);
    ROS_INFO("angular: %f", cmd.angular);
    ROS_INFO("direction: %f", cmd.direction);
    pub_chassis_velocity_.publish(cmd);
}

ArmpiChassis::ArmpiChassis(ros::NodeHandle& nh) : nh_(nh) {
    pub_chassis_velocity_ = nh_.advertise<chassis_control::SetVelocity>("/chassis_control/set_velocity", 1); 
    
}

ArmpiChassis::~ArmpiChassis() {
    geometry_msgs::Twist stop_cmd;
    stop_cmd.linear.x = 0.0;
    stop_cmd.angular.z = 0.0;
    pub_chassis_velocity_.publish(stop_cmd);
    
    ROS_WARN("ArmpiChassis shutting down. Zero velocity published.");
}
