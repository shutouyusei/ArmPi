cmake_minimum_required(VERSION 3.0.2)
project(armpi)

find_package(catkin REQUIRED COMPONENTS
  std_msgs
  roscpp
)
catkin_package(
  CATKIN_DEPENDS roscpp std_msgs
)

include_directories(
  include
  ${catkin_INCLUDE_DIRS}
)


add_executable(armpi scripts/Subscriber.cpp)

target_link_libraries(armpi
  ${catkin_LIBRARIES}
)
