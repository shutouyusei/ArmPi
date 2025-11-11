import h5py
import pandas as pd
import numpy as np
import os

# --- 設定 ---
H5_FILE = 'datasets/output_dataset.h5'
OUTPUT_CSV = 'datasets/synchronized_data.csv' # 分析結果の確認用

# 同期のための許容誤差 (± 50ミリ秒)
TOLERANCE_MS = 50 
# -----------------

def load_h5_to_dataframes(h5_path):
    """HDF5 ファイルを読み込み、3つの Pandas DataFrame に変換する"""

    dataframes = {}

    with h5py.File(h5_path, 'r') as hf:

        # 1. 画像 (Images) の DataFrame
        if 'images' in hf:
            img_stamps = np.array(hf['images/timestamps'])
            # DataFrame には画像本体(data)ではなく、
            # H5ファイル内のインデックス(img_index)を保存する
            img_indices = np.arange(len(img_stamps))

            df_img = pd.DataFrame({
                'timestamp': img_stamps,
                'img_index': img_indices
            })
            # タイムスタンプでソート (merge_asof に必須)
            df_img = df_img.sort_values(by='timestamp').reset_index(drop=True)
            dataframes['images'] = df_img
            print(f"画像 {len(df_img)} 件をロード")

        # 2. 関節 (Joints) の DataFrame
        if 'joints' in hf:
            joint_stamps = np.array(hf['joints/timestamps'])
            joint_pos = np.array(hf['joints/position'])
            joint_vel = np.array(hf['joints/velocity'])
            # 関節名をデコード
            joint_names = [name.decode('utf-8') for name in hf['joints/names']]

            # DataFrame を作成
            df_joint = pd.DataFrame(
                joint_pos, 
                columns=[f"{name}_pos" for name in joint_names]
            )
            df_joint_vel = pd.DataFrame(
                joint_vel,
                columns=[f"{name}_vel" for name in joint_names]
            )
            df_joint['timestamp'] = joint_stamps
            df_joint = pd.concat([df_joint, df_joint_vel], axis=1) # 位置と速度を結合

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
    if not os.path.exists(H5_FILE):
        print(f"エラー: HDF5 ファイルが見つかりません: {H5_FILE}")
    else:
        # 1. H5から全データをDataFrameとしてロード
        dfs = load_h5_to_dataframes(H5_FILE)

        # 2. データを同期
        tolerance_seconds = TOLERANCE_MS / 1000.0
        df_final = synchronize_data(dfs, tolerance_seconds)

        if df_final is not None:
            print("\n--- 同期前 (抜粋) ---")
            print(df_final.head(10))

            # 3. 許容誤差内にデータがなかった行 (NaN) を削除
            #    (コマンドや関節が記録されていない画像フレームは除外)
            original_count = len(df_final)
            df_final = df_final.dropna()
            cleaned_count = len(df_final)

            print("\n--- 同期・クリーニング後 (抜粋) ---")
            print(df_final.head(10))

            print(f"\n分析完了: {original_count} フレーム -> {cleaned_count} フレームにクリーニング")

            # 4. (確認用) 結果をCSVに保存
            df_final.to_csv(OUTPUT_CSV, index=False)
            print(f"同期済みデータを {OUTPUT_CSV} に保存しました。")
