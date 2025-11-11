import h5py
import numpy as np
import cv2
from os.path import expanduser # ホームディレクトリ(~)展開用

# ▼▼▼【修正】▼▼▼
from pathlib import Path # Path オブジェクトを使うためにインポート
from rosbags.rosbag1 import Reader # v0.11.0
from rosbags.typesys import Stores, get_typestore
from rosbags.typesys import get_types_from_msg # スニペットから
# ▲▲▲▲▲▲▲▲▲▲▲

def get_ros_time(stamp):
    """ROSのタイムスタンプをfloatの秒に変換"""
    return stamp.sec + stamp.nanosec * 1e-9

def extract_command_data(msg):
    """RobotCommandメッセージをフラットなリストに変換"""
    return [
        msg.base_velocity.linear.x,
        msg.base_velocity.linear.y,
        msg.base_velocity.linear.z,
        msg.base_velocity.angular.x,
        msg.base_velocity.angular.y,
        msg.base_velocity.angular.z,
        msg.arm_x,
        msg.arm_y,
        msg.arm_z,
        msg.gripper,
        msg.arm_alpha,
        msg.arm_alpha1,
        msg.arm_alpha2,
        msg.gripper_position,
    ]

COMMAND_FIELDS = [
    "base_vel_linear_x", "base_vel_linear_y", "base_vel_linear_z",
    "base_vel_angular_x", "base_vel_angular_y", "base_vel_angular_z",
    "arm_x", "arm_y", "arm_z", "gripper", "arm_alpha", "arm_alpha1",
    "arm_alpha2", "gripper_position"
]

def convert_bag_to_h5(bag_path, h5_path, custom_msg_file):
    """
    ROS1の.bagファイルをHDF5ファイルに変換する (v0.11.0 + 手動MSG登録)
    """
    print(f"HDF5ファイルを作成します: {h5_path}")

    try:
        # 1. 標準メッセージストア(NOETIC)をロード
        typestore = get_typestore(Stores.ROS1_NOETIC)

        # ▼▼▼【スニペットを適用】▼▼▼
        # 2. カスタム .msg ファイルのパスを解決
        msg_file_path = Path(expanduser(custom_msg_file))
        print(f"カスタムMSGファイルを読み込みます: {msg_file_path}")

        # 3. .msg ファイルの内容をテキストとして読み込む
        msg_text = msg_file_path.read_text()

        # 4. メッセージ定義を辞書として取得
        #    第2引数はメッセージのフルネーム (package/msg/Name)
        custom_types = get_types_from_msg(msg_text, 'armpi_operation_msgs/msg/RobotCommand')

        # 5. 新しい型を typestore に登録
        typestore.register(custom_types)
        # ▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲

        print(f"カスタムメッセージ 'armpi_operation_msgs/msg/RobotCommand' を登録しました。")

    except Exception as e:
        print(f"Typestoreの初期化/登録に失敗: {e}")
        print("CUSTOM_MSG_FILE のパスが正しいか確認してください。")
        return

    # (これ以降のロジックは、前回の「rgb8」対応版と同じ)

    # メモリ上に一時的にデータをロードするためのリスト
    image_data = []
    image_stamps = []
    joint_stamps = []
    joint_positions = []
    joint_velocities = []
    joint_names = None
    command_data = []
    command_stamps = []

    print(f"Rosbag を読み込み中: {bag_path} ...")
    try:
        with Reader(Path(bag_path)) as reader:
            for connection, timestamp, rawdata in reader.messages():
                try:
                    # 登録済みの typestore を使ってデシリアライズ
                    msg = typestore.deserialize_ros1(rawdata, connection.msgtype)
                except Exception as e:
                    # これで 'armpi_operation_msgs/msg/RobotCommand' が見つかるはず
                    print(f"デシリアライズ失敗: Topic={connection.topic}, Type={connection.msgtype}, Error={e}")
                    continue 

                # 1. 画像データの処理
                if connection.topic == '/collected/image':
                    stamp = get_ros_time(msg.header.stamp)
                    image_stamps.append(stamp)

                    img_bytes = np.frombuffer(msg.data, dtype=np.uint8)

                    try:
                        if 'compressed' in msg.encoding:
                            img_array = cv2.imdecode(img_bytes, cv2.IMREAD_UNCHANGED)
                        elif msg.encoding == 'bgr8':
                            img_array = img_bytes.reshape((msg.height, msg.width, 3))
                        elif msg.encoding == 'rgb8': # rgb8 対応
                            img_array = img_bytes.reshape((msg.height, msg.width, 3))
                        elif msg.encoding == 'mono8':
                            img_array = img_bytes.reshape((msg.height, msg.width))
                        else:
                            print(f"未対応のエンコーディング: {msg.encoding}")
                            continue

                        image_data.append(img_array)

                    except Exception as e:
                        print(f"画像デコードエラー: {e}")
                        image_stamps.pop()

                    # 2. 関節データの処理
                elif connection.topic == '/collected/joint_states':
                    if joint_names is None:
                        joint_names = msg.name

                    joint_stamps.append(get_ros_time(msg.header.stamp))
                    joint_positions.append(msg.position)
                    joint_velocities.append(msg.velocity)

                    # 3. コマンドデータの処理
                elif connection.topic == '/collected/command':
                    command_stamps.append(get_ros_time(msg.header.stamp))
                    command_data.append(extract_command_data(msg))

    except Exception as e:
        print(f"Rosbagの読み込みに失敗しました: {e}")
        return

    print("Rosbag の読み込み完了。HDF5 にデータを書き込みます...")
    # (HDF5への書き込み処理 ... 変更なし)
    with h5py.File(h5_path, 'w') as hf:
# --- 画像データの保存 ---
        if image_data:
            print(f"保存中: Images ({len(image_data)} フレーム)")
            try:
                # データをNumPy配列に変換して保存
                hf.create_dataset('images/data', data=np.stack(image_data), compression="gzip")
                hf.create_dataset('images/timestamps', data=np.array(image_stamps))
            except ValueError as e:
                print(f"警告: 画像サイズが不均一なためNumPyスタックに失敗 {e}")

        # --- 関節データの保存 ---
        if joint_stamps:
            print(f"保存中: Joints ({len(joint_stamps)} サンプル)")
            hf.create_dataset('joints/timestamps', data=np.array(joint_stamps))
            hf.create_dataset('joints/position', data=np.array(joint_positions))
            hf.create_dataset('joints/velocity', data=np.array(joint_velocities))
            # 関節名をUTF-8で保存
            hf.create_dataset('joints/names', data=[n.encode('utf-8') for n in joint_names])

        # --- コマンドデータの保存 ---
        if command_stamps:
            print(f"保存中: Commands ({len(command_stamps)} サンプル)")
            hf.create_dataset('commands/timestamps', data=np.array(command_stamps))
            hf.create_dataset('commands/data', data=np.array(command_data))
            # フィールド名をUTF-8で保存
            hf.create_dataset('commands/fields', data=[f.encode('utf-8') for f in COMMAND_FIELDS])
    print(f"--- 変換完了: {h5_path} ---")


if __name__ == '__main__':
    CUSTOM_MSG_FILE = '~/LIS5/ArmPi/share/src/armpi_operation_msgs/msg/RobotCommand.msg'

    BAG_FILE = 'datasets/data_20251111_021556.bag'
    H5_FILE = 'datasets/output_dataset.h5'

    if 'datasets/data_20251111_021556.bag' in BAG_FILE:
        print(f"スクリプトを実行します: {BAG_FILE}")

    convert_bag_to_h5(BAG_FILE, H5_FILE, CUSTOM_MSG_FILE)
