#pragma once

#include "ArmpiController.h"
#include <termios.h>
#include <unistd.h>
#include <thread>
#include <armpi_operation_msgs/RobotCommand.h>

class KeyboardController : public ArmpiController {
public:
  KeyboardController(ros::NodeHandle& nh);
  ~KeyboardController() override;

  void start() override;

private:
  std::thread input_thread_;

  const float MAX_SPEED = 100.0; 
  const float MAX_TURN = 0.5; 
  const float IK_STEP = 0.005;
  const float GRIPPER_STEP = 10;
  const int KEY_BUFFER_SIZE = 16;


  armpi_operation_msgs::RobotCommand cmd_;

  void keyLoop();
  void terminalSetting(struct termios &oldt);
  void getCommand(char &c);
private:
  void updateArm(char &c);
};
