#include "armpi_controller/keyboard/KeyboardController.h"
#include <iostream>
#include <string>
#include <armpi_controller/sdl/SDLHandler.h>
#include <chrono>

KeyboardController::KeyboardController(ros::NodeHandle &nh, const std::string &task_name) : ArmpiController(nh, "KeyboardController"),collect_data_(nh, task_name),task_name_(task_name){
  sdl_handler_ = new SDLHandler();
  ROS_INFO("KeyboardController (SDL) initialized.");
  ROS_INFO_STREAM("ArmpiController for task " << task_name_ << " is started");
}

KeyboardController::~KeyboardController() {
  {
    std::lock_guard<std::mutex> lk(cv_mtx_);
    stop_request_ = true;
  }
  cv_.notify_all();

  if (worker_thread_.joinable()) {
    worker_thread_.join();
  }
  delete sdl_handler_;
}

void KeyboardController::getCommand() {
  sdl_handler_->pollSDLEvents();
  updateChassis();
  updateArm();
  keyControl();
}


void KeyboardController::keyControl() {
  if (is_playing_){
    if (sdl_handler_->is_pressed_1time(SDL_SCANCODE_Z)) {
      if (collect_data_.is_running_ == false) {
        collect_data_.start();
      }
    }
    if (sdl_handler_->is_pressed_1time(SDL_SCANCODE_Y)) {
      if (collect_data_.is_running_ == true) {
        collect_data_.finish(true);
      }
    }
    if (sdl_handler_->is_pressed_1time(SDL_SCANCODE_N)) {
      if (collect_data_.is_running_ == true) {
        collect_data_.finish(false);
      }
    }

    // '1'
    if (sdl_handler_->is_pressed_1time(SDL_SCANCODE_1)) {
      resetServo();
    }

    if (sdl_handler_->is_pressed_1time(SDL_SCANCODE_ESCAPE)){
      {
        std::lock_guard<std::mutex> lk(cv_mtx_);
        stop_request_ = true;
      }
      ros::shutdown();
    }
  }
  if (sdl_handler_->is_pressed_1time(SDL_SCANCODE_X)) {
    if (is_playing_) {
      ROS_INFO("Stopping Collect Record Data Mode...");

      {
        std::lock_guard<std::mutex> lk(cv_mtx_);
        stop_request_ = true;
      }
      cv_.notify_all();

      if (worker_thread_.joinable()) {
        worker_thread_.join();
      }
      ROS_INFO("Thread stopped.");

    } else {
      ROS_INFO("Start Collect Play Data Mode");
      stop_request_ = false;
      worker_thread_ = std::thread(&KeyboardController::periodicallyCollectData, this);
    }
    is_playing_ = !is_playing_;
  }
}

void KeyboardController::updateChassis() {
  if (sdl_handler_->is_pressed(SDL_SCANCODE_W)) {
    cmd_.chassis_move_forward = 1;
  } else if (sdl_handler_->is_pressed(SDL_SCANCODE_S)) {
    cmd_.chassis_move_forward = -1;
  }

  if (sdl_handler_->is_pressed(SDL_SCANCODE_A)) {
    cmd_.angular_right = -1;
  } else if (sdl_handler_->is_pressed(SDL_SCANCODE_D)) {
    cmd_.angular_right = 1;
  }
}
void KeyboardController::periodicallyCollectData(){
  while (ros::ok()) {
    if (stop_request_) break;
    if (collect_data_.is_running_) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        continue;
    }

    collect_data_.start();
    ROS_INFO("Data collection started. Waiting for 1 minute...");
    //wait for minutes
    std::unique_lock<std::mutex> lk(cv_mtx_);
    bool interrupted = cv_.wait_for(lk, std::chrono::minutes(1), [this]{
      return stop_request_.load() || !ros::ok();
    });

    if (interrupted) {
      ROS_INFO("Collection interrupted.");
      collect_data_.finish(false);
      break;
    } else {
      ROS_INFO("1 minute passed. Saving data.");
      collect_data_.finish(true);
    }
  }
}

void KeyboardController::updateArm() {
  if (sdl_handler_->is_pressed(SDL_SCANCODE_O)) cmd_.arm_z = 1;
  else if (sdl_handler_->is_pressed(SDL_SCANCODE_U)) cmd_.arm_z = -1;

  if (sdl_handler_->is_pressed(SDL_SCANCODE_I)) cmd_.arm_y = 1;
  else if (sdl_handler_->is_pressed(SDL_SCANCODE_K)) cmd_.arm_y = -1;

  if (sdl_handler_->is_pressed(SDL_SCANCODE_L)) cmd_.arm_x = 1;
  else if (sdl_handler_->is_pressed(SDL_SCANCODE_J)) cmd_.arm_x = -1;

  if (sdl_handler_->is_pressed(SDL_SCANCODE_R)) cmd_.gripper_close = 1;
  else if (sdl_handler_->is_pressed(SDL_SCANCODE_F)) cmd_.gripper_close = -1;

  if (sdl_handler_->is_pressed(SDL_SCANCODE_UP)) cmd_.arm_alpha = 1;
  else if (sdl_handler_->is_pressed(SDL_SCANCODE_DOWN)) cmd_.arm_alpha = -1;

  if (sdl_handler_->is_pressed(SDL_SCANCODE_RIGHT)) cmd_.rotation = 1;
  else if (sdl_handler_->is_pressed(SDL_SCANCODE_LEFT)) cmd_.rotation = -1;
}
