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

  const float MAX_SPEED = 50.0; 
  const float MAX_TURN = 0.5; 
  const float IK_STEP = 0.01;

  void keyLoop();
  void terminalSetting(struct termios &oldt);
  armpi_operation_msgs::RobotCommand getCommand(char &c);
};
