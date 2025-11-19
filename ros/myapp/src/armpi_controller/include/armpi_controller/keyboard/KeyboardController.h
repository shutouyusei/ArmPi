#pragma once

#include "armpi_controller/ArmpiController.h"
#include <ros/ros.h>
#include <termios.h>

class SDLHandler;

class KeyboardController : public ArmpiController {
public:
  KeyboardController(ros::NodeHandle& nh, const std::string& task_name);
  ~KeyboardController();

protected:
  void getCommand() override;

private:
  void shutdown();
  void keyControl();
  void updateChassis();
  void updateArm();
private:
  SDLHandler* sdl_handler_;
};
