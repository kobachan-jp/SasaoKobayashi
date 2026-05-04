import os
import random
import math

# --- 設定項目 ---
INPUT_SIZE = 28   # ヘルスケアデータは28。optdigitsの時は64に変更
OUTPUT_SIZE = 2   # ヘルスケアデータは2。optdigitsの時は10に変更
INPUT_EXT = ".tru"
OUTPUT_DIR = "espresso_project"

# 出力用フォルダの作成
if not os.path.exists(OUTPUT_DIR):
    os.makedirs(OUTPUT_DIR)

def split_into_ten(lst):
    """リストをなるべく均等に10分割する"""
    random.shuffle(lst)
    n = len(lst)
    size = n // 10
    remainder = n % 10
    chunks = []
    start = 0
    for i in range(10):
        # 余りを1つずつ割り振ってサイズを調整
        end = start + size + (1 if i < remainder else 0)
        chunks.append(lst[start:end])
        start = end
    return chunks

def process_file(filepath):
    filename = os.path.basename(filepath)
    base_name = os.path.splitext(filename)[0]
    
    with open(filepath, 'r', encoding='utf-8') as f:
        lines = [line.strip() for line in f if line.strip() and not line.startswith('.')]

    # 1. データの分類 (Positive: 10, Negative: 01)
    pos_data = [l for l in lines if l.endswith("10")]
    neg_data = [l for l in lines if l.endswith("01")]

    # 2. それぞれを10等分（層化分割）
    pos_folds = split_into_ten(pos_data)
    neg_folds = split_into_ten(neg_data)

    # 3. 10-CV用のデータ作成
    for i in range(10):
        # i番目をテスト用、それ以外を学習用にする
        test_set = pos_folds[i] + neg_folds[i]
        train_set = []
        for j in range(10):
            if i != j:
                train_set += pos_folds[j] + neg_folds[j]

        # --- 学習用PLAの書き出し ---
        train_filename = f"{base_name}_fold{i+1}_train.pla"
        with open(os.path.join(OUTPUT_DIR, train_filename), 'w') as f:
            f.write(f".i {INPUT_SIZE}\n")
            f.write(f".o {OUTPUT_SIZE}\n")
            f.write(".type fr\n")
            for line in train_set:
                f.write(f"{line}\n")
            f.write(".e\n")

        # --- テスト用データの書き出し（後のシミュレーション用） ---
        test_filename = f"{base_name}_fold{i+1}_test.txt"
        with open(os.path.join(OUTPUT_DIR, test_filename), 'w') as f:
            for line in test_set:
                f.write(f"{line}\n")

    print(f"完了: {filename} を10分割しました。")
    # --- 分割の正しさを確認するための表示 ---
    print(f"\n【確認レポート: {filename}】")
    print(f"元データ合計: {len(lines)}行 (Pos:{len(pos_data)}, Neg:{len(neg_data)})")
    
    for i in range(10):
        t_pos = len([l for l in pos_folds[i]])
        t_neg = len([l for l in neg_folds[i]])
        print(f" Fold{i+1}: テストデータ計 {t_pos + t_neg}行 (内訳 Pos:{t_pos}, Neg:{t_neg})")

# 実行メイン処理
if __name__ == "__main__":
    files = [f for f in os.listdir('.') if f.endswith(INPUT_EXT)]
    if not files:
        print(f"{INPUT_EXT} ファイルが見つかりません。")
    else:
        for f in files:
            process_file(f)
        print(f"\nすべての処理が終了しました。'{OUTPUT_DIR}' フォルダを確認してください。")

