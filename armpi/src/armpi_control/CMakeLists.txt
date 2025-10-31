cmake_minimum_required(VERSION 3.0.2)
project(armpi_control)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  armpi_operation_msgs
  armpi_chassis
)
catkin_package(
  CATKIN_DEPENDS roscpp armpi_operation_msgs
)

include_directories(
  include
  ${catkin_INCLUDE_DIRS}
  ${armpi_driver_INCLUDE_DIRS}
)


catkin_install_python(PROGRAMS
  scripts/test_driver.py
  DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)

add_executable(armpi_control
  src/ArmpiControl.cpp
  src/ArmpiCommandSubscriber.cpp
)

target_link_libraries(armpi_control
  ${catkin_LIBRARIES}
)
