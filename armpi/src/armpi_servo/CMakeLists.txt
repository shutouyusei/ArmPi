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
  scripts/test_servo.py
  DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)

catkin_package(
  INCLUDE_DIRS include
  LIBRARIES armpi_servo_lib
  CATKIN_DEPENDS roscpp rospy std_msgs hiwonder_servo_msgs
)

include_directories(
  include
  ${catkin_INCLUDE_DIRS}
)

add_library(armpi_servo_lib
  src/ArmpiServo.cpp
)

# ライブラリのリンク
target_link_libraries(armpi_servo_lib
  ${catkin_LIBRARIES}
)
