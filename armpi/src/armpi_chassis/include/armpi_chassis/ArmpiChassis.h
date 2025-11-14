#pragma once

#include <chassis_control/SetVelocity.h>
#include <ros/ros.h>

struct ChassisCommand {
  int move_forward = 0;
  int move_right = 0;
  int angular = 0;
};

class ArmpiChassis {
public:
  ArmpiChassis(ros::NodeHandle &nh);
  ~ArmpiChassis();

  void publishChassisCommand(const ChassisCommand &command);

private:
  float getMove(const int move);
  float getAngular(const int angular);

private:
  ros::NodeHandle nh_;
  ros::Publisher pub_chassis_velocity_;

  const float MAX_SPEED = 100.0;
  const float MAX_TURN = 0.5;
};
