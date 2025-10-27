cmake_minimum_required(VERSION 3.0.2)
project(armpi)

option(BUILD_FOR_PC "Build for PC environment (using stub)" OFF)

if(BUILD_FOR_PC)
  message(STATUS "Build for PC environment (using stub)")
else()
  message(STATUS "Build for robot environment")
endif()

find_package(catkin REQUIRED COMPONENTS
  roscpp
  rospy
  std_msgs
)
# find required components for catkin
if(BUILD_FOR_PC)
  catkin_package()
else()
  find_package(catkin REQUIRED COMPONENTS
    chassis_control
  )
  catkin_package(
    CATKIN_DPENDS chassis_control
  )
endif()


# config for using python
# write python script file name
catkin_install_python(PROGRAMS
  scripts/keyboard_control.py
  DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)
