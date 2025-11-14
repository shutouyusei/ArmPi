#pragma once

#include "ArmpiController.h"

class AIController : public ArmpiController {
public:
  AIController(ros::NodeHandle& nh,const std::string& model_name);
  ~AIController() override;

protected:
  void getCommand() override;
};
