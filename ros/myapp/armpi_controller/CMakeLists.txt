cmake_minimum_required(VERSION 3.0.2)
project(armpi_controller)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  armpi_operation_msgs
  collect_data
  ai_model_service
)
find_package(SDL2 REQUIRED)

catkin_package(
  CATKIN_DEPENDS roscpp armpi_operation_msgs collect_data ai_model_service
)

include_directories(
  include
  ${catkin_INCLUDE_DIRS}
  ${collect_data_INCLUDE_DIRS}
)

add_executable(armpi_controller
  src/ArmpiControllerMain.cpp
  src/ArmpiController.cpp
  src/keyboard/KeyboardController.cpp
  src/ai/AIController.cpp
  src/ai/UserInput.cpp
  src/sdl/SDLHandler.cpp
)

target_link_libraries(armpi_controller
  ${collect_data_LIBRARIES}
  ${catkin_LIBRARIES}
  ${CMAKE_THREAD_LIBS_INIT}
  ${SDL2_LIBRARIES}
  rt
  stdc++fs
)

