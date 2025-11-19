import pandas as pd
import h5py
import os
import argparse

def check_action_data(h5_path, data_key='sync_data'):
    """
    HDF5ファイル内の /sync_data を読み込み、
    行動（action）カラムの統計情報を表示する
    """
    
    # --- (1) 訓練データと同じカラム名を定義 ---
    # (推論サーバー 'inference_server.py' で定義したものと一致させる)
    ACTION_COLUMNS = [
        "chassis_move_forward",
        "chassis_move_right",
        "angular_right",
        "arm_x",
        "arm_y",
        "arm_z",
        "arm_alpha",
        "rotation",
        "gripper_close",
    ]

    print(f"Checking HDF5 file: {h5_path}")
    
    # --- (2) /sync_data をDataFrameとして読み込む ---
    try:
        df = pd.read_hdf(h5_path, key=data_key)
    except FileNotFoundError:
        print(f"エラー: ファイルが見つかりません: {h5_path}")
        return
    except KeyError:
        print(f"エラー: H5ファイル内にキー '{data_key}' が見つかりません。")
        return
    except Exception as e:
        print(f"H5ファイルの読み込みエラー: {e}")
        return

    print(f"ロード成功: 合計 {len(df)} フレーム")

    # --- (3) 行動カラムの統計情報を表示 ---
    print("\n--- 行動 (Action) カラムの統計情報 ---")
    
    try:
        # アクションカラムだけを抽出
        action_df = df[ACTION_COLUMNS]
        
        # .describe() で統計情報を計算
        # (count, mean, std, min, 25%, 50%, 75%, max)
        stats = action_df.describe()
        
        print(stats)

        # --- (4) 結論のチェック ---
        print("\n--- チェック結果 ---")
        
        # .abs().sum().sum() ですべての行動値の絶対値の合計を計算
        total_action_sum = action_df.abs().sum().sum()
        
        if total_action_sum == 0.0:
            print("🚨 警告: すべての行動データが 0.0 です！")
            print("   -> モデルは「何もしない」ことしか学習できません。")
            print("   -> BAGファイルの録画中にキー操作が正しく記録されたか確認してください。")
        else:
            print("✅ 正常: 行動データに 0 以外の値が含まれています。")

    except KeyError:
        print(f"エラー: H5ファイルの /sync_data に '{ACTION_COLUMNS[0]}' などのカラムが存在しません。")
    except Exception as e:
        print(f"統計の計算中にエラー: {e}")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="HDF5 データセットの行動（Action）カラムをチェックする")
    parser.add_argument(
        '-f', '--file',
        required=True,
        dest='h5_file',
        help="チェック対象の .h5 ファイルへのパス (例: datasets/bring_up/data_xxxx.h5)"
    )
    
    args = parser.parse_args()
    
    check_action_data(args.h5_file)
