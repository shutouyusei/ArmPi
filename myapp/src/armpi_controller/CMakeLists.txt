cmake_minimum_required(VERSION 3.0.2)
project(armpi_controller)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  armpi_operation_msgs
  armpi_command_publisher
)

catkin_package(
  CATKIN_DEPENDS roscpp armpi_operation_msgs armpi_command_publisher
)

include_directories(
  include
  ${armpi_command_publisher_INCLUDE_DIRS}
  ${catkin_INCLUDE_DIRS}
)

add_executable(armpi_controller src/ArmpiController.cpp)

target_link_libraries(armpi_controller
  ${armpi_command_publisher_LIBRARIES}
  ${catkin_LIBRARIES}
)

