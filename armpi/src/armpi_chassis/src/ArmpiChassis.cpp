#include <armpi_chassis/ArmpiChassis.h>
#include <iostream>

void ArmpiChassis::publishChassisCommand(const ChassisCommand &command) {
  chassis_control::SetVelocity cmd;
  cmd.velocity = this->getMove(command.move_forward);
  cmd.angular = this->angular(command.angular);
  cmd.direction = 90.0;
  pub_chassis_velocity_.publish(cmd);
}

ArmpiChassis::ArmpiChassis(ros::NodeHandle &nh) : nh_(nh) {
  pub_chassis_velocity_ = nh_.advertise<chassis_control::SetVelocity>(
      "/chassis_control/set_velocity", 1);
}

float ArmpiChassis::getMove(const int move) {
  switch (move) {
  case 1:
    return MAX_SPEED;
  case -1:
    return -MAX_SPEED;
  default:
    return 0;
  }
}

float ArmpiChassis::getAngular(const int angular) {
  switch (angular) {
  case 1:
    return MAX_TURN;
  case -1:
    return -MAX_TURN;
  default:
    return 0;
  }
}

ArmpiChassis::~ArmpiChassis() {
  // STOP
  publishChassisCommand(0, 0, 0);
  ROS_WARN("ArmpiChassis shutting down. Zero velocity published.");
}
