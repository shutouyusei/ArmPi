#include <armpi_controller/KeyboardController.h>
#include <algorithm>
#include <iostream>

KeyboardController::KeyboardController(ros::NodeHandle& nh):ArmpiController(nh, "KeyboardController"){
  ROS_INFO("KeyboardController initialized.");
  ROS_INFO("Use 'w/s' for Linear X, 'a/d' for Angular Z, 'Space' for Stop, 'Ctrl+C' to exit.");
  cmd_.arm_x = 0.0;
  cmd_.arm_y = 0.12;
  cmd_.arm_z = 0.15;
  cmd_.arm_alpha = -90.0;
  cmd_.arm_alpha1 = -180.0;
  cmd_.arm_alpha2 = 0.0;
  cmd_.gripper = 200;
}

KeyboardController::~KeyboardController() {
  running_ = false;
  if (input_thread_.joinable()) {
    input_thread_.join();
  }
}
void KeyboardController::getCommand(char &c) {
  switch (c) {
    // --- 車体制御 (Twist) ---
    case 'w': case 'W':{
      if(cmd_.base_velocity.linear.x == 0.0){
        cmd_.base_velocity.linear.x = MAX_SPEED;  // 前進
      }
      break;
    }
    case 's': case 'S':{
      if(cmd_.base_velocity.linear.x == 0.0){
        cmd_.base_velocity.linear.x = -MAX_SPEED; break; // 後退
      }
      break;
    }
    case 'a': case 'A': {
      if(cmd_.base_velocity.angular.z == 0.0){
        cmd_.base_velocity.angular.z = MAX_TURN; break; // 左旋回
      }
      break;
    }
    case 'd': case 'D':{
      if(cmd_.base_velocity.angular.z == 0.0){
        cmd_.base_velocity.angular.z = -MAX_TURN; break; // 右旋回
      }
      break;
    }
    case '\x03': // Ctrl+C
      ros::shutdown(); // ROSをシャットダウン
      break;

    default:
      updateArm(c);
      break;
  }
}

void KeyboardController::updateArm(char &c) {
  switch (c){
    // 'o'/'u' で Z軸移動 (上下)
    case 'o': case 'O':{
      if(cmd_.arm_z + IK_STEP <= 0.3){
        cmd_.arm_z += IK_STEP;
      }
      break;
    }
    case 'u': case 'U': {
      if(cmd_.arm_z - IK_STEP >= -0.1){
        cmd_.arm_z -= IK_STEP;
      }
      break;
    }
    // 'i'/'k' で Y軸移動 (前後)
    case 'i': case 'I': {
      if (cmd_.arm_y + IK_STEP <= 0.3){
        cmd_.arm_y += IK_STEP;
      }
      break;
    }
    case 'k': case 'K': {
      if(cmd_.arm_y - IK_STEP >= -0.1){
        cmd_.arm_y -= IK_STEP;
      }
      break;
    }
    // 'j'/'l' で X軸移動 (左右)
    case 'l': case 'L':{
      if(cmd_.arm_x + IK_STEP <= 0.3){
        cmd_.arm_x += IK_STEP;
      }
      break;
    } 
    case 'j': case 'J': {
      if (cmd_.arm_x - IK_STEP >= -0.3){
        cmd_.arm_x -= IK_STEP;
      }
      break;
    }
    // --- 特殊コマンド ---
    case 'r': case 'R': {
      if(cmd_.gripper + GRIPPER_STEP <= 600){
        cmd_.gripper += GRIPPER_STEP; /* グリッパー開閉 (別途フィールドがあれば設定) */
      }
      break;
    }
    case 'f': case 'F':{
      if (cmd_.gripper - GRIPPER_STEP >= 0){
        cmd_.gripper -= GRIPPER_STEP; /* アームの初期位置に戻るコマンドなど */
      }
      break;
    }
  }
}

void KeyboardController::keyLoop() {
  char key_buffer[KEY_BUFFER_SIZE];
  bool speed_changed = false;
  struct termios oldt;
  terminalSetting(oldt);
  ROS_INFO("--- Keyboard Teleop Active ---");
  while (running_ && ros::ok()) {
    cmd_.base_velocity.linear.x = 0.0;
    cmd_.base_velocity.angular.z = 0.0;
    size_t bytes_read = read(STDIN_FILENO, key_buffer, KEY_BUFFER_SIZE);
    if (bytes_read > 0){
      for (size_t i = 0; i < bytes_read; ++i) {
        char c = key_buffer[i];
        getCommand(c);
      }
    }
    command_publisher_.sendCommand(cmd_);
    ros::Duration(0.05).sleep(); 
  }
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}

void KeyboardController::terminalSetting(struct termios &oldt) {
  struct termios  newt;
  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  newt.c_cc[VMIN] = 0;
  newt.c_cc[VTIME] = 0;
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
}

void KeyboardController::start() {
  running_ = true;
  input_thread_ = std::thread(&KeyboardController::keyLoop, this);
}
