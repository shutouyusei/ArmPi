#include <ros/ros.h>
#include <std_msgs/String.h>
#include <sstream>

int main(int argc, char **argv)
{
  // init node
  ros::init(argc, argv, "cpp_talker");

  // create node handle
  ros::NodeHandle nh;

  // create publisher
  ros::Publisher pub = nh.advertise<std_msgs::String>("chatter", 1000);

  // message rate (1Hz)
  ros::Rate loop_rate(1); 
  ROS_INFO("Waiting for subscriber connection...");
  ros::Rate connect_rate(1);
  while (pub.getNumSubscribers() < 1) {
      ros::spinOnce(); // ROSイベントを処理 (接続要求を含む)
      connect_rate.sleep();
  }
  ROS_INFO("Hello ROS");
  int count = 0;
  while (ros::ok())
  {
    // test message
    std_msgs::String msg;
    std::stringstream ss;
    ss << "hello world " << count;
    msg.data = ss.str();

    ROS_INFO("%s", msg.data.c_str());

    pub.publish(msg);

    ros::spinOnce();

    loop_rate.sleep();
    count++;
  }

  return 0;
}
