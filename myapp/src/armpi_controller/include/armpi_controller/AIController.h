#pragma once
#include <armpi_controller/ArmpiController.h>
#include <ros/ros.h>
#include <sensor_msgs/Image.h>
#include <sensor_msgs/JointState.h>
#include <image_transport/image_transport.h>
#include "ai_model_service/PredictAction.h"
#include <mutex>

class AIController : public ArmpiController {
public:
    AIController(ros::NodeHandle& nh, const std::string& model_name);
    ~AIController();

protected:
    void getCommand() override;

private:
    void imageCallback(const sensor_msgs::ImageConstPtr& msg);

    void jointStateCallback(const sensor_msgs::JointStateConstPtr& msg);

private:
    image_transport::ImageTransport it_;
    image_transport::Subscriber sub_image_;
    ros::Subscriber sub_joint_state_;
    ros::ServiceClient client_predict_;

    sensor_msgs::ImageConstPtr latest_image_msg_;
    sensor_msgs::JointStateConstPtr latest_joint_state_msg_;

    std::mutex image_mutex_;
    std::mutex joint_mutex_;
    
    bool received_image_ = false;
    bool received_joint_state_ = false;
};
