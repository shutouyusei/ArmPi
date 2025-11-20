#!/usr/bin/env python3
import rospy
import numpy as np

# --- 必要なメッセージ型とサービス型 ---
from sensor_msgs.msg import Image, JointState
from std_msgs.msg import Header
from ai_model_service.srv import PredictAction, PredictActionRequest
# (もし `armpi_operation_msgs` が別パッケージなら、C++側と同様に依存関係が必要)
from armpi_operation_msgs.msg import RobotCommand 

# --- テスト用のダミーデータ設定 ---

# 1. 状態 (JointState) の設定
# (サーバー側の STATE_COLUMNS と対応させる)
STATE_COLUMNS = [
    'joint1_pos', 'joint2_pos', 'joint3_pos', 'joint4_pos', 'joint5_pos', 'r_joint_pos'
]
# .srv で送る JointState の 'name' フィールド (サーバー側で '_pos' を除去することを想定)
JOINT_NAMES = [col.replace('_pos', '') for col in STATE_COLUMNS]

# 2. 画像 (Image) の設定
IMG_HEIGHT = 480
IMG_WIDTH = 640
IMG_ENCODING = 'bgr8'
IMG_CHANNELS = 3 # bgr8 は 3 チャンネル

def create_random_request():
    """
    推論サービスに送るためのランダムなリクエスト(PredictActionRequest)を作成
    """
    rospy.loginfo("ランダムなリクエストデータを生成中...")
    req = PredictActionRequest()

    # --- 1. ダミーの JointState を作成 ---
    header = Header(stamp=rospy.Time.now())
    joint_state = JointState()
    joint_state.header = header
    joint_state.name = JOINT_NAMES
    
    # 6個のランダムな関節角度 (0.0 ~ 1.0) を生成
    joint_state.position = np.random.rand(len(JOINT_NAMES)).tolist()
    # (velocity, effort は空でもOK)

    req.current_joint_state = joint_state

    # --- 2. ダミーの Image を作成 ---
    image = Image()
    image.header = header
    image.height = IMG_HEIGHT
    image.width = IMG_WIDTH
    image.encoding = IMG_ENCODING
    image.is_bigendian = 0
    image.step = IMG_WIDTH * IMG_CHANNELS # 1行のバイト数

    # (高さ x 幅 x 3チャンネル) のランダムなバイト配列を生成
    image_data_size = IMG_HEIGHT * IMG_WIDTH * IMG_CHANNELS
    image.data = np.random.bytes(image_data_size)
    
    req.current_image = image
    
    return req

def test_service_client():
    """
    サービスに接続し、ランダムなリクエストを送信して応答を確認する
    """
    service_name = "predict_action"
    
    # 1. サービスが起動するまで待機
    rospy.loginfo(f"サービス '{service_name}' を待機中...")
    try:
        rospy.wait_for_service(service_name, timeout=10.0)
    except rospy.ROSException:
        rospy.logerr(f"エラー: サービス '{service_name}' が見つかりません。サーバーは起動していますか？")
        return

    rospy.loginfo("サービスに接続しました。")

    # 2. サービスクライアントを作成
    try:
        predict_service = rospy.ServiceProxy(service_name, PredictAction)
        
        # 3. ランダムなリクエストを作成
        request = create_random_request()
        
        rospy.loginfo("サービスを呼び出し、推論結果を待機中...")
        
        # 4. サービスを呼び出し (Python側でモデル計算が実行される)
        response = predict_service(request)
        
        # 5. 結果を表示
        if response and response.predicted_command:
            rospy.loginfo("--- サービスから応答を受信 (成功) ---")
            print("受け取った予測コマンド (RobotCommand):")
            
            # レスポンス全体を簡易表示
            print(response.predicted_command)
            
            # 特定の値をピックアップして表示 (例)
            rospy.loginfo(f"  -> 予測された arm_x: {response.predicted_command.arm_x:.4f}")
            rospy.loginfo(f"  -> 予測された gripper: {response.predicted_command.gripper:.4f}")
            
        else:
            rospy.logwarn("サービスは応答しましたが、predicted_command が空です。")

    except rospy.ServiceException as e:
        rospy.logerr(f"サービス呼び出しに失敗しました: {e}")
    except Exception as e:
        rospy.logerr(f"予期せぬエラーが発生しました: {e}")


if __name__ == '__main__':
    try:
        rospy.init_node("test_inference_client")
        test_service_client()
    except rospy.ROSInterruptException:
        pass
