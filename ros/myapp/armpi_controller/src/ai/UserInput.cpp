#include <armpi_controller/ai/UserInput.h>
#include <armpi_controller/sdl/SDLHandler.h>
#include <string>
#include <armpi_controller/ArmpiController.h>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <iomanip>
#include <ctime>
UserInput::UserInput(ArmpiController* armpi_controller):armpi_controller_(armpi_controller){
  sdl_handler_ = new SDLHandler();
}

UserInput::~UserInput(){
  delete sdl_handler_;
}

void UserInput::keyInput(){
  sdl_handler_->pollSDLEvents();
  if (sdl_handler_->is_pressed_1time(SDL_SCANCODE_SPACE)) {
    if(!is_running_){
      ROS_INFO("result count:%ld",result_list_.size()+1);
      armpi_controller_->resetServo();
      is_running_ = !is_running_;
      std::string  debug = is_running_ ? "AI Controller is running" : "AI Controller is stopped";
      ROS_INFO_STREAM(debug);
    }
  }
  if (sdl_handler_->is_pressed_1time(SDL_SCANCODE_Y)){
    if(is_running_){
      is_running_ = !is_running_;
      armpi_controller_->resetServo();
      result_list_.push_back(true);
    }
  }
  if (sdl_handler_->is_pressed_1time(SDL_SCANCODE_N)){
    if(is_running_){
      is_running_ = !is_running_;
      armpi_controller_->resetServo();
      result_list_.push_back(false);
    }
  }
  if (sdl_handler_->is_pressed_1time(SDL_SCANCODE_S)){
    if(!is_running_){
      saveCSV();
      result_list_.clear();
    }
  }
}

void UserInput::saveCSV(){
  std::string csv_file_path = generateCSVFileName();
  std::ofstream outputFile(csv_file_path);
  for (size_t i = 0; i < result_list_.size(); ++i) {
    outputFile << result_list_[i];
    if (i < result_list_.size() - 1) {
      outputFile << ",";
    }
  }
  outputFile << "\n";

  outputFile.close();
  std::cout << "finish create csv" << std::endl;
}

std::string UserInput::generateCSVFileName(){
  std::time_t t = std::time(nullptr);
  std::tm tm = *std::localtime(&t);
  std::stringstream ss;

  ss << "/home/rosuser/ros_ws/results/data_"
      << std::put_time(&tm, "%Y%m%d_%H%M%S")
      << ".csv";
  std::string full_path_str = ss.str();
  
  try {
    std::filesystem::path csv_path(full_path_str);
    std::filesystem::path dir_path = csv_path.parent_path();
    if (!std::filesystem::exists(dir_path)) {
        std::filesystem::create_directories(dir_path);
    }
  } catch (const std::filesystem::filesystem_error& e) {
      ROS_ERROR("Failed to create directory: %s", e.what());
  }
  return full_path_str;
}
