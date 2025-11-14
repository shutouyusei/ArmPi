#pragma once
#include <armpi_servo/ComputeArmIK.h>
#include <ros/ros.h>

struct ArmCommand {
  int arm_x = 0;
  int arm_y = 0;
  int arm_z = 0;
  int arm_alpha = 0;
  int arm_alpha1 = 0;
  int arm_alpha2 = 0;
  int gripper_close = 0;
};

struct ArmPos {
  float x = 0.0;
  float y = 0.0;
  float z = 0.0;
  float alpha = -90.0;
  float alpha1 = -180.0;
  float alpha2 = 0.0;
  float gripper = 0.0;
};

class ArmpiServo {
public:
  ArmpiServo(ros::NodeHandle &nh);

  ~ArmpiServo();

  bool requestArmMove(const ArmCommand &command);

private:
  ArmPos calArmPos(const ArmCommand &command);
  float move(const int arm);
  float grab(const int gripper_close);

private:
  ros::NodeHandle nh_;
  ros::ServiceClient ik_client_;
  const std::string ik_service_name_;

  ArmPos current_armpos;

  const ArmPos;
  // armpi controller constantsk
  const float IK_STEP = 0.005;
  const float GRIPPER_STEP = 10;
  void waitForService();
};
