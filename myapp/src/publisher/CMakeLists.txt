cmake_minimum_required(VERSION 3.0.2)
project(publisher)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  std_msgs
)

catkin_package(
  CATKIN_DEPENDS roscpp std_msgs
)

include_directories(
  include
  ${catkin_INCLUDE_DIRS}
)

add_executable(publisher scripts/Publisher.cpp)

target_link_libraries(publisher
  ${catkin_LIBRARIES}
)
