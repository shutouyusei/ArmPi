# --- ライブラリのインポート (すべてマージ) ---
import h5py
import pandas as pd
import numpy as np
import os
import argparse
import cv2
from os.path import expanduser # ホームディレクトリ(~)展開用
from pathlib import Path # Path オブジェクトを使うためにインポート
from rosbags.rosbag1 import Reader # rosbags v0.11.0
from rosbags.typesys import Stores, get_typestore
from rosbags.typesys import get_types_from_msg # スニペットから

# --- 定数定義 ---

# コマンドメッセージのフィールド定義
COMMAND_FIELDS = [
    "base_vel_linear_x", "base_vel_linear_y", "base_vel_linear_z",
    "base_vel_angular_x", "base_vel_angular_y", "base_vel_angular_z",
    "arm_x", "arm_y", "arm_z", "gripper", "arm_alpha", "arm_alpha1",
    "arm_alpha2", "gripper_position"
]

# 同期のための許容誤差 (± 50ミリ秒)

# カスタムMSGファイルへのパス
custom_msg_file = '~/LIS5/ArmPi/share/src/armpi_operation_msgs/msg/RobotCommand.msg'

# --- 関数定義 1: BAG -> H5 変換ヘルパー ---

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

# --- 関数定義 2: BAG -> H5 変換メイン ---

def convert_bag_to_h5(bag_path, h5_path):
    """
    ROS1の.bagファイルをHDF5ファイルに変換する (生データを保存)
    """
    print(f"HDF5ファイルを作成します: {h5_path}")

    # H5ファイルが置かれるディレクトリを作成
    os.makedirs(Path(h5_path).parent, exist_ok=True)

    try:
        # 1. 標準メッセージストア(NOETIC)をロード
        typestore = get_typestore(Stores.ROS1_NOETIC)

        # 2. カスタム .msg ファイルのパスを解決
        msg_file_path = Path(expanduser(custom_msg_file))
        print(f"カスタムMSGファイルを読み込みます: {msg_file_path}")

        # 3. .msg ファイルの内容をテキストとして読み込む
        msg_text = msg_file_path.read_text()

        # 4. メッセージ定義を辞書として取得
        custom_types = get_types_from_msg(msg_text, 'armpi_operation_msgs/msg/RobotCommand')

        # 5. 新しい型を typestore に登録
        typestore.register(custom_types)
        print(f"カスタムメッセージ 'armpi_operation_msgs/msg/RobotCommand' を登録しました。")

    except Exception as e:
        print(f"Typestoreの初期化/登録に失敗: {e}")
        print("CUSTOM_MSG_FILE のパスが正しいか確認してください。")
        return False

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
    if not os.path.exists(bag_path):
        print(f"エラー: Rosbagファイルが見つかりません: {bag_path}")
        return False

    try:
        with Reader(Path(bag_path)) as reader:
            for connection, timestamp, rawdata in reader.messages():
                try:
                    # 登録済みの typestore を使ってデシリアライズ
                    msg = typestore.deserialize_ros1(rawdata, connection.msgtype)
                except Exception as e:
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
        return False

    print("Rosbag の読み込み完了。HDF5 に生データを書き込みます...")

    with h5py.File(h5_path, 'w') as hf:
        # --- 画像データの保存 ---
        if image_data:
            print(f"保存中: Images ({len(image_data)} フレーム)")
            try:
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
            hf.create_dataset('joints/names', data=[n.encode('utf-8') for n in joint_names])

        # --- コマンドデータの保存 ---
        if command_stamps:
            print(f"保存中: Commands ({len(command_stamps)} サンプル)")
            hf.create_dataset('commands/timestamps', data=np.array(command_stamps))
            hf.create_dataset('commands/data', data=np.array(command_data))
            hf.create_dataset('commands/fields', data=[f.encode('utf-8') for f in COMMAND_FIELDS])

    print(f"--- 生データのH5変換完了: {h5_path} ---")
    return True

# --- 関数定義 3: H5 -> DataFrame 読み込み ---

