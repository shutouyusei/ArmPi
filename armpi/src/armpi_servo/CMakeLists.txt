cmake_minimum_required(VERSION 3.0.2)
project(armpi_servo)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  rospy
  std_msgs
  message_generation
  hiwonder_servo_msgs
)

add_service_files(
  FILES
  ComputeArmIK.srv
)

generate_messages(
  DEPENDENCIES
  std_msgs
  hiwonder_servo_msgs
)

catkin_install_python(PROGRAMS
  scripts/ik_action_server.py
  DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)

include_directories(
  include
  ${catkin_INCLUDE_DIRS}
)

add_library(arm_move_control_lib src/ArmpiServoControl.cpp)

# ライブラリのリンク
target_link_libraries(arm_move_control_lib
  ${catkin_LIBRARIES}
)
