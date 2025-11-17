#pragma once
#include <armpi_servo/ComputeArmIK.h>
#include <ros/ros.h>
#include <std_msgs/Empty.h>

struct ArmCommand {
  int arm_x = 0;
  int arm_y = 0;
  int arm_z = 0;
  int arm_alpha = 0;
  int rotation = 0;
  int gripper_close = 0;
};

struct ArmPos {
  float x = 0.0;
  float y = 0.0;
  float z = 0.0;
  float alpha = -90.0;
  float rotation = 0;
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
  float alpha(const int alpha);
  float rotation(const int rotation);
  void reset();
  void requestReset(const std_msgs::Empty::ConstPtr &msg);
private:
  ros::NodeHandle nh_;
  ros::ServiceClient ik_client_;
  const std::string ik_service_name_;
  ros::Subscriber sub_;

  ArmPos current_armpos;
  // armpi controller constantsk
  const float IK_STEP = 0.005;
  const float GRIPPER_STEP = 10;
  const float ALPHA_STEP = 5;
  const float ROTATION_STEP = 10;
  void waitForService();
};
