#pragma once
#include <ros/ros.h>
#include <vector>

class SDLHandler;
class ArmpiController;

class UserInput{
public:
  UserInput(ArmpiController *armpi_controller);
  ~UserInput();
  void keyInput();
  void saveCSV();
private:
  static std::string generateCSVFileName();
public:
  bool is_running_ = false;
private:
  SDLHandler* sdl_handler_;
  std::vector<bool> result_list_;
  ArmpiController *armpi_controller_;
};
