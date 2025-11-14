#include <algorithm>
#include <armpi_controller/KeyboardController.h>
#include <iostream>

KeyboardController::KeyboardController(ros::NodeHandle &nh,
                                       const std::string &task_name)
    : ArmpiController(nh, "KeyboardController", task_name) {
  ROS_INFO("KeyboardController initialized.");
  ROS_INFO("Use 'w/s' for Linear X, 'a/d' for Angular Z, 'Space' for Stop, "
           "'Ctrl+C' to exit.");
  terminalSetting(oldt);
}

KeyboardController::~KeyboardController() {
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}

void KeyboardController::getCommand() {
  size_t bytes_read = read(STDIN_FILENO, key_buffer, KEY_BUFFER_SIZE);
  if (bytes_read > 0) {
    for (size_t i = 0; i < bytes_read; ++i) {
      char c = key_buffer[i];
      keyControl(c);
      updateChassis(c);
      updateArm(c);
    }
  }
}
void KeyboardController::keyControl(char &c) {
  switch (c) {
  case 'z':
  case 'Z':
    if (collect_data_.is_running_ == false)
      collect_data_.start();
    else
      collect_data_.finish(is_successed());
    break;
  case '1':
    reset();
    break;
  case '\x03':       // Ctrl+C
    ros::shutdown(); // ROSをシャットダウン
    break;
  default:
    break;
  }
}

void KeyboardController::updateChassis(char &c) {
  switch (c) {
  case 'w':
  case 'W': {
    if (cmd_.chassis_move_forward == 0) {
      cmd_.chassis_move_forward = 1;
    }
    break;
  }
  case 's':
  case 'S': {
    if (cmd_.chassis_move_forward == 0) {
      cmd_.chassis_move_forward = -1;
    }
    break;
  }
  case 'a':
  case 'A': {
    if (cmd_.angular_right == 0.0) {
      cmd_.angular_right = -1;
      break;
    }
    break;
  }
  case 'd':
  case 'D': {
    if (cmd_.angular_right == 0.0) {
      cmd_.angular_right = 1;
      break;
    }
    break;
  }
  default:
    break;
  }
}

void KeyboardController::updateArm(char &c) {
  switch (c) {
  // 'o'/'u' で Z軸移動 (上下)
  case 'o':
  case 'O': {
    cmd_.arm_z = 1;
    break;
  }
  case 'u':
  case 'U': {
    cmd_.arm_z = -1;
    break;
  }
  // 'i'/'k' で Y軸移動 (前後)
  case 'i':
  case 'I': {
    cmd_.arm_y = 1;
    break;
  }
  case 'k':
  case 'K': {
    cmd_.arm_y = -1;
    break;
  }
  // 'j'/'l' で X軸移動 (左右)
  case 'l':
  case 'L': {
    cmd_.arm_x = 1;
    break;
  }
  case 'j':
  case 'J': {
    cmd_.arm_x = -1;
    break;
  }
  // --- 特殊コマンド ---
  case 'r':
  case 'R': {
    cmd_.gripper_close = 1;
    break;
  }
  case 'f':
  case 'F': {
    cmd_.gripper_close = -1;
    break;
  }
  }
}

bool KeyboardController::is_successed() {
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  std::cout << "Success? (y/n)" << std::endl;
  std::string input;
  input.clear();
  std::getline(std::cin, input);
  terminalSetting(oldt);
  if (input == "y" || input == "Y")
    return true;
  else
    return false;
}

void KeyboardController::terminalSetting(struct termios &oldt) {
  struct termios newt;
  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  newt.c_cc[VMIN] = 0;
  newt.c_cc[VTIME] = 0;
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
}
