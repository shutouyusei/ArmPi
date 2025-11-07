cmake_minimum_required(VERSION 3.0.2)
project(collect_data)

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



add_library(collect_data_lib
  src/CollectData.cpp
)

target_link_libraries(collect_data_lib
  ${catkin_LIBRARIES}
)
