cmake_minimum_required(VERSION 3.0.2)
project(armpi_chassis)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  std_msgs
  geometry_msgs
  chassis_control
)

catkin_package(
  INCLUDE_DIRS include
  LIBRARIES armpi_chassis_lib
  CATKIN_DEPENDS roscpp chassis_control
)

include_directories(
  include
  ${catkin_INCLUDE_DIRS}
)

add_library(armpi_chassis_lib
  src/ArmpiChassis.cpp
)

target_link_libraries(armpi_chassis_lib
  ${catkin_LIBRARIES}
  ${chassis_control_LIBRARIES}
)
