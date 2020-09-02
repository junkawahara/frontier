//
// MateSTPath.cpp
//
// Copyright (c) 2012 -- 2016 Jun Kawahara
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software
// and associated documentation files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or
// substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
// BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include <cstring>
#include <cassert>

#include "StateSTPath.hpp"

namespace frontier_lib {

using namespace std;

//*************************************************************************************************
// StateSTPath

void StateSTPath::UnpackMate(ZDDNode* node, Mate* mate, int child_num)
{
    StateFrontier<MateSTPath>::UnpackMate(node, mate, child_num);

    MateSTPath* m = static_cast<MateSTPath*>(mate);

    if (child_num == 0) { // Hi枝の場合は既にunpackされているので、無視する

        if (IsCycle()) { // サイクルの場合
            for (int i = 0; i < frontier_manager_.GetEnteringFrontierSize(); ++i) {
                int v = frontier_manager_.GetEnteringFrontierValue(i);
                m->frontier[v] = v;
            }
        } else { // サイクルではない場合
            // 処理速度効率化のため、s と t はあらかじめつながれていると考え、
            // s-t パスを作るのではなく、サイクルを作ると考える。
            // したがって、初めて s (or t) がフロンティア上に出現したとき、
            // その mate 値を t (or s) に設定する。
            // ただし、mate[x] = s (or t) となる x が既にフロンティア上に存在する場合は、
            // mate[s (or t)] = x に設定する

            for (int i = 0; i < frontier_manager_.GetEnteringFrontierSize(); ++i) {
                int v = frontier_manager_.GetEnteringFrontierValue(i);
                m->frontier[v] = v;
                if (v == start_vertex_) {
                    m->frontier[v] = end_vertex_;
                    for (int j = 0; j < frontier_manager_.GetPreviousFrontierSize(); ++j) {
                        if (m->frontier[frontier_manager_.GetPreviousFrontierValue(j)] == start_vertex_) {
                            m->frontier[v] = frontier_manager_.GetPreviousFrontierValue(j);
                            break;
                        }
                    }
                } else if (v == end_vertex_) {
                    m->frontier[v] = start_vertex_;
                    for (int j = 0; j < frontier_manager_.GetPreviousFrontierSize(); ++j) {
                        if (m->frontier[frontier_manager_.GetPreviousFrontierValue(j)] == end_vertex_) {
                            m->frontier[v] = frontier_manager_.GetPreviousFrontierValue(j);
                            break;
                        }
                    }
                }
            }
        }
    }
}

void StateSTPath::UpdateMate(MateSTPath* mate, int child_num)
{
    Edge edge = GetCurrentEdge();

    if (child_num == 1) { // Hi枝のとき
        int sm = mate->frontier[edge.src];
        int dm = mate->frontier[edge.dest];

        // 辺をつないだときの mate の更新
        // （↓の計算順を変更すると正しく動作しないことに注意）
        mate->frontier[edge.src] = 0;
        mate->frontier[edge.dest] = 0;
        mate->frontier[sm] = dm;
        mate->frontier[dm] = sm;
    }
}

// mate update 前の終端判定。
// 0終端なら 0, 1終端なら 1, どちらでもない場合は -1 を返す。
int StateSTPath::CheckTerminalPre(MateSTPath* mate, int child_num)
{
    if (child_num == 0) { // Lo枝のとき
        return -1;
    } else { // Hi枝のとき
        Edge edge = GetCurrentEdge();

        if (mate->frontier[edge.src] == 0 || mate->frontier[edge.dest] == 0) {
            // 分岐が発生
            return 0;
        } else if (mate->frontier[edge.src] == edge.dest) {
            // サイクルが完成

            if (!IsCycle() && !STEnteringFrontier()) { // s or t がまだフロンティアに入っていない
                return 0;
            }

            // フロンティアに属する残りの頂点についてチェック

            for (int i = 0; i < frontier_manager_.GetNextFrontierSize(); ++i) {
                mate_t v = frontier_manager_.GetNextFrontierValue(i);
                // 張った辺の始点と終点、及びs,tはチェックから除外
                if (v != edge.src && v != edge.dest) {
                    if (IsHamilton()) { // ハミルトンパス、サイクルの場合
                        if (mate->frontier[v] != 0) {
                            return 0;
                        }
                    } else {
                        // パスの途中でなく、孤立した点でもない場合、
                        // 不完全なパスができることになるので、0を返す
                        if (mate->frontier[v] != 0 && mate->frontier[v] != v) { // v の次数が 1
                            return 0;
                        }
                    }
                }
            }
            if (IsHamilton()) { // ハミルトンパス、サイクルの場合
                if (frontier_manager_.IsExistUnprocessedVertex(current_edge_)) {
                    return 0;
                }
            }
            return 1;
        } else {
            return -1;
        }
    }
}

// mate update 後の終端判定。
// 0終端なら 0, 1終端なら 1, どちらでもない場合は -1 を返す。
int StateSTPath::CheckTerminalPost(MateSTPath* mate)
{
    for (int i = 0; i < frontier_manager_.GetLeavingFrontierSize(); ++i) {
        mate_t v = frontier_manager_.GetLeavingFrontierValue(i); // フロンティアから抜ける頂点 v
        if (IsHamilton()) { // ハミルトンパス、サイクルの場合
            if (mate->frontier[v] != 0) { // v の次数が 0 or 1
                return 0;
            }
        } else {
            if (mate->frontier[v] != 0 && mate->frontier[v] != v) { // v の次数が 1
                return 0;
            }
        }
    }
    if (IsLastEdge()) {
        return 0;
    } else {
        return -1;
    }
}

} // the end of the namespace
