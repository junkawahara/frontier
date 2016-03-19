//
// StateSetPartition.cpp
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

#include "StateSetPartition.hpp"

namespace frontier_lib {

using namespace std;

//*************************************************************************************************
// StateSetPartition

void StateSetPartition::UnpackMate(ZDDNode* node, Mate* mate, int child_num)
{
    StateFrontierHyper<MateSetPartition>::UnpackMate(node, mate, child_num);

    MateSetPartition* m = static_cast<MateSetPartition*>(mate);

    for (int i = 0; i < frontier_manager_.GetEnteringFrontierSize(); ++i) {
        int v = frontier_manager_.GetEnteringFrontierValue(i);
        m->frontier[v] = 0;
    }
}

void StateSetPartition::UpdateMate(MateSetPartition* mate, int child_num)
{
    if (child_num == 1) { // Hi枝のとき
        const HyperEdge& edge = GetCurrentHyperEdge();
        for (uint i = 0; i < edge.var_array.size(); ++i) {
            mate->frontier[edge.var_array[i]] = 1;
        }
    }
}

// mate update 前の終端判定。
// 0終端なら 0, 1終端なら 1, どちらでもない場合は -1 を返す。
int StateSetPartition::CheckTerminalPre(MateSetPartition* mate, int child_num)
{
    if (child_num == 1) { // Hi枝のとき
        const HyperEdge& edge = GetCurrentHyperEdge();
        for (uint i = 0; i < edge.var_array.size(); ++i) {
            if (mate->frontier[edge.var_array[i]] != 0) {
                return 0;
            }
        }
    }
    return -1;
}

// mate update 後の終端判定。
// 0終端なら 0, 1終端なら 1, どちらでもない場合は -1 を返す。
int StateSetPartition::CheckTerminalPost(MateSetPartition* mate)
{
    for (int i = 0; i < frontier_manager_.GetLeavingFrontierSize(); ++i) {
        int v = frontier_manager_.GetLeavingFrontierValue(i);
        if (mate->frontier[v] == 0) {
            return 0;
        }
    }

    if (IsLastEdge()) {
        return 1;
    } else {
        return -1;
    }
}

} // the end of the namespace
