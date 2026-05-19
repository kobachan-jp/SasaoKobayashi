#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct Cube {
    uint64_t pos_bits;     
    uint64_t neg_bits;     
    struct Cube* next;     
} Cube;

// (前述のプール管理・制限関数がある前提。再帰のためにここで宣言)
extern Cube* alloc_cube();
extern void free_cube(Cube* cube);
extern void free_cube_list(Cube* head);
extern Cube* compute_restriction_optimized(const Cube* list_10, const Cube* list_01, const Cube* universe);

// 単一のCube同士のAND（積）を計算するヘルパー関数
Cube* intersect_two_cubes(const Cube* a, const Cube* b) {
    uint64_t p = a->pos_bits & b->pos_bits;
    uint64_t n_b = a->neg_bits & b->neg_bits;
    
    if ((~p & ~n_b) != 0ULL) return NULL; // 矛盾（空集合）
    
    Cube* res = alloc_cube();
    if (!res) return NULL;
    res->pos_bits = p;
    res->neg_bits = n_b;
    return res;
}

// 2つのリストを破壊的に結合するヘルパー関数 (OR演算)
Cube* append_lists_destructive(Cube* list1, Cube* list2) {
    if (!list1) return list2;
    if (!list2) return list1;
    Cube* tail = list1;
    while (tail->next != NULL) tail = tail->next;
    tail->next = list2;
    return list1;
}

/**
 * 1. 最も 0 と 1 の数が拮抗している（同じ値になる）ビットを探す関数
 * @param G : 対象の集合
 * @param n : 有効な変数の個数
 * @return 選択されたビットのインデックス (0 ~ n-1)。見つからない場合は -1
 */
int find_best_pivot_bit(const Cube* G, int n) {
    if (!G) return -1;
    
    int best_bit = -1;
    int min_difference = 999999; // 差の最小値を追跡

    for (int i = 0; i < n; i++) {
        uint64_t mask = (1ULL << (n - 1 - i)); // 左シフト方式の並び順に対応
        int count_0 = 0;
        int count_1 = 0;
        
        const Cube* curr = G;
        while (curr != NULL) {
            bool has_pos = (curr->pos_bits & mask) != 0;
            bool has_neg = (curr->neg_bits & mask) != 0;
            
            if (has_pos && !has_neg) count_1++;      // 肯定 '1'
            else if (!has_pos && has_neg) count_0++; // 否定 '0'
            // 両方1の場合は Don't Care '-' なのでカウントしない
            
            curr = curr->next;
        }
        
        // 0と1のどちらも現れない（すべてDCなど）変数はスキップ
        if (count_0 == 0 && count_1 == 0) continue;
        
        // 差の絶対値を計算
        int diff = (count_0 > count_1) ? (count_0 - count_1) : (count_1 - count_0);
        
        // より拮抗している（差が小さい）ビットを更新
        if (diff < min_difference) {
            min_difference = diff;
            best_bit = i;
        }
    }
    return best_bit;
}

/**
 * 単一のCubeの否定（NOT）を求める関数
 * 2ビット表現において、1つの項の否定は「各リテラルを反転させたCubeの集合（リスト）」になります
 */
Cube* invert_single_cube(const Cube* c, int n) {
    Cube* res_head = NULL;
    Cube* res_tail = NULL;
    
    for (int i = 0; i < n; i++) {
        uint64_t mask = (1ULL << (n - 1 - i));
        bool has_pos = (c->pos_bits & mask) != 0;
        bool has_neg = (c->neg_bits & mask) != 0;
        
        Cube* inverted = NULL;
        if (has_pos && !has_neg) {
            // '1' の否定は '0'
            inverted = alloc_cube();
            if (inverted) {
                inverted->pos_bits = ~0ULL & ~mask; // そのビットのposを落とす
                inverted->neg_bits = ~0ULL;
            }
        } else if (!has_pos && has_neg) {
            // '0' の否定は '1'
            inverted = alloc_cube();
            if (inverted) {
                inverted->pos_bits = ~0ULL;
                inverted->neg_bits = ~0ULL & ~mask; // そのビットのnegを落とす
            }
        }
        
        if (inverted) {
            if (!res_head) { res_head = inverted; res_tail = inverted; }
            else { res_tail->next = inverted; res_tail = inverted; }
        }
    }
    return res_head;
}

