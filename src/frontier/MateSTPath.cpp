//
// MateSTPath.cpp
//
// Copyright (c) 2012 Jun Kawahara
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

#include "MateSTPath.hpp"

namespace frontier_dd {

using namespace std;

//*************************************************************************************************
// StateSTPath

Mate* StateSTPath::MakeEmptyMate()
{
   return new MateSTPath(this);
}

void StateSTPath::UnpackMate(ZDDNode* node, Mate* mate, int child_num)
{
    StateFrontier<mate_t>::UnpackMate(node, mate, child_num);

    if (child_num == 0) { // Hi枝の場合は既にunpackされているので、無視する

        if (!IsCycle()) { // サイクルではない場合
            // 処理速度効率化のため、s と t はあらかじめつながれていると考え、
            // s-t パスを作るのではなく、サイクルを作ると考える。
            // したがって、初めて s (or t) がフロンティア上に出現したとき、
            // その mate 値を t (or s) に設定する。
            // ただし、mate[x] = s (or t) となる x が既にフロンティア上に存在する場合は、
            // mate[s (or t)] = x に設定する
            mate_t* frontier_array = static_cast<MateSTPath*>(mate)->frontier_array;

            for (int i = 0; i < GetEnteringFrontierSize(); ++i) {
                int v = GetEnteringFrontierValue(i);
                frontier_array[v] = v;
                if (v == start_vertex_) {
                    frontier_array[v] = end_vertex_;
                    for (int i = 0; i < GetPreviousFrontierSize(); ++i) {
                        if (frontier_array[GetPreviousFrontierValue(i)] == start_vertex_) {
                            frontier_array[v] = GetPreviousFrontierValue(i);
                            break;
                        }
                    }
                } else if (v == end_vertex_) {
                    frontier_array[v] = start_vertex_;
                    for (int i = 0; i < GetPreviousFrontierSize(); ++i) {
                        if (frontier_array[GetPreviousFrontierValue(i)] == end_vertex_) {
                            frontier_array[v] = GetPreviousFrontierValue(i);
                            break;
                        }
                    }
                }
            }
        }
    }
}

//*************************************************************************************************
// MateSTPath

void MateSTPath::UpdateMate(State* state, int child_num)
{
    Edge edge = state->GetCurrentEdge();

    if (child_num == 1) { // Hi枝のとき
        int sm = frontier_array[edge.src];
        int dm = frontier_array[edge.dest];

        // 辺をつないだときの mate の更新
        // （↓の計算順を変更すると正しく動作しないことに注意）
        frontier_array[edge.src] = 0;
        frontier_array[edge.dest] = 0;
        frontier_array[sm] = dm;
        frontier_array[dm] = sm;
    }
}

// mate update 前の終端判定。
// 0終端なら 0, 1終端なら 1, どちらでもない場合は -1 を返す。
int MateSTPath::CheckTerminalPre(State* state, int child_num)
{
    if (child_num == 0) { // Lo枝のとき
        return -1;
    } else { // Hi枝のとき
        StateSTPath* st = static_cast<StateSTPath*>(state);
        Edge edge = state->GetCurrentEdge();

        if (frontier_array[edge.src] == 0 || frontier_array[edge.dest] == 0) {
            // 分岐が発生
            return 0;
        } else if (frontier_array[edge.src] == edge.dest) {
            // サイクルが完成

            if (!st->STEnteringFrontier()) { // s or t がまだフロンティアに入っていない
                return 0;
            }

            // フロンティアに属する残りの頂点についてチェック
            for (int i = 0; i < st->GetNextFrontierSize(); ++i) {
                mate_t v = st->GetNextFrontierValue(i);
                // 張った辺の始点と終点、及びs,tはチェックから除外
                if (v != edge.src && v != edge.dest) {
                    if (st->IsHamilton()) { // ハミルトンパス、サイクルの場合
                        if (frontier_array[v] != 0) {
                            return 0;
                        }
                    } else {
                        // パスの途中でなく、孤立した点でもない場合、
                        // 不完全なパスができることになるので、0を返す
                        if (frontier_array[v] != 0 && frontier_array[v] != v) { // v の次数が 1
                            return 0;
                        }
                    }
                }
            }
            if (st->IsHamilton()) { // ハミルトンパス、サイクルの場合
                if (st->IsExistUnprocessedVertex()) {
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
int MateSTPath::CheckTerminalPost(State* state)
{
    StateSTPath* st = static_cast<StateSTPath*>(state);

    for (int i = 0; i < st->GetLeavingFrontierSize(); ++i) {
        mate_t v = st->GetLeavingFrontierValue(i); // フロンティアから抜ける頂点 v
        if (st->IsHamilton()) { // ハミルトンパス、サイクルの場合
            if (frontier_array[v] != 0) { // v の次数が 0 or 1
                return 0;
            }
        } else {
            if (frontier_array[v] != 0 && frontier_array[v] != v) { // v の次数が 1
                return 0;
            }
        }
    }
    if (st->IsLastEdge()) {
        return 0;
    } else {
        return -1;
    }
}


// old function

#if 0

int MateSTPath::CheckTerminalPreOld(State* state, int child_num)
{
    if (child_num == 0) { // Lo枝の処理のときはチェックの必要がない
        return -1;
    } else {
        StateSTPath* st = static_cast<StateSTPath*>(state);
        Edge edge = state->GetCurrentEdge();
        int sv = st->GetStartVertex();
        int ev = st->GetEndVertex();

        if (frontier_array[edge.src] == 0 || frontier_array[edge.dest] == 0) {
            // 分岐が発生
            return 0;
        } else if ((edge.src == sv || edge.dest == sv) && frontier_array[sv] != sv) {
            // s の次数が1
            return 0;
        } else if ((edge.src == ev || edge.dest == ev) && frontier_array[ev] != ev) {
            // t の次数が1
            return 0;
        } else if (frontier_array[edge.src] == edge.dest) {
            // サイクルが完成
            return 0;
        } else if ((frontier_array[edge.src] == sv && frontier_array[edge.dest] == ev)
                   || (frontier_array[edge.src] == ev && frontier_array[edge.dest] == sv)) {
            // s-t パスが完成

            // フロンティアに属する残りの頂点についてチェック
            for (int i = 0; i < state->GetNextFrontierSize(); ++i) {
                mate_t v = state->GetNextFrontierValue(i);
                // 張った辺の始点と終点、及びs,tはチェックから除外
                if (v != edge.src && v != edge.dest && v != sv && v != ev) {
                    if (st->IsHamilton()) { // ハミルトンパス、サイクルの場合
                        if (frontier_array[v] != 0) {
                            return 0;
                        }
                    } else {
                        // パスの途中でなく、孤立した点でもない場合、
                        // 不完全なパスができることになるので、0を返す
                        if (frontier_array[v] != 0 && frontier_array[v] != v) {
                            return 0;
                        }
                    }
                }
            }
            if (st->IsHamilton()) { // ハミルトンパス、サイクルの場合
                if (st->IsExistUnprocessedVertex()) {
                    return 0;
                }
            }
            return 1;
        } else {
            return -1;
        }
    }
}

int MateSTPath::CheckTerminalPostOld(State* state)
{
    StateSTPath* st = static_cast<StateSTPath*>(state);

    for (int i = 0; i < state->GetLeavingFrontierSize(); ++i) {
        mate_t v = state->GetLeavingFrontierValue(i);
        if (st->IsHamilton()) { // ハミルトンパス、サイクルの場合
            if (frontier_array[v] != 0) {
                return 0;
            }
        } else {
            int sv = st->GetStartVertex();
            int ev = st->GetEndVertex();
            if (v == sv || v == ev) {
                if (frontier_array[v] == v) { // v が始点または終点で v の次数が0
                    return 0;
                }
            } else {
                if (frontier_array[v] != 0 && frontier_array[v] != v) { // v の次数が1
                    return 0;
                }
            }
        }
    }
    if (state->IsLastEdge()) {
        return 0;
    } else {
        return -1;
    }
}

#endif

} // the end of the namespace
