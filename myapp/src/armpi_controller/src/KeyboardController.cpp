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
void KeyboardController::keyProceccing(char& c,bool& speed_changed) {
  //TODO:ほかの操作も記述する
  if (read(STDIN_FILENO, &c, 1) > 0) {
    linear_x_.store(0.0);
    angular_z_.store(0.0);

    switch (c) {
      case 'w': case 'W': linear_x_.store(MAX_SPEED); break;
      case 's': case 'S': linear_x_.store(-MAX_SPEED); break;
      case 'a': case 'A': angular_z_.store(MAX_TURN); break;
      case 'd': case 'D': angular_z_.store(-MAX_TURN); break;
      case ' ':
        break;
      case '\x03':
        running_ = false;
        ros::shutdown();
        break;
      default:
        speed_changed = false;
        break;
    }
    if (c != '\x03') {
      speed_changed = true;
    }
  }
}

void KeyboardController::keyLoop() {
  char c;
  struct termios oldt, newt;
  bool speed_changed = false;
  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  newt.c_cc[VMIN] = 0;
  newt.c_cc[VTIME] = 0;
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);

  ROS_INFO("--- Keyboard Teleop Active ---");

  while (running_) {
    keyProceccing(c,speed_changed);
    if (speed_changed) {
      this->publishCommand();
      ROS_INFO("Current: Linear=%.2f, Angular=%.2f", linear_x_.load(), angular_z_.load());
      speed_changed = false;
    }

    ros::Duration(0.001).sleep(); 
  }
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}

void KeyboardController::publishCommand() {
  armpi_operation_msgs::RobotCommand cmd;

  cmd.base_velocity.linear.x = linear_x_.load();
  cmd.base_velocity.angular.z = angular_z_.load();

  // アームとグリッパーは0に設定
  // TODO:今後ここも登録する
  cmd.arm_joint_velocities.resize(4);
  std::fill(cmd.arm_joint_velocities.begin(), cmd.arm_joint_velocities.end(), 0.0);
  cmd.gripper_position = 0.0; 

  command_publisher_.sendCommand(cmd); 
}

void KeyboardController::start() {
  running_ = true;
  input_thread_ = std::thread(&KeyboardController::keyLoop, this);
}
