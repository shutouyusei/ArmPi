#!/usr/bin/env python3
# coding=utf8
import rospy
import time
from geometry_msgs.msg import Point # 目標XYZ座標を送るためのROS標準メッセージ

# トピック名: C++の制御ノードが購読しているトピック名に合わせる
# (例: /armpi_command のように、トピック名を /armpi_arm_command と仮定します)
ARM_COMMAND_TOPIC = '/armpi_arm_command'

def test_arm_movement_trigger():
    """
    C++の ArmControlSubscriber ノードに対して目標XYZ座標を送信し、
    アーム制御（IKサービス呼び出し）がトリガーされるかテストする。
    """
    
    # 1. ノードの初期化
    rospy.init_node('arm_command_tester', anonymous=True)
    
    # 2. Publisherの作成: C++ノードが購読するトピックに Publish
    # geometry_msgs/Point を使用して目標XYZ座標を送信
    pub = rospy.Publisher(ARM_COMMAND_TOPIC, Point, queue_size=10)
    
    rospy.loginfo("Arm Tester node started. Waiting for C++ subscriber...")
    
    # Subscriber（C++ノード）が接続するのを待つ (推奨: 1秒)
    rospy.sleep(1)

    rospy.loginfo("Subscriber connected. Sending XYZ test coordinates...")

    rate = rospy.Rate(0.5) # 2秒に1回コマンドを送信 (アームの動作時間考慮)
    
    # 3. テスト用のXYZ座標シーケンス (メートル単位)
    # Pythonデモコードと同じ動作を再現: 初期位置 -> X移動 -> 初期位置 -> Y移動 ...
    
    test_coordinates = [
        # (X, Y, Z, 名前)
        (0.00, 0.12, 0.15, "INITIAL"),   # 初期位置
        (0.15, 0.12, 0.15, "X_MOVE"),    # X軸移動
        (0.00, 0.12, 0.15, "INITIAL"),   # 初期位置に戻る
        (0.00, 0.20, 0.15, "Y_MOVE"),    # Y軸移動
        (0.00, 0.12, 0.15, "INITIAL"),   # 初期位置に戻る
        (0.00, 0.12, 0.24, "Z_MOVE"),    # Z軸移動
        (0.00, 0.12, 0.15, "INITIAL"),   # 最終位置
    ]

    for i, (x, y, z, name) in enumerate(test_coordinates):
        if not rospy.is_shutdown():
            # 4. Point メッセージの作成
            target_point = Point()
            target_point.x = x
            target_point.y = y
            target_point.z = z
            
            rospy.loginfo(f"[{i+1}/{len(test_coordinates)}] Publishing {name}: X={x:.2f}, Y={y:.2f}, Z={z:.2f}")
            
            # 5. メッセージのPublish
            pub.publish(target_point)
            
            rate.sleep()

    rospy.loginfo("Test sequence finished. Check arm movement.")


if __name__ == '__main__':
    try:
        # このスクリプトを実行する前に、C++ノードとPython IKサービスが起動している必要があります！
        test_arm_movement_trigger()
    except rospy.ROSInterruptException:
        rospy.loginfo("Test interrupted.")
    except Exception as e:
        rospy.logerr(f"An unexpected error occurred: {e}")