/**
 * 集合Gの否定を再帰的に計算するコア関数
 * @param G : 否定対象の集合(リスト)
 * @param c_accum : これまでの再帰で蓄積された「制限をかける項」の積（初期値はUniverse）
 * @param n : 変数の個数
 */
Cube* compute_complement_rec(Cube* G, const Cube* c_accum, int n) {
    // 【ベースケース1】集合Gが空（0）なら、その否定はUniverse全体。
    // returnは「制限をかける項c」と「否定(Universe)」の積なので、c_accumのコピーを返す。
    if (G == NULL) {
        Cube* res = alloc_cube();
        if (res) { res->pos_bits = c_accum->pos_bits; res->neg_bits = c_accum->neg_bits; }
        return res;
    }
    
    // 【ベースケース2】項数が1つになったら再帰を終了
    // 「制限をかける項c_accum」と「集合Gの単一項の否定」の積を計算して返す
    if (G->next == NULL) {
        // 1つの項Gの否定を計算
        Cube* G_not = invert_single_cube(G, n);
        
        // それと現在の蓄積項 c_accum との積（AND）をとる
        Cube* final_res_head = NULL;
        Cube* final_res_tail = NULL;
        Cube* curr = G_not;
        
        while (curr != NULL) {
            Cube* intersected = intersect_two_cubes(c_accum, curr);
            if (intersected) {
                if (!final_res_head) { final_res_head = intersected; final_res_tail = intersected; }
                else { final_res_tail->next = intersected; final_res_tail = intersected; }
            }
            curr = curr->next;
        }
        free_cube_list(G_not);
        return final_res_head;
    }
    
    // 1. 最も0と1の数が拮抗しているビットを探す
    int pivot = find_best_pivot_bit(G, n);
    
    // もし拮抗するビットがない（すべての項が完全にDCなど）場合の安全弁
    if (pivot == -1) {
        Cube* G_not = invert_single_cube(G, n); // 先頭の項で代用
        // c_accumとの積をとる処理（同上）
        Cube* final_res_head = NULL; Cube* final_res_tail = NULL; Cube* curr = G_not;
        while (curr != NULL) {
            Cube* intersected = intersect_two_cubes(c_accum, curr);
            if (intersected) {
                if (!final_res_head) { final_res_head = intersected; final_res_tail = intersected; }
                else { final_res_tail->next = intersected; final_res_tail = intersected; }
            }
            curr = curr->next;
        }
        free_cube_list(G_not);
        return final_res_head;
    }
    
    uint64_t mask = (1ULL << (n - 1 - pivot));
    
    // 2. c1 (そのbitを1、他をDC) と c2 (そのbitを0、他をDC) を作成
    Cube c1 = { .pos_bits = ~0ULL, .neg_bits = ~0ULL & ~mask, .next = NULL }; // '1'
    Cube c2 = { .pos_bits = ~0ULL & ~mask, .neg_bits = ~0ULL, .next = NULL }; // '0'
    
    // これまでの累積制限項に、今回のブランチを掛け合わせる
    Cube* next_c1 = intersect_two_cubes(c_accum, &c1);
    Cube* next_c2 = intersect_two_cubes(c_accum, &c2);
    
    // 3. 制限 G1 と G2 を作成
    // ※ compute_restriction_optimized は内部で 10 と 01 のリストのOR（結合）を求める設計ですが、
    // 既に結合された1つの集合Gを渡すため、第2引数は NULL を指定します
    Cube* G1 = compute_restriction_optimized(G, NULL, &c1);
    Cube* G2 = compute_restriction_optimized(G, NULL, &c2);
    
    // 4. 再帰呼び出し
    Cube* res1 = NULL;
    Cube* res2 = NULL;
    if (next_c1) res1 = compute_complement_rec(G1, next_c1, n);
    if (next_c2) res2 = compute_complement_rec(G2, next_c2, n);
    
    // 一時的な制限リストと蓄積項の解放
    free_cube_list(G1);
    free_cube_list(G2);
    if (next_c1) free_cube(next_c1);
    if (next_c2) free_cube(next_c2);
    
    // 両方のブランチの結果を OR（リスト結合）して返す
    return append_lists_destructive(res1, res2);
}

/**
 * 外部から呼び出すためのラッパー関数
 */
Cube* invert_logic_function(Cube* G, int n) {
    // 最初のcは、すべての空間を含む Universe (全ビット1)
    Cube universe = { .pos_bits = ~0ULL, .neg_bits = ~0ULL, .next = NULL };
    return compute_complement_rec(G, &universe, n);
}