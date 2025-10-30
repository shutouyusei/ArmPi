cmake_minimum_required(VERSION 3.0.2)
project(armpi_command_publisher)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  armpi_operation_msgs
)

catkin_package(
  INCLUDE_DIRS include
  LIBRARIES armpi_command_publisher_lib
  CATKIN_DEPENDS roscpp armpi_operation_msgs
)

include_directories(
  include
  ${catkin_INCLUDE_DIRS}
)

add_library(armpi_command_publisher_lib
  src/ArmpiCommandPublisher.cpp
)



target_link_libraries(armpi_command_publisher_lib
  ${catkin_LIBRARIES}
)
