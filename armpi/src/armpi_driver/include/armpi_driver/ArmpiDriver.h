#pragma once

#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <chassis_control/SetVelocity.h>


class ArmpiDriver {
public:
    ArmpiDriver(ros::NodeHandle& nh);
    ~ArmpiDriver();

    void publishChassisCommand(const geometry_msgs::Twist& base_velocity);
private:
    ros::NodeHandle nh_;
    ros::Publisher pub_chassis_velocity_;
    
    const double LINEAR_SCALE = 0.005; 
    const double ANGULAR_SCALE = 0.005;

};
