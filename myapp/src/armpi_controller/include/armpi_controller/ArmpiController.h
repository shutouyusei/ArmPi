#pragma once
#include <ros/ros.h>
#include <armpi_command_publisher/ArmpiCommandPublisher.h>

class ArmpiController{
public:
  ArmpiCommandPublisher armpi_command_publisher_;
};
