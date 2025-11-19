#!/usr/bin/env python3
# coding=utf8
import rospy
import time

# 作成したサービスヘッダをインポート
# サービス名: ComputeArmIK (srvファイル名と同じ)
from armpi_servo.srv import ComputeArmIK, ComputeArmIKRequest 

# IKサービスの名前（Pythonサービスサーバーと一致させる）
IK_SERVICE_NAME = 'compute_arm_ik_and_move'

def test_arm_movement_caller():
    """
    ROSサービスを直接呼び出し、アームのIK計算と動作をトリガーする。
    """
    
    # 1. ノードの初期化
    rospy.init_node('arm_service_caller', anonymous=True)
    
    # 2. サービスのプロキシ（クライアント）を作成
    rospy.wait_for_service(IK_SERVICE_NAME)
    try:
        # サービスを呼び出すための関数を作成
        ik_service_proxy = rospy.ServiceProxy(IK_SERVICE_NAME, ComputeArmIK)
    except rospy.ServiceException as e:
        rospy.logerr(f"Service call failed: {e}")
        return

    rospy.loginfo("Service proxy created. Sending IK requests...")

    # 3. テスト用のXYZ座標シーケンス (メートル単位)
    test_coordinates = [
        # (X, Y, Z, 名前)
        (0.00, 0.12, 0.15, "INITIAL"),   # 初期位置
        (0.15, 0.12, 0.15, "X_MOVE"),    # X軸移動
        (0.00, 0.12, 0.15, "INITIAL"),   # 初期位置に戻る
        (0.00, 0.20, 0.15, "Y_MOVE"),    # Y軸移動
        (0.00, 0.12, 0.15, "INITIAL"),   # 初期位置に戻る
        (0.00, 0.12, 0.24, "Z_MOVE"),    # Z軸移動
        (0.00, 0.12, 0.15, "FINAL"),     # 最終位置
    ]
    
    # 動作間の待機時間 (アームの動作時間 1.5秒を考慮し、2秒待機)
    move_delay = rospy.Duration(2.0) 

    for i, (x, y, z, name) in enumerate(test_coordinates):
        if rospy.is_shutdown():
            break
            
        # 4. ComputeArmIKRequest メッセージの作成
        req = ComputeArmIKRequest()
        req.x = x
        req.y = y
        req.z = z
        
        # IK計算に必要な角度制約をサービスに渡す
        req.alpha = -90.0   
        req.alpha1 = -180.0 
        req.alpha2 = 0.0    
        
        rospy.loginfo(f"[{i+1}/{len(test_coordinates)}] REQUEST {name}: X={x:.2f}, Y={y:.2f}, Z={z:.2f}")
        
        # 5. サービス呼び出し (同期処理)
        try:
            resp = ik_service_proxy(req)
            
            if resp.success:
                rospy.loginfo(f"  SUCCESS. Received S3={resp.servo3}, S4={resp.servo4}")
            else:
                rospy.logwarn("  FAILED. IK solution not found for this coordinate.")
        
        except rospy.ServiceException as e:
            rospy.logerr(f"Service call failed: {e}")
            break
            
        rospy.sleep(move_delay)

    rospy.loginfo("Test sequence finished.")


if __name__ == '__main__':
    try:
        # このスクリプトを実行する前に、Pythonサービスサーバー ik_action_server.py が起動している必要があります！
        test_arm_movement_caller()
    except rospy.ROSInterruptException:
        rospy.loginfo("Test interrupted.")
