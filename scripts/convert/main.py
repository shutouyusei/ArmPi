from convert_bag_to_h5 import convert_bag_to_h5,synchronize_data,load_h5_to_dataframes
import argparse
from pathlib import Path

# 1. 引数の設定
parser = argparse.ArgumentParser(description="RosbagをHDF5に変換し、データを同期・クリーニングするスクリプト")
parser.add_argument(
    '-f',
    '--foldername',
    required=True,
    dest='foldername',
    help="拡張子なしの入力BAGファイル名 (例: data_001)"
)
args = parser.parse_args()

p = Path("datasets/" + args.foldername)
bag_files = list(p.glob("*.bag"))

TOLERANCE_MS = 50

for BAG_FILE in bag_files:
    H5_FILE = BAG_FILE.parent / (BAG_FILE.stem + ".h5")

    print(f"--- ステップ 1/3: Rosbag から H5 への変換開始 ---")
    print(f"入力BAG: {BAG_FILE}")
    print(f"出力H5:  {H5_FILE}")

    # 3. BAG -> H5 生データ変換
    success = convert_bag_to_h5(BAG_FILE, H5_FILE)

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
