#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <opencv2/highgui/highgui.hpp>
#include <cv_bridge/cv_bridge.h>

void imageCallback(const sensor_msgs::ImageConstPtr& msg) {
  try {
    cv_bridge::CvImageConstPtr cv_ptr = cv_bridge::toCvShare(msg, sensor_msgs::image_encodings::BGR8);
    cv::Mat image_bgr = cv_ptr->image;

    if (image_bgr.empty()) {
      ROS_ERROR("Received empty image!");
      return;
    }

    ROS_INFO("Image received: %d x %d (Type: %s)", 
             image_bgr.cols, image_bgr.rows, 
             cv_ptr->encoding.c_str());

  } catch (cv_bridge::Exception& e) {
    ROS_ERROR("cv_bridge exception: %s", e.what());
    return;
  }
}

int main(int argc, char** argv) {
  ROS_INFO("Setup Subscriber for image");
  // 1. ROSノードの初期化
  ros::init(argc, argv, "cpp_image_subscriber");
  ros::NodeHandle nh;
  // 2. image_transport の初期化
  image_transport::ImageTransport it(nh);

  // 3. /usb_cam/image_raw トピックの購読
  // image_transport::Subscriber を使用して、Imageメッセージを購読する
  image_transport::Subscriber sub = it.subscribe("/usb_cam/image_raw", 1, imageCallback);

  ROS_INFO("Listening for images on /usb_cam/image_raw...");

  // 4. メッセージ処理ループ
  ros::spin(); 

  return 0;
}
