#pragma once

#include "ArmpiController.h"
#include <termios.h>
#include <unistd.h>
#include <armpi_operation_msgs/RobotCommand.h>

class KeyboardController : public ArmpiController {
public:
  KeyboardController(ros::NodeHandle& nh);
  ~KeyboardController() override;

protected:
  void getCommand() override;
private:
  void terminalSetting(struct termios &oldt);
  void keyControl(char &c);
  void updateChassis(char &c);
  void updateArm(char &c);
  static const int KEY_BUFFER_SIZE = 16;
  char key_buffer[KEY_BUFFER_SIZE];
  bool speed_changed = false;
  struct termios oldt;
};
