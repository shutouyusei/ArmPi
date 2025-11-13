import h5py
import pandas as pd
import numpy as np
import cv2
import os

# --- 設定 ---
H5_FILE = 'datasets/bring_up/data_20251111_093855.h5'
OUTPUT_VIDEO_FILE = 'datasets/bring_up/data_20251111_093855.mp4'
# -----------------

def create_video_from_all_frames(h5_path, video_path):
    """
    HDF5内の *すべて* の画像フレームから動画を作成する
    """
    
    # 1. HDF5ファイルを開く
    if not os.path.exists(h5_path):
        print(f"エラー: HDF5ファイルが見つかりません: {h5_path}")
        return

    try:
        hf = h5py.File(h5_path, 'r')
    except Exception as e:
        print(f"HDF5ファイルを開けません: {e}")
        return

    try:
        # 2. 必要なデータセットが存在するか確認
        if 'images/data' not in hf or 'images/timestamps' not in hf:
            print("エラー: HDF5内に /images/data または /images/timestamps が見つかりません。")
            print("convert_bag_to_h5.py が正しく実行されたか確認してください。")
            hf.close()
            return
            
        img_timestamps = hf['images/timestamps']
        img_data = hf['images/data']
        total_frames = len(img_timestamps)

        if total_frames == 0:
            print("エラー: HDF5に画像データが0件です。")
            hf.close()
            return

        print(f"合計 {total_frames} フレームの画像を読み込みます。")

        # 3. 動画の仕様（解像度・FPS）を決定
        
        # 解像度 (H, W, C) を最初の画像から取得
        first_img_shape = img_data[0].shape
        height, width, channels = first_img_shape
        print(f"動画解像度: {width} x {height}")

        # FPS (フレームレート) を全画像のタイムスタンプの平均差から計算
        if total_frames > 1:
            timestamps = np.array(img_timestamps)
            # (最後の時間 - 最初の時間) / (フレーム数 - 1)
            avg_diff = (timestamps[-1] - timestamps[0]) / (total_frames - 1)
            fps = 1.0 / avg_diff
        else:
            fps = 10.0 # フレームが1つしかない場合はデフォルト10FPS
        
        print(f"計算されたFPS (全フレーム): {fps:.2f}")

        # 4. VideoWriter を初期化
        fourcc = cv2.VideoWriter_fourcc(*'mp4v')
        video_writer = cv2.VideoWriter(video_path, fourcc, fps, (width, height))

        if not video_writer.isOpened():
            print("エラー: VideoWriter を開けませんでした。")
            hf.close()
            return

        print(f"動画の書き込み開始: {video_path}")
        
        # 5. すべての画像 (img_data) をループ
        for i in range(total_frames):
            # HDF5から画像データ (RGB) を読み込む
            rgb_img = img_data[i]
            
            # ▼▼▼【重要】RGB -> BGR に変換 ▼▼▼
            bgr_img = cv2.cvtColor(rgb_img, cv2.COLOR_RGB2BGR)
            
            # 動画ファイルにフレームを書き込む
            video_writer.write(bgr_img)
            
            if (i + 1) % 50 == 0: # 50フレームごとに進捗を表示
                print(f"  ... {i+1} / {total_frames} フレーム処理完了")

        print(f"動画の書き込み完了。")

    except Exception as e:
        print(f"動画作成中にエラーが発生しました: {e}")
    finally:
        # 6. リソースを解放
        if 'video_writer' in locals() and video_writer.isOpened():
            video_writer.release()
        hf.close()

# --- メインの実行処理 ---
if __name__ == "__main__":
    create_video_from_all_frames(H5_FILE, OUTPUT_VIDEO_FILE)
