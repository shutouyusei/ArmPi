#pragma once

#include "armpi_controller/ArmpiController.h"
#include <ros/ros.h>
#include <termios.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <collect_data/CollectData.h>

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
  void periodicallyCollectData();
private:
  SDLHandler* sdl_handler_;
  CollectData collect_data_;

  bool is_playing_ = false;
  std::thread worker_thread_;
  std::mutex cv_mtx_;
  std::condition_variable cv_;
  std::atomic<bool> stop_request_{false};
  std::string task_name_;
};
