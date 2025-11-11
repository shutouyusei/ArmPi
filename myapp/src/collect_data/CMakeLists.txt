cmake_minimum_required(VERSION 3.0.2)
project(collect_data)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  rosbag
  sensor_msgs
  armpi_camera
  armpi_operation_msgs
)

catkin_package(
  INCLUDE_DIRS include
  LIBRARIES collect_data_lib
  CATKIN_DEPENDS roscpp sensor_msgs armpi_camera armpi_operation_msgs rosbag
)

include_directories(
  include
  ${armpi_camera_INCLUDE_DIRS}
  ${catkin_INCLUDE_DIRS}
)


add_library(collect_data_lib
  src/CollectData.cpp
  src/CollectJoint.cpp
  src/CollectCommand.cpp
)

target_link_libraries(collect_data_lib
  ${catkin_LIBRARIES}
)
