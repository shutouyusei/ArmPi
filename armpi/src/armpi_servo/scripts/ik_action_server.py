#!/usr/bin/env python3
# coding=utf8
import rospy
import time
from armpi_pro_kinematics.srv import ComputeArmIK, ComputeArmIKResponse
from kinematics import ik_transform        # 既存の IK 計算モジュール
from hiwonder_servo_msgs.msg import MultiRawIdPosDur # サーボ制御メッセージ
from armpi_pro import bus_servo_control    # 既存のサーボ制御関数 (set_servos)

# --- 定数 ---
# サーボ制御トピック名 (以前のログで確認済み)
SERVO_PUB_TOPIC = '/servo_controllers/port_id_1/multi_id_pos_dur'

# --- グローバル初期化 ---
ik_solver = ik_transform.ArmIK()
joints_pub = None # Publisher は main 関数で初期化

# サービスリクエストの処理関数
def handle_compute_ik_and_move(req):
    """
    C++クライアントからのリクエストを受け取り、IK計算後、直接アームを動作させる。
    """
    global joints_pub
    
    # 1. IK計算の実行
    coordinate_data = (req.x, req.y, req.z)
    result = ik_solver.setPitchRanges(
        coordinate_data, 
        req.alpha, 
        req.alpha1, 
        req.alpha2
    )

    response = ComputeArmIKResponse()
    
    if result:
        _, servo_data, _ = result 
        
        duration_sec = 1.5 
        
        bus_servo_control.set_servos(
            joints_pub, 
            duration_sec, 
            (
                (1, 200), 
                (2, 500), 
                (3, servo_data['servo3']),
                (4, servo_data['servo4']),
                (5, servo_data['servo5']),
                (6, servo_data['servo6'])
            )
        )
        
        response.success = True
        response.servo3 = servo_data['servo3']
        response.servo4 = servo_data['servo4']
        response.servo5 = servo_data['servo5']
        response.servo6 = servo_data['servo6']
        
        rospy.loginfo("IK & Move Success: (%.2f, %.2f, %.2f) -> Arm moved.", req.x, req.y, req.z)
        
        time.sleep(duration_sec + 0.1) 
    else:
        response.success = False
        rospy.logwarn("IK & Move Failed: No solution found for (%.2f, %.2f, %.2f).", req.x, req.y, req.z)
        
    return response

if __name__ == "__main__":
    rospy.init_node('ik_action_server', log_level=rospy.INFO)
    
    joints_pub = rospy.Publisher(SERVO_PUB_TOPIC, MultiRawIdPosDur, queue_size=1)
    rospy.sleep(0.2) # Publisher が確立されるのを待機
    
    # サービスを公開
    rospy.Service('compute_arm_ik_and_move', ComputeArmIK, handle_compute_ik_and_move)
    rospy.loginfo("IK Action Service Server Ready (Listening on 'compute_arm_ik_and_move').")
    
    # ノードを継続的に実行し、サービスリクエストを待ちます
    rospy.spin()
