#pragma once

#include "ArmpiController.h"
#include <termios.h>
#include <unistd.h>
#include <thread>
#include <atomic> 
#include <armpi_operation_msgs/RobotCommand.h>

class KeyboardController : public ArmpiController {
public:
  KeyboardController(ros::NodeHandle& nh);
  ~KeyboardController() override;

  void start() override;

private:
  std::thread input_thread_;

  std::atomic<float> linear_x_{0.0};
  std::atomic<float> angular_z_{0.0};
  const float MAX_SPEED = 50.0; 
  const float MAX_TURN = 0.5; 

  void keyLoop();
  void publishCommand();
  void keyProceccing(char& c,bool& speed_changed);
};
