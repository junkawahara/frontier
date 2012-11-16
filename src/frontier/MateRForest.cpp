//
// MateRForest.cpp
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

#include "MateRForest.hpp"

namespace frontier_dd {

using namespace std;

//*************************************************************************************************
// StateRForest
StateRForest::StateRForest(Graph* graph) : StateSForest(graph)
{
    // delete the instance of MateForestBase created in the constructor of StateForestBase
    delete global_mate_;

    global_mate_ = new MateRForest(this);
}

StateRForest::~StateRForest()
{
    delete global_mate_;

    // for destructor of StateForestBase
    global_mate_ = NULL;
}

Mate* StateRForest::UnpackMate(ZDDNode* node, int child_num)
{
    if (child_num == 0) { // Hi枝の場合は既にunpackされているので、無視する
        StateFrontierFixed<MateConfNull>::UnpackAll(node, global_mate_->GetMateArray(), NULL);
        StateFrontierFixed<MateConfNull>::SeekTailAll();

        for (int i = 0; i < GetEnteringFrontierSize(); ++i) {
            int v = GetEnteringFrontierValue(i);
            if (root_mgr_->Exists(v)) {
                global_mate_->GetMateArray()[v] = -v;
            } else {
                global_mate_->GetMateArray()[v] = v;
            }
        }
    }

    return global_mate_;
}

//*************************************************************************************************
// MateRForest
MateRForest::MateRForest(State* state) : MateSForest(state)
{
    // nothing
}

// mate update 前の終端判定。
// 0終端なら 0, 1終端なら 1, どちらでもない場合は -1 を返す。
int MateRForest::CheckTerminalPre(State* state, int child_num)
{
    if (child_num == 0) { // Lo枝のとき
        return -1;
    } else { // Hi枝のとき
        Edge edge = state->GetCurrentEdge();
        if (mate_[edge.src] == mate_[edge.dest]) { // cycle generated
            return 0;
        } else if (mate_[edge.src] < 0
                && mate_[edge.dest] < 0) { // 異なる根をもつ2つの森が接続される
            return 0;
        } else {
            return -1;
        }
    }
}

// mate update 後の終端判定。
// 0終端なら 0, 1終端なら 1, どちらでもない場合は -1 を返す。
int MateRForest::CheckTerminalPost(State* state)
{
    StateFrontier* st = static_cast<StateFrontier*>(state);

    if (st->IsLastEdge()) {
        Edge edge = st->GetCurrentEdge();
         // src か dest の少なくとも一方がどの根付き森にも含まれていない
        if (mate_[edge.src] > 0 || mate_[edge.dest] > 0) {
            return 0;
        } else {
            return 1;
        }
    } else {
        for (int i = 0; i < st->GetLeavingFrontierSize(); ++i) {
            mate_t v = st->GetLeavingFrontierValue(i); // フロンティアから抜ける頂点
            // v がどの根付き森にも含まれていない
            if (mate_[v] > 0) {
                bool is_exist = false;
                for (int j = 0; j < st->GetNextFrontierSize(); ++j) {
                    if (mate_[v] == mate_[st->GetNextFrontierValue(j)]) {
                        is_exist = true;
                        break;
                    }
                }
                if (!is_exist) { // フロンティア上に値 mate_[v] をもつ頂点が存在しない
                    return 0;
                }
            }
        }
        return -1;
    }
}

} // the end of the namespace
