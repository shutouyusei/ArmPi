#include <armpi_chassis/ArmpiChassis.h>
#include <iostream>

void ArmpiChassis::publishChassisCommand(const ChassisCommand &command) {
  chassis_control::SetVelocity cmd;
  cmd.velocity = this->getMove(command.move_forward);
  cmd.angular = this->getAngular(command.angular);
  cmd.direction = 90.0;
  pub_chassis_velocity_.publish(cmd);
}

ArmpiChassis::ArmpiChassis(ros::NodeHandle &nh) : nh_(nh) {
  pub_chassis_velocity_ = nh_.advertise<chassis_control::SetVelocity>(
      "/chassis_control/set_velocity", 1);
}

float ArmpiChassis::getMove(const int move) {
  return move * MAX_SPEED;
}

float ArmpiChassis::getAngular(const int angular) {
  return -angular * MAX_TURN;
}

ArmpiChassis::~ArmpiChassis() {
  publishChassisCommand(STOP);
  ROS_WARN("ArmpiChassis shutting down. Zero velocity published.");
}
