cmake_minimum_required(VERSION 3.0.2)
project(armpi_camera)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  sensor_msgs
  image_transport
  cv_bridge
)

find_package(OpenCV REQUIRED)

catkin_package(
  INCLUDE_DIRS include
  LIBRARIES armpi_camera_lib
  CATKIN_DEPENDS roscpp sensor_msgs image_transport cv_bridge
)

include_directories(
  include
  ${catkin_INCLUDE_DIRS}
  ${OpenCV_INCLUDE_DIRS}
)



add_library(armpi_camera_lib
  src/ArmpiCamera.cpp
)

target_link_libraries(armpi_camera_lib
  ${catkin_LIBRARIES}
  ${OpenCV_LIBS}
)
