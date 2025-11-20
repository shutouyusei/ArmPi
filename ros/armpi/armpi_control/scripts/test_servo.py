#!/usr/bin/env python3
# coding=utf8
import rospy
import time
import math
from armpi_operation_msgs.msg import RobotCommand
from geometry_msgs.msg import Twist

def test_subscriber_trigger():
    # 1. ノードの初期化
    # (anonymous=Trueで、同じノード名を複数実行可能にする)
    rospy.init_node('command_trigger_tester', anonymous=True)
    
    # 2. Publisherの作成
    # C++ノードが購読しているトピック '/armpi_command' に Publish
    pub = rospy.Publisher('armpi_command', RobotCommand, queue_size=10)
    
    rospy.loginfo("Tester node started. Waiting for subscriber to connect...")
    
    # C++ノードが起動していることを確認する (PublisherがSubscriberを検出するのを待つ)
    # これにより、メッセージが失われることを防ぐ

    rospy.loginfo("Subscriber connected. Sending test commands...")

    rate = rospy.Rate(1) # 1Hz でコマンドを送信
    test_duration = 5   # 5秒間テスト

    for i in range(test_duration):
        # 3. RobotCommand メッセージの作成
        cmd = RobotCommand()

        # ベース速度 (Twist) の設定
        # iの値に基づいて前進と旋回を交互に設定
        #
        if i % 2 == 0:
            # 前進 (Vx=0.3)
            cmd.arm_x = 0.00
            cmd.arm_y = 0.12
            cmd.arm_z = 0.15
            cmd.arm_alpha = 0.0
            rospy.loginfo(f"[{i+1}/5] Publishing FORWARD (Vx=0.3)")
        else:
            cmd.arm_x = 0.15
            cmd.arm_y = 0.12
            cmd.arm_z = 0.15
            rospy.loginfo(f"[{i+1}/5] Publishing TURN (Wz=0.5)")
        
        cmd.arm_alpha = -90.0   
        cmd.rotation = 500;
        # アーム/グリッパーのダミーデータ（Python側ではこの設定も送信が必要）
        cmd.gripper_position = 0.5
        
        # 4. メッセージのPublish
        pub.publish(cmd)
        rate.sleep()

    # 5. 停止コマンドの送信
    stop_cmd = RobotCommand()
    rospy.loginfo("Sending final STOP command.")
    pub.publish(stop_cmd)
    rospy.sleep(0.5)


if __name__ == '__main__':
    try:
        test_subscriber_trigger()
    except rospy.ROSInterruptException:
        ROS_INFO("ERROR")
        pass
