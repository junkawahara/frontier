//
// MateSetPartition.cpp
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

#include "MateSetPartition.hpp"

namespace frontier_dd {

using namespace std;

const int MAX_WIDTH = 1000000;

//*************************************************************************************************
// StateSetPartition

StateSetPartition::StateSetPartition(HyperGraph* graph) : StateFrontierHyper(graph)
{
    global_mate_ = new MateSetPartition(this);
    current_edge_bits_ = new uintx[MAX_WIDTH];
    leaving_frontier_bits_ = new uintx[MAX_WIDTH];
}

StateSetPartition::~StateSetPartition()
{
    delete[] leaving_frontier_bits_;
    delete[] current_edge_bits_;
    delete global_mate_;
}

void StateSetPartition::PackMate(ZDDNode* node, Mate* mate)
{
    MateSetPartition* mt = static_cast<MateSetPartition*>(mate);

    StateFrontierHyper::Pack(node, mt->GetMateArray());
}

Mate* StateSetPartition::UnpackMate(ZDDNode* node, int child_num)
{
    if (child_num == 0) { // Hi枝の場合は既にunpackされているので、無視する
        StateFrontierHyper::Unpack(node, global_mate_->GetMateArray());
    }

    return global_mate_;
}

//*************************************************************************************************
// MateSetPartition

MateSetPartition::MateSetPartition(State* )
{
    mate_ = new uintx[MAX_WIDTH];
}

MateSetPartition::~MateSetPartition()
{
    if (mate_ != NULL) {
        delete[] mate_;
    }
}

void MateSetPartition::Update(State* state, int child_num)
{
    StateSetPartition* st = static_cast<StateSetPartition*>(state);

    if (child_num == 1) { // Hi枝のとき
        for (int i = 0; i < st->GetCurrentFrontierWidthWord(); ++i) {
            mate_[i] |= st->GetCurrentEdgeBits(i);
        }
    }
}

// mate update 前の終端判定。
// 0終端なら 0, 1終端なら 1, どちらでもない場合は -1 を返す。
int MateSetPartition::CheckTerminalPre(State* state, int child_num)
{
    StateSetPartition* st = static_cast<StateSetPartition*>(state);

    if (child_num == 1) { // Hi枝のとき
        for (int i = 0; i < st->GetCurrentFrontierWidthWord(); ++i) {
            if ((mate_[i] & st->GetCurrentEdgeBits(i)) != 0) {
                return 0;
            }
        }
    }

    for (int i = 0; i < st->GetCurrentFrontierWidthWord(); ++i) {
        if (child_num == 0) {
            if ((~mate_[i] & st->GetLeavingFrontierBits(i)) != 0) {
                return 0;
            }
        } else {
            if ((~(mate_[i] | st->GetCurrentEdgeBits(i)) & st->GetLeavingFrontierBits(i)) != 0) {
                return 0;
            }
        }
    }

    return -1;
}

// mate update 後の終端判定。
// 0終端なら 0, 1終端なら 1, どちらでもない場合は -1 を返す。
int MateSetPartition::CheckTerminalPost(State* state)
{
    if (state->IsLastEdge()) {
        return 1;
    } else {
        return -1;
    }
}

} // the end of the namespace
