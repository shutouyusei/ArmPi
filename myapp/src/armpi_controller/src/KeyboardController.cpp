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
        case 'w': case 'W': cmd_.base_velocity.linear.x = MAX_SPEED; break; // 前進
        case 's': case 'S': cmd_.base_velocity.linear.x = -MAX_SPEED; break; // 後退
        case 'a': case 'A': cmd_.base_velocity.angular.z = MAX_TURN; break; // 左旋回
        case 'd': case 'D': cmd_.base_velocity.angular.z = -MAX_TURN; break; // 右旋回
        
        // --- アームのXYZ座標制御 (IK) ---
        // 'o'/'u' で Z軸移動 (上下)
        case 'o': case 'O': cmd_.arm_z += IK_STEP; break; 
        case 'u': case 'U': cmd_.arm_z -= IK_STEP; break;
        // 'i'/'k' で Y軸移動 (前後)
        case 'i': case 'I': cmd_.arm_y += IK_STEP; break;
        case 'k': case 'K': cmd_.arm_y -= IK_STEP; break;
        // 'j'/'l' で X軸移動 (左右)
        case 'l': case 'L': cmd_.arm_x += IK_STEP; break;
        case 'j': case 'J': cmd_.arm_x -= IK_STEP; break;

        // --- 特殊コマンド ---
        case 'r': case 'R': /* グリッパー開閉 (別途フィールドがあれば設定) */ break;
        case 'f': case 'F': /* アームの初期位置に戻るコマンドなど */ break;
        case ' ':           /* 停止 (Twistは既に0なので、必要に応じてアームも停止) */ break;

        case '\x03': // Ctrl+C
            ros::shutdown(); // ROSをシャットダウン
            break;
            
        default:
            break;
    }
}

void KeyboardController::keyLoop() {
  char c = 0;
  bool speed_changed = false;
  struct termios oldt;
  terminalSetting(oldt);

  ROS_INFO("--- Keyboard Teleop Active ---");
  while (running_ && ros::ok()) {
    if (read(STDIN_FILENO, &c, 1) > 0) { 
      getCommand(c);
      c = 0;
    }else{
      cmd_.base_velocity.linear.x = 0.0;
      cmd_.base_velocity.angular.z = 0.0;
    }
    command_publisher_.sendCommand(cmd_);
    ros::spinOnce();
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
