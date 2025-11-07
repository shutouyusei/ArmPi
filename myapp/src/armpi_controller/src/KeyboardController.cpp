#include <armpi_controller/KeyboardController.h>
#include <algorithm>
#include <iostream>

KeyboardController::KeyboardController(ros::NodeHandle& nh):ArmpiController(nh, "KeyboardController"){
  ROS_INFO("KeyboardController initialized.");
  ROS_INFO("Use 'w/s' for Linear X, 'a/d' for Angular Z, 'Space' for Stop, 'Ctrl+C' to exit.");
  terminalSetting(oldt);
}

KeyboardController::~KeyboardController() {
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}

void KeyboardController::getCommand() {
  size_t bytes_read = read(STDIN_FILENO, key_buffer, KEY_BUFFER_SIZE);
  if (bytes_read > 0){
    for (size_t i = 0; i < bytes_read; ++i) {
      char c = key_buffer[i];
      keyControl(c);
      updateChassis(c);
      updateArm(c);
    }
  }
}
void KeyboardController::keyControl(char &c) {
  switch(c){
    case 'z': case 'Z':
      if (collect_data_.is_running_ == false) collect_data_.start();
      else collect_data_.finish();
      break;
    case '\x03': // Ctrl+C
      ros::shutdown(); // ROSをシャットダウン
      break;
    default:
      break;
  }
}

void KeyboardController::updateChassis(char &c) {
  switch (c) {
    case 'w': case 'W':{
      if(cmd_.base_velocity.linear.x == 0.0){
        cmd_.base_velocity.linear.x = MAX_SPEED;  // move forward
      }
      break;
    }
    case 's': case 'S':{
      if(cmd_.base_velocity.linear.x == 0.0){
        cmd_.base_velocity.linear.x = -MAX_SPEED; break; // back
      }
      break;
    }
    case 'a': case 'A': {
      if(cmd_.base_velocity.angular.z == 0.0){
        cmd_.base_velocity.angular.z = MAX_TURN; break; // turn left
      }
      break;
    }
    case 'd': case 'D':{
      if(cmd_.base_velocity.angular.z == 0.0){
        cmd_.base_velocity.angular.z = -MAX_TURN; break; // turn right
      }
      break;
    }
    default:
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


void KeyboardController::terminalSetting(struct termios &oldt) {
  struct termios  newt;
  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  newt.c_cc[VMIN] = 0;
  newt.c_cc[VTIME] = 0;
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
}
