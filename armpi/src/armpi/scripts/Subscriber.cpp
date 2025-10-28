#include <ros/ros.h>
#include <std_msgs/String.h>

// 1. コールバック関数の定義
// メッセージが受信されるたびにこの関数が実行される
void chatterCallback(const std_msgs::String::ConstPtr& msg)
{
  // 受信したメッセージをログに出力
  ROS_INFO("I heard: [%s]", msg->data.c_str());
}

int main(int argc, char **argv)
{
  // 2. ノードの初期化
  ros::init(argc, argv, "cpp_listener");

  // 3. ノードハンドルの作成
  ros::NodeHandle nh;

  // 4. Subscriberの作成
  // トピック名 "/chatter"、メッセージ型 std_msgs::String、キューサイズ 1000
  // メッセージを受信したら chatterCallback 関数を実行
  ros::Subscriber sub = nh.subscribe("chatter", 1000, chatterCallback);

  // 5. ループ処理
  // ros::spin() はノードが終了するまで待機し、
  // メッセージの受信イベントが発生するたびにコールバック関数を自動で呼び出す
  ROS_INFO("start");
  ros::spin();

  return 0;
}
