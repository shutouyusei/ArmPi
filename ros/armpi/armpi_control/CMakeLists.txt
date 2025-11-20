cmake_minimum_required(VERSION 3.0.2)
project(armpi_control)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  armpi_operation_msgs
  armpi_chassis
  armpi_servo
)
catkin_package(
  CATKIN_DEPENDS roscpp armpi_operation_msgs
)

include_directories(
  include
  ${catkin_INCLUDE_DIRS}
  ${armpi_chassis_INCLUDE_DIRS}
  ${armpi_servo_INCLUDE_DIRS}
)


catkin_install_python(PROGRAMS
  scripts/test_driver.py
  scripts/test_servo.py
  DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)

add_executable(armpi_control
  src/ArmpiControl.cpp
)

target_link_libraries(armpi_control
  ${catkin_LIBRARIES}
)
