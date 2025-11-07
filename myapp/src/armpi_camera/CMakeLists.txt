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
  CATKIN_DEPENDS roscpp sensor_msgs image_transport cv_bridge
)

include_directories(
  include
  ${catkin_INCLUDE_DIRS}
  ${OpenCV_INCLUDE_DIRS}
)



add_executable(armpi_camera
  src/ArmpiCamera.cpp
)

target_link_libraries(armpi_camera
  ${catkin_LIBRARIES}
  ${OpenCV_LIBS}
)
