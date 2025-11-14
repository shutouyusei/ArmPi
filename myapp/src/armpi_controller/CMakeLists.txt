cmake_minimum_required(VERSION 3.0.2)
project(armpi_controller)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  armpi_operation_msgs
  armpi_command_publisher
  collect_data
  ai_model_service
)

catkin_package(
  CATKIN_DEPENDS roscpp armpi_operation_msgs armpi_command_publisher collect_data ai_model_service
)

include_directories(
  include
  ${armpi_command_publisher_INCLUDE_DIRS}
  ${catkin_INCLUDE_DIRS}
  ${collect_data_INCLUDE_DIRS}
)

add_executable(armpi_controller
  src/ArmpiControllerMain.cpp
  src/ArmpiController.cpp
  src/KeyboardController.cpp
  src/AIController.cpp
)

target_link_libraries(armpi_controller
  ${armpi_command_publisher_LIBRARIES}
  ${collect_data_LIBRARIES}
  ${catkin_LIBRARIES}
  ${CMAKE_THREAD_LIBS_INIT}
  rt
)

