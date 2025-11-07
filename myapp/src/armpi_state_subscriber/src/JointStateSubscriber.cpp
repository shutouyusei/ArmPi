#include <ros/ros.h>
#include <sensor_msgs/JointState.h>

void jointStateCallback(const sensor_msgs::JointState::ConstPtr& msg)
{
    for (size_t i = 0; i < msg->name.size(); ++i)
    {
        if (msg->name[i] == "joint2")
        {
            ROS_INFO("joint2 Position: %.3f rad", msg->position[i]);
        }
        
        if (msg->name[i] == "joint3")
        {
            ROS_INFO("joint3 Position: %.3f rad", msg->position[i]);
        }
    }
}

int main(int argc, char** argv)
{
    ros::init(argc, argv, "motor_state_subscriber");
    ros::NodeHandle nh;

    ros::Subscriber sub = nh.subscribe("/joint_states", 10, jointStateCallback);

    ROS_INFO("Listening for motor states on /joint_states...");

    ros::spin();
    return 0;
}
