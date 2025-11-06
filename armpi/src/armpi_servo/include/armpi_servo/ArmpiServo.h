#pragma once
#include <ros/ros.h>
#include <armpi_servo/ComputeArmIK.h>

class ArmpiServo {
public:
    ArmpiServo(ros::NodeHandle& nh);

    ~ArmpiServo();

    bool requestArmMove(double x, double y, double z,double gripper, double alpha = -90.0, double alpha1 = -180.0, double alpha2 = 0.0);

private:
    ros::NodeHandle nh_;
    ros::ServiceClient ik_client_;
    const std::string ik_service_name_;

    void waitForService();
};
