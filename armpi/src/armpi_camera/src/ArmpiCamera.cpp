#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <opencv2/highgui/highgui.hpp>
#include <cv_bridge/cv_bridge.h>

// カメラ画像を受け取るコールバック関数
void imageCallback(const sensor_msgs::ImageConstPtr& msg) {
  try {
    // 1. ROS ImageメッセージをOpenCVのcv::Matに変換
    // cv_bridge::toCvShare() を使用して、メッセージデータを共有し、変換オーバーヘッドを削減
    cv_bridge::CvImageConstPtr cv_ptr = cv_bridge::toCvShare(msg, sensor_msgs::image_encodings::BGR8);

    // 2. OpenCVの画像データ (cv::Mat) を取得
    cv::Mat image_bgr = cv_ptr->image;

    // 3. 画像処理ロジックの開始
    if (image_bgr.empty()) {
      ROS_ERROR("Received empty image!");
      return;
    }

    //TODO:カメラの映像を保存できるか
    // ここに画像処理ロジック（例: cv2.cvtColor, cv2.inRange, 顔認識ネット処理など）を記述します
    // 4. (デバッグ用) 画像の表示
    // cv::imshow("Camera Feed", image_bgr);
    // cv::waitKey(1); // 描画処理のために必要

    ROS_INFO("Image received: %d x %d (Type: %s)", 
             image_bgr.cols, image_bgr.rows, 
             cv_ptr->encoding.c_str());

  } catch (cv_bridge::Exception& e) {
    ROS_ERROR("cv_bridge exception: %s", e.what());
    return;
  }
}

int main(int argc, char** argv) {
  // 1. ROSノードの初期化
  ros::init(argc, argv, "cpp_image_subscriber");
  ros::NodeHandle nh;
  cv::startWindowThread();
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
