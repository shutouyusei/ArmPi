cmake_minimum_required(VERSION 3.0.2)
project(armpi_driver)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  rospy
  std_msgs
  geometry_msgs
  chassis_control
)

catkin_package(
  INCLUDE_DIRS include
  LIBRARIES armpi_driver_lib
  CATKIN_DEPENDS roscpp chassis_control
)

include_directories(
  include
  ${catkin_INCLUDE_DIRS}
)

catkin_install_python(PROGRAMS
  scripts/test_armpi_driver.py
  DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)

add_library(armpi_driver_lib
src/ArmpiDriver.cpp
)

target_link_libraries(armpi_driver_lib
  ${catkin_LIBRARIES}
  ${chassis_control_LIBRARIES}
)
