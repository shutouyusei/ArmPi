cmake_minimum_required(VERSION 3.0.2)
project(armpi_state_subscriber)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  sensor_msgs
)

catkin_package(
  CATKIN_DEPENDS roscpp sensor_msgs
)

include_directories(
  include
  ${catkin_INCLUDE_DIRS}
)



add_executable(joint_state_subscriber
  src/JointStateSubscriber.cpp
)

target_link_libraries(joint_state_subscriber
  ${catkin_LIBRARIES}
)