def load_h5_to_dataframes(h5_path):
    """HDF5 ファイルを読み込み、3つの Pandas DataFrame に変換する"""

    dataframes = {}

    with h5py.File(h5_path, 'r') as hf:

        # 1. 画像 (Images) の DataFrame
        if 'images' in hf:
            img_stamps = np.array(hf['images/timestamps'])
            img_indices = np.arange(len(img_stamps))

            df_img = pd.DataFrame({
                'timestamp': img_stamps,
                'img_index': img_indices
            })
            df_img = df_img.sort_values(by='timestamp').reset_index(drop=True)
            dataframes['images'] = df_img
            print(f"画像 {len(df_img)} 件をロード")

        # 2. 関節 (Joints) の DataFrame
        if 'joints' in hf:
            joint_stamps = np.array(hf['joints/timestamps'])
            joint_pos = np.array(hf['joints/position'])
            joint_vel = np.array(hf['joints/velocity'])
            joint_names = [name.decode('utf-8') for name in hf['joints/names']]

            df_joint = pd.DataFrame(
                joint_pos, 
                columns=[f"{name}_pos" for name in joint_names]
            )
            df_joint_vel = pd.DataFrame(
                joint_vel,
                columns=[f"{name}_vel" for name in joint_names]
            )
            df_joint['timestamp'] = joint_stamps
            df_joint = pd.concat([df_joint, df_joint_vel], axis=1) 

            df_joint = df_joint.sort_values(by='timestamp').reset_index(drop=True)
            dataframes['joints'] = df_joint
            print(f"関節 {len(df_joint)} 件をロード")

        # 3. コマンド (Commands) の DataFrame
        if 'commands' in hf:
            cmd_stamps = np.array(hf['commands/timestamps'])
            cmd_data = np.array(hf['commands/data'])
            cmd_fields = [f.decode('utf-8') for f in hf['commands/fields']]

            df_cmd = pd.DataFrame(cmd_data, columns=cmd_fields)
            df_cmd['timestamp'] = cmd_stamps

            df_cmd = df_cmd.sort_values(by='timestamp').reset_index(drop=True)
            dataframes['commands'] = df_cmd
            print(f"コマンド {len(df_cmd)} 件をロード")

    return dataframes

# --- 関数定義 4: データ同期 ---

def synchronize_data(dataframes, tolerance_sec):
    """
    merge_asof を使って、画像を基準にデータを同期する
    """

    if 'images' not in dataframes:
        print("基準となる画像データがありません。同期を中止します。")
        return None

    # 基準となる DataFrame
    df_sync = dataframes['images']

    print(f"同期中... (許容誤差: ±{tolerance_sec * 1000:.0f} ms)")

    # 1. 画像(df_sync) に 関節(df_joints) をマージ
    if 'joints' in dataframes:
        df_sync = pd.merge_asof(
            df_sync,
            dataframes['joints'],
            on='timestamp',         # タイムスタンプ列で
            direction='nearest',    # 時間的に最も近いもの
            tolerance=tolerance_sec # 許容誤差内
        )

    # 2. (1の結果) に コマンド(df_cmd) をマージ
    if 'commands' in dataframes:
        df_sync = pd.merge_asof(
            df_sync,
            dataframes['commands'],
            on='timestamp',
            direction='nearest',
            tolerance=tolerance_sec
        )

    return df_sync

# --- メインの実行処理 ---
if __name__ == "__main__":

    # 1. 引数の設定
    parser = argparse.ArgumentParser(description="RosbagをHDF5に変換し、データを同期・クリーニングするスクリプト")
    parser.add_argument(
        '--filename',
        required=True,
        dest='filename',
        help="拡張子なしの入力BAGファイル名 (例: data_001)"
    )
    args = parser.parse_args()

    # 2. ファイルパスの定義
    BAG_FILE = f'datasets/{args.filename}.bag'
    H5_FILE = f'datasets/{args.filename}.h5' # 生データと同期データを両方保存するH5ファイル

    print(f"--- ステップ 1/3: Rosbag から H5 への変換開始 ---")
    print(f"入力BAG: {BAG_FILE}")
    print(f"出力H5:  {H5_FILE}")

    # 3. BAG -> H5 生データ変換
    success = convert_bag_to_h5(BAG_FILE, H5_FILE, CUSTOM_MSG_FILE)

    if success:
        print(f"\n--- ステップ 2/3: H5 データの同期処理開始 ---")

        # 4. H5からDataFrameをロード
        dfs = load_h5_to_dataframes(H5_FILE)

        # 5. データを同期
        tolerance_seconds = TOLERANCE_MS / 1000.0
        df_final = synchronize_data(dfs, tolerance_seconds)

        if df_final is not None:

            # 6. 許容誤差内にデータがなかった行 (NaN) を削除
            original_count = len(df_final)
            df_final = df_final.dropna()
            cleaned_count = len(df_final)

            print(f"\n分析完了: {original_count} フレーム -> {cleaned_count} フレームにクリーニング")

            if cleaned_count > 0:
                print(f"\n--- ステップ 3/3: 同期済みデータをH5に追記 ---")

                try:
                    # 7. 同期済みDataFrameをH5ファイルに 'sync_data' キーで追記
                    df_final.to_hdf(H5_FILE, key='sync_data', mode='a', format='table')
                    print(f"同期済みデータ ({cleaned_count}件) を '{H5_FILE}' に 'sync_data' として保存しました。")
                    print("\n--- 処理完了 ---")

                except Exception as e:
                    print(f"エラー: 同期済みデータのHDF5への保存に失敗しました。 {e}")
                    print("ライブラリ 'tables' がインストールされているか確認してください (pip install tables)")
            else:
                print("同期できるデータが0件だったため、'sync_data' は保存されませんでした。")
    else:
        print(f"H5ファイルへの変換に失敗したため、処理を中断しました。")
