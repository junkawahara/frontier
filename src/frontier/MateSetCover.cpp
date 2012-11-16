//
// MateSetCover.cpp
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

#include "MateSetCover.hpp"

namespace frontier_dd {

using namespace std;

//*************************************************************************************************
// StateSetCover

StateSetCover::StateSetCover(HyperGraph* graph) : StateSetPartition(graph)
{
    // delete the instance of MateSetPartition created in the constructor of StateSetPartition
    delete global_mate_;

    global_mate_ = new MateSetCover(this);
}

StateSetCover::~StateSetCover()
{
    delete global_mate_;

    // for destructor of StateSetPartition
    global_mate_ = NULL;
}

//*************************************************************************************************
// MateSetCover

// mate update 前の終端判定。
// 0終端なら 0, 1終端なら 1, どちらでもない場合は -1 を返す。
int MateSetCover::CheckTerminalPre(State* state, int child_num)
{
    StateSetCover* st = static_cast<StateSetCover*>(state);

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

} // the end of the namespace
