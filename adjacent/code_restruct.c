#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct Cube {
    uint64_t pos_bits;     
    uint64_t neg_bits;     
    struct Cube* next;     
} Cube;

// (前述のプール管理関数 alloc_cube, free_cube, free_cube_list 等がある前提)
extern Cube* alloc_cube();
extern void free_cube(Cube* cube);
extern void free_cube_list(Cube* head);

/**
 * 補助関数: リストのディープコピー（複製）を作る
 * 集合演算で元のリストを破壊しないために使用します
 */
Cube* duplicate_list(const Cube* head) {
    Cube* new_head = NULL;
    Cube* new_tail = NULL;
    const Cube* curr = head;
    
    while (curr != NULL) {
        Cube* copy = alloc_cube();
        if (!copy) return NULL;
        copy->pos_bits = curr->pos_bits;
        copy->neg_bits = curr->neg_bits;
        copy->next = NULL;
        
        if (new_head == NULL) {
            new_head = copy;
            new_tail = copy;
        } else {
            new_tail->next = copy;
            new_tail = copy;
        }
        curr = curr->next;
    }
    return new_head;
}

/**
 * 1. 10のリストと01のリストのORをとる関数 (集合Fの作成)
 * SOP（積和形）におけるORは、単純なリストの結合（および複製）です。
 */
Cube* create_union_F(const Cube* list_10, const Cube* list_01) {
    // 元のリストを書き換えないよう、双方のコピーをとって結合する
    Cube* head_10 = duplicate_list(list_10);
    Cube* head_01 = duplicate_list(list_01);
    
    if (head_10 == NULL) return head_01;
    if (head_01 == NULL) return head_10;
    
    // 10のリストの末尾を探す
    Cube* tail = head_10;
    while (tail->next != NULL) {
        tail = tail->next;
    }
    // 10の末尾に01の先頭を繋ぐ (OR結合)
    tail->next = head_01;
    
    return head_10; // これが集合F
}

/**
 * 補助関数: 1つのCubeとUniverseのANDをとる
 * ※ もしuniverseが「特定の変数だけを指定したCube」である場合、
 *    お互いに矛盾（同じ変数で一方が10、他方が01）があれば、ANDの結果は消滅(NULL)します。
 */
Cube* intersect_cube_with_universe(const Cube* c, const Cube* uni) {
    uint64_t p = c->pos_bits & uni->pos_bits;
    uint64_t n_b = c->neg_bits & uni->neg_bits;
    
    // 矛盾チェック: どこかの変数で pos も neg も 0 になっていたら、そのCubeは消滅
    // (p | n_b) のどこかのビットが 0 になっている ＝ 矛盾
    // 64ビットすべてが矛盾していないことを確認（全ビットが1または01/10のいずれかであること）
    // 今回の条件「下位nビット以外は1」を考慮すると、(~p & ~n_b) が1を持つなら矛盾。
    if ((~p & ~n_b) != 0ULL) {
        return NULL; // 矛盾が生じたのでこのCubeは消滅
    }
    
    Cube* res = alloc_cube();
    if (!res) return NULL;
    res->pos_bits = p;
    res->neg_bits = n_b;
    return res;
}

/**
 * アルゴリズム実行関数
 * @param list_10 : 出力10のリスト
 * @param list_01 : 出力01のリスト
 * @param universe : 制限をかける対象の空間Cube (例: x0=1 の空間など)
 */
/**
 * アルゴリズム実行関数（各項にビットごとのORを適用する修正版）
 * 
 * 1. 集合FとuniverseのANDをとり、集合F'とする。
 * 2. F'のすべてのCubeに対して、universeの否定をビットごとにORし、集合Gとする。
 */
Cube* compute_restriction_optimized(const Cube* list_10, const Cube* list_01, const Cube* universe) {
    // --- 準備: 集合Fの作成（10のリストと01のリストのOR＝リスト結合） ---
    Cube* F = create_union_F(list_10, list_01);
    
    Cube* G_head = NULL;
    Cube* G_tail = NULL;
    Cube* curr_F = F;
    
    // --- universe の否定（NOT）をあらかじめビット演算で作っておく ---
    // universeが固定しているビット（0になっている部分）を逆転させたマスクを作ります
    // 例: universeが x0=1 (下位4ビットが 0111 / 1011) のとき、
    //     not_uni_pos = 1000, not_uni_neg = 0100 となり、合わせるとその変数が11(Don't Care)化します。
    uint64_t not_uni_pos = ~universe->pos_bits;
    uint64_t not_uni_neg = ~universe->neg_bits;
    
    // --- 1 & 2. Fの各項に対してANDをとった後、ビットごとにuniverseの否定をORする ---
    while (curr_F != NULL) {
        // 1. universe との AND (共通部分) を計算
        uint64_t p = curr_F->pos_bits & universe->pos_bits;
        uint64_t n_b = curr_F->neg_bits & universe->neg_bits;
        
        // 矛盾チェック: どこかの変数で pos も neg も 0 になっていたら、この項は消滅（スキップ）
        if ((~p & ~n_b) != 0ULL) {
            curr_F = curr_F->next;
            continue; 
        }
        
        // 2. 【ご指定の処理】生存した項に対して、universe の否定をビットごとに OR
        p |= not_uni_pos;
        n_b |= not_uni_neg;
        
        // 結果を格納する新しいCubeをプールから取得
        Cube* res_cube = alloc_cube();
        if (res_cube != NULL) {
            res_cube->pos_bits = p;
            res_cube->neg_bits = n_b;
            res_cube->next = NULL;
            
            // 集合Gのリストに繋ぐ
            if (G_head == NULL) {
                G_head = res_cube;
                G_tail = res_cube;
            } else {
                G_tail->next = res_cube;
                G_tail = res_cube;
            }
        }
        
        curr_F = curr_F->next;
    }
    
    // 中間リスト F のメモリを解放
    free_cube_list(F);
    
    return G_head; // これが最終的な集合G（制限された関数のSOP）
}