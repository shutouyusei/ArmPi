cmake_minimum_required(VERSION 3.0.2)
project(ai_model_service)

find_package(catkin REQUIRED COMPONENTS
  rospy
  cv_bridge
  sensor_msgs
  armpi_operation_msgs
  message_generation
)

add_service_files(
  FILES
  PredictAction.srv
)

catkin_python_setup()

generate_messages(
  DEPENDENCIES
  sensor_msgs
  armpi_operation_msgs
)


catkin_package(
  CATKIN_DEPENDS rospy cv_bridge sensor_msgs armpi_operation_msgs
  message_runtime
)

catkin_install_python(
  PROGRAMS
  scripts/inference_server.py
  test/test_inference_server.py
  DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)
