//
// StateRForest.cpp
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

#include "StateRForest.hpp"

namespace frontier_lib {

using namespace std;

void StateRForest::UnpackMate(ZDDNode* node, Mate* mate, int child_num)
{
    StateFrontierComp<MateRForest>::UnpackMate(node, mate, child_num);

    MateRForest* m = static_cast<MateRForest*>(mate);

    for (int i = 0; i < frontier_manager_.GetEnteringFrontierSize(); ++i) {
        int v = frontier_manager_.GetEnteringFrontierValue(i);
        if (root_mgr_->Exists(v)) {
            m->frontier[v].comp = -(v + GetNumberOfVertices());
        } else {
            m->frontier[v].comp = v + GetNumberOfVertices();
        }
    }
}

// mate update 前の終端判定。
// 0終端なら 0, 1終端なら 1, どちらでもない場合は -1 を返す。
int StateRForest::CheckTerminalPre(MateRForest* mate, int child_num)
{
    if (child_num == 0) { // Lo枝のとき
        return -1;
    } else { // Hi枝のとき
        Edge edge = GetCurrentEdge();
        if (mate->frontier[edge.src].comp == mate->frontier[edge.dest].comp) { // cycle generated
            return 0;
        } else if (mate->frontier[edge.src].comp < 0
                   && mate->frontier[edge.dest].comp < 0) { // 異なる根をもつ2つの森が接続される
            return 0;
        } else {
            return -1;
        }
    }
}

// mate update 後の終端判定。
// 0終端なら 0, 1終端なら 1, どちらでもない場合は -1 を返す。
int StateRForest::CheckTerminalPost(MateRForest* mate)
{
    if (IsLastEdge()) {
        Edge edge = GetCurrentEdge();
         // src か dest の少なくとも一方がどの根付き森にも含まれていない
        if (mate->frontier[edge.src].comp > 0 || mate->frontier[edge.dest].comp > 0) {
            return 0;
        } else {
            return 1;
        }
    } else {
        for (int i = 0; i < frontier_manager_.GetLeavingFrontierSize(); ++i) {
            mate_t v = frontier_manager_.GetLeavingFrontierValue(i); // フロンティアから抜ける頂点
            // v がどの根付き森にも含まれていない
            if (mate->frontier[v].comp > 0) {
                bool is_exist = false;
                for (int j = 0; j < frontier_manager_.GetNextFrontierSize(); ++j) {
                    mate_t w = frontier_manager_.GetNextFrontierValue(j);
                    if (mate->frontier[v].comp == mate->frontier[w].comp) {
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
