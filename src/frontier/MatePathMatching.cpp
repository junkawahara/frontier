//
// MatePathMatching.cpp
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

#include "MatePathMatching.hpp"

namespace frontier_dd {

using namespace std;

//*************************************************************************************************
// StatePathMatching
StatePathMatching::StatePathMatching(Graph* graph) : StateFrontier(graph)
{
    global_mate_ = new MatePathMatching(this);
}

StatePathMatching::~StatePathMatching()
{
    delete global_mate_;
}

void StatePathMatching::PackMate(ZDDNode* node, Mate* mate)
{
    MatePathMatching* mt = static_cast<MatePathMatching*>(mate);

    StateFrontier::Pack(node, mt->GetMateArray());
}

Mate* StatePathMatching::UnpackMate(ZDDNode* node, int child_num)
{
    if (child_num == 0) { // Hi枝の場合は既にunpackされているので、無視する
        mate_t* mate = global_mate_->GetMateArray();
        StateFrontier::UnpackAndSeek(node, mate);
    }

    return global_mate_;
}

//*************************************************************************************************
// MatePathMatching
MatePathMatching::MatePathMatching(State* state)
{
    mate_ = new mate_t[state->GetNumberOfVertices() + 1];
}

MatePathMatching::~MatePathMatching()
{
    if (mate_ != NULL) {
        delete[] mate_;
    }
}

void MatePathMatching::Update(State* state, int child_num)
{
    Edge edge = state->GetCurrentEdge();

    if (child_num == 1) { // Hi枝のとき
        int sm = mate_[edge.src];
        int dm = mate_[edge.dest];

        // 辺をつないだときの mate の更新
        // （↓の計算順を変更すると正しく動作しないことに注意）
        mate_[edge.src] = 0;
        mate_[edge.dest] = 0;
        mate_[sm] = dm;
        mate_[dm] = sm;
    }
}

// mate update 前の終端判定。
// 0終端なら 0, 1終端なら 1, どちらでもない場合は -1 を返す。
int MatePathMatching::CheckTerminalPre(State* state, int child_num)
{
    if (child_num == 0) { // Lo枝とき
        return -1;
    } else { // Hi枝とき
        Edge edge = state->GetCurrentEdge();

        if (mate_[edge.src] == 0 || mate_[edge.dest] == 0) {
            // 分岐が発生
            return 0;
        } else if (mate_[edge.src] == edge.dest) {
            // サイクルが完成
            return 0;
        } else {
            return -1;
        }
    }
}

// mate update 後の終端判定。
// 0終端なら 0, 1終端なら 1, どちらでもない場合は -1 を返す。
int MatePathMatching::CheckTerminalPost(State* state)
{
    StatePathMatching* st = static_cast<StatePathMatching*>(state);

    if (st->IsHamilton()) { // ハミルトンパスの場合
        for (int i = 0; i < st->GetLeavingFrontierSize(); ++i) {
            mate_t v = st->GetLeavingFrontierValue(i); // フロンティアから抜ける頂点
            if (mate_[v] == v) { // v の次数が 0
                return 0;
            }
        }
    }

    if (st->IsLastEdge()) {
        return 1;
    } else {
        return -1;
    }
}

} // the end of the namespace
