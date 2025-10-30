#include <armpi_controller/ArmpiController.h>
#include <armpi_operation_msgs/RobotCommand.h>

int main(int argc, char **argv){
  ros::init(argc, argv, "armpi_controller");
  armpi_operation_msgs::RobotCommand robot_command;

  // 1. ベース速度の設定
  robot_command.base_velocity.linear.x = 0.5;
  robot_command.base_velocity.linear.y = 0.0;
  robot_command.base_velocity.angular.z = 0.1;

  // 2. アームの関節速度の設定 (4 DOFを想定)
  robot_command.arm_joint_velocities.resize(4);
  robot_command.arm_joint_velocities[0] = 0.8;
  robot_command.arm_joint_velocities[1] = -0.3;
  robot_command.arm_joint_velocities[2] = 0.0;
  robot_command.arm_joint_velocities[3] = 0.0;

  // 3. グリッパー位置の設定
  robot_command.gripper_position = 0.75;

  ArmpiController armpi_controller;
  armpi_controller.armpi_command_publisher_.sendCommand(robot_command);
  ros::spin();
  return 0;
}
