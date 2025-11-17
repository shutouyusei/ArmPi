#include "armpi_controller/keyboard/KeyboardController.h"
#include <iostream>
#include <string>
#include <armpi_controller/keyboard/SDLHandler.h>
#include <std_msgs/Empty.h>

KeyboardController::KeyboardController(ros::NodeHandle &nh, const std::string &task_name) : ArmpiController(nh, "KeyboardController", task_name){
  pub_reset_servo_ = nh_.advertise<std_msgs::Empty>("reset_servo", 1);
  sdl_handler_ = new SDLHandler();
  ROS_INFO("KeyboardController (SDL) initialized.");
}

KeyboardController::~KeyboardController() {
  delete sdl_handler_;
}

void KeyboardController::getCommand() {
  sdl_handler_->pollSDLEvents();
  keyControl();
  updateChassis();
  updateArm();
}


void KeyboardController::keyControl() {
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
    pub_reset_servo_.publish(std_msgs::Empty());
  }

  if (sdl_handler_->is_pressed_1time(SDL_SCANCODE_ESCAPE)){
    ros::shutdown();
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
