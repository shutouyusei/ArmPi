#pragma once

#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <chassis_control/SetVelocity.h>


class ArmpiChassis {
public:
    ArmpiChassis(ros::NodeHandle& nh);
    ~ArmpiChassis();

    void publishChassisCommand(const geometry_msgs::Twist& base_velocity);
private:
    ros::NodeHandle nh_;
    ros::Publisher pub_chassis_velocity_;
    
    const double LINEAR_SCALE = 0.005; 
    const double ANGULAR_SCALE = 0.005;

};
