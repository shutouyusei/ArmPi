cmake_minimum_required(VERSION 3.0.2)
project(armpi_command_subscriber)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  armpi_operation_msgs
)
catkin_package(
  CATKIN_DEPENDS roscpp armpi_operation_msgs
)

include_directories(
  include
  ${catkin_INCLUDE_DIRS}
)


add_executable(armpi_command_subscriber scripts/ArmpiCommandSubscriber.cpp)

target_link_libraries(armpi_command_subscriber
  ${catkin_LIBRARIES}
)
