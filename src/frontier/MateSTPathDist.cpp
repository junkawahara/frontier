//
// MateSTPathDist.cpp
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

#include "MateSTPathDist.hpp"

namespace frontier_dd {

using namespace std;

//*************************************************************************************************
// StateSTPathDist
StateSTPathDist::StateSTPathDist(Graph* graph) : StateFrontierFixed<MateConfSTPathDist>(graph),
    max_distance_(99999999)
{
    global_mate_ = new MateSTPathDist(this);

    MateConfSTPathDist initial_conf;
    initial_conf.sum = 0;

    StateFrontierFixed<MateConfSTPathDist>::Initialize(initial_conf);
}

StateSTPathDist::~StateSTPathDist()
{
    delete global_mate_;
}

void StateSTPathDist::PackMate(ZDDNode* node, Mate* mate)
{
    MateSTPathDist* mt = static_cast<MateSTPathDist*>(mate);

    StateFrontierFixed<MateConfSTPathDist>::PackAll(node, mt->GetMateArray(), mt->GetConf());
}

Mate* StateSTPathDist::UnpackMate(ZDDNode* node, int child_num)
{
    mate_t* mate = global_mate_->GetMateArray();
    MateConfSTPathDist* conf = global_mate_->GetConf();

    if (child_num == 0) { // Hi枝の場合は既にunpackされているので、無視する
        StateFrontierFixed<MateConfSTPathDist>::UnpackAll(node, mate, conf);
        StateFrontierFixed<MateConfSTPathDist>::SeekTailFrontier();

        if (!IsCycle()) {
            for (int i = 0; i < GetEnteringFrontierSize(); ++i) {
                int v = GetEnteringFrontierValue(i);
                if (v == start_vertex_) {
                    mate[v] = end_vertex_;
                    for (int i = 0; i < GetPreviousFrontierSize(); ++i) {
                        if (mate[GetPreviousFrontierValue(i)] == start_vertex_) {
                            mate[v] = GetPreviousFrontierValue(i);
                            break;
                        }
                    }
                } else if (v == end_vertex_) {
                    mate[v] = start_vertex_;
                    for (int i = 0; i < GetPreviousFrontierSize(); ++i) {
                        if (mate[GetPreviousFrontierValue(i)] == end_vertex_) {
                            mate[v] = GetPreviousFrontierValue(i);
                            break;
                        }
                    }
                }
            }
        }
    } else {
        StateFrontierFixed<MateConfSTPathDist>::UnpackFixed(node, conf);
        StateFrontierFixed<MateConfSTPathDist>::SeekTailFixed();
    }

    return global_mate_;
}

//*************************************************************************************************
// MateSTPathDist

void MateSTPathDist::Update(State* state, int child_num)
{
    MateSTPath::Update(state, child_num);

    if (child_num == 1) {
        conf_.sum += state->GetCurrentEdge().weight;
    }
}

// mate update 前の終端判定。
// 0終端なら 0, 1終端なら 1, どちらでもない場合は -1 を返す。
int MateSTPathDist::CheckTerminalPre(State* state, int child_num)
{
    StateSTPathDist* st = static_cast<StateSTPathDist*>(state);

    if (child_num == 1) { // Hi枝のとき
        if (conf_.sum + state->GetCurrentEdge().weight > st->GetMaxDistance()) {
            return 0;
        }
    }

    if (child_num == 0) { // Lo枝のとき
        return -1;
    } else { // Hi枝のとき
        StateSTPathDist* st = static_cast<StateSTPathDist*>(state);
        Edge edge = state->GetCurrentEdge();

        if (mate_[edge.src] == 0 || mate_[edge.dest] == 0) {
            // 分岐が発生
            return 0;
        } else if (mate_[edge.src] == edge.dest) {
            // サイクルが完成

            // フロンティアに属する残りの頂点についてチェック
            for (int i = 0; i < st->GetNextFrontierSize(); ++i) {
                mate_t v = st->GetNextFrontierValue(i);
                // 張った辺の始点と終点、及びs,tはチェックから除外
                if (v != edge.src && v != edge.dest) {
                    if (st->IsHamilton()) { // ハミルトンパス、サイクルの場合
                        if (mate_[v] != 0) {
                            return 0;
                        }
                    } else {
                        // パスの途中でなく、孤立した点でもない場合、
                        // 不完全なパスができることになるので、0を返す
                        if (mate_[v] != 0 && mate_[v] != v) { // v の次数が 1
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
int MateSTPathDist::CheckTerminalPost(State* state)
{
    StateSTPathDist* st = static_cast<StateSTPathDist*>(state);

    for (int i = 0; i < st->GetLeavingFrontierSize(); ++i) {
        mate_t v = st->GetLeavingFrontierValue(i); // フロンティアから抜ける頂点 v
        if (st->IsHamilton()) { // ハミルトンパス、サイクルの場合
            if (mate_[v] != 0) { // v の次数が 0 or 1
                return 0;
            }
        } else {
            if (mate_[v] != 0 && mate_[v] != v) { // v の次数が1
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

} // the end of the namespace
