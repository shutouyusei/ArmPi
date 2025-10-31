#include <armpi_controller/KeyboardController.h>
#include <algorithm>
#include <iostream>

KeyboardController::KeyboardController(ros::NodeHandle& nh):ArmpiController(nh, "KeyboardController"){
  ROS_INFO("KeyboardController initialized.");
  ROS_INFO("Use 'w/s' for Linear X, 'a/d' for Angular Z, 'Space' for Stop, 'Ctrl+C' to exit.");
}

KeyboardController::~KeyboardController() {
  running_ = false;
  if (input_thread_.joinable()) {
    input_thread_.join();
  }
}
armpi_operation_msgs::RobotCommand KeyboardController::getCommand(char &c) {
    armpi_operation_msgs::RobotCommand cmd;
    
    cmd.base_velocity.linear.x = 0.0;
    cmd.base_velocity.angular.z = 0.0;
    
    cmd.arm_x = 0.0;
    cmd.arm_y = 0.0;
    cmd.arm_z = 0.0;

    switch (c) {
        // --- 車体制御 (Twist) ---
        case 'w': case 'W': cmd.base_velocity.linear.x = MAX_SPEED; break; // 前進
        case 's': case 'S': cmd.base_velocity.linear.x = -MAX_SPEED; break; // 後退
        case 'a': case 'A': cmd.base_velocity.angular.z = MAX_TURN; break; // 左旋回
        case 'd': case 'D': cmd.base_velocity.angular.z = -MAX_TURN; break; // 右旋回
        
        // --- アームのXYZ座標制御 (IK) ---
        // 'o'/'u' で Z軸移動 (上下)
        case 'o': case 'O': cmd.arm_z = IK_STEP; break; 
        case 'u': case 'U': cmd.arm_z = -IK_STEP; break;
        // 'i'/'k' で X軸移動 (前後)
        case 'i': case 'I': cmd.arm_x = IK_STEP; break;
        case 'k': case 'K': cmd.arm_x = -IK_STEP; break;
        // 'j'/'l' で Y軸移動 (左右)
        case 'j': case 'J': cmd.arm_y = IK_STEP; break;
        case 'l': case 'L': cmd.arm_y = -IK_STEP; break;

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

    return cmd;
}

void KeyboardController::keyLoop() {
  char c = 0;
  bool speed_changed = false;
  struct termios oldt;
  terminalSetting(oldt);

  ROS_INFO("--- Keyboard Teleop Active ---");
  armpi_operation_msgs::RobotCommand cmd;
  while (running_ && ros::ok()) {
    if (read(STDIN_FILENO, &c, 1) > 0) { 
      cmd = getCommand(c);
    }else{
      cmd = armpi_operationsgs::RobotCommand();
    }
    command_publisher_.sendCommand(cmd);
    ros::Duration(0.001).sleep(); 
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
