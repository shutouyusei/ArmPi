#!/usr/bin/env python3
# coding=utf8
import rospy
import time
from armpi_operation_msgs.msg import RobotCommand
from geometry_msgs.msg import Twist
import math

def test_chassis_command():
    """
    RobotCommandメッセージを作成し、/robot_command トピックに送信して
    ArmpiDriver::commandCallback をトリガーするテスト関数。
    """
    
    # 1. ノードの初期化
    rospy.init_node('chassis_command_tester', anonymous=True)
    
    # 2. Publisherの作成
    # C++ノードが購読しているトピックにPublishする
    pub = rospy.Publisher('/robot_command', RobotCommand, queue_size=10)
    
    # ノードが起動し、Publisherがトピックに接続するまで少し待つ
    rospy.loginfo("Waiting for C++ ArmpiDriver node to subscribe to /robot_command...")
    time.sleep(1) # C++ノードが起動していることを前提

    rate = rospy.Rate(1) # 1Hz でコマンドを送信
    rospy.loginfo("Starting test commands (1 Hz)...")

    i = 0
    while not rospy.is_shutdown() and i < 5: # 5回送信して終了
        # 3. RobotCommand メッセージの作成
        cmd = RobotCommand()

        # ベース速度 (Twist) の設定: 前進と旋回を交互にテスト
        if i % 2 == 0:
            # 前進 (線形速度 0.3)
            cmd.base_velocity.linear.x = 0.3
            cmd.base_velocity.angular.z = 0.0
            rospy.loginfo("Sending: FORWARD (Vx=0.3)")
        else:
            # 旋回 (角速度 0.5)
            cmd.base_velocity.linear.x = 0.0
            cmd.base_velocity.angular.z = 0.5
            rospy.loginfo("Sending: TURN (Wz=0.5)")
        
        # アームとグリッパーのダミーデータ（C++ノードがエラーにならないよう設定）
        cmd.arm_joint_velocities = [0.1, 0.0, 0.0, 0.0] 
        cmd.gripper_position = 0.5
        
        # 4. メッセージのPublish
        pub.publish(cmd)
        
        i += 1
        rate.sleep()

    # 停止コマンドの送信
    stop_cmd = RobotCommand()
    rospy.loginfo("Sending STOP command.")
    pub.publish(stop_cmd)


if __name__ == '__main__':
    try:
        test_chassis_command()
    except rospy.ROSInterruptException:
        pass
    except Exception as e:
        rospy.logerr("An error occurred: %s", str(e))
