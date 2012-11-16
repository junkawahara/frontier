//
// MateSTree.cpp
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

#include "MateSTree.hpp"

namespace frontier_dd {

using namespace std;

//*************************************************************************************************
// StateSTree
StateSTree::StateSTree(Graph* graph) : StateSForest(graph)
{
    // delete the instance of MateForestBase created in the constructor of StateForestBase
    delete global_mate_;

    global_mate_ = new MateSTree(this);
}

StateSTree::~StateSTree()
{
    delete global_mate_;

    // for destructor of StateForestBase
    global_mate_ = NULL;
}

//*************************************************************************************************
// MateSTree
MateSTree::MateSTree(State* state) : MateSForest(state)
{
    // nothing
}

// mate update 後の終端判定。
// 0終端なら 0, 1終端なら 1, どちらでもない場合は -1 を返す。
int MateSTree::CheckTerminalPost(State* state)
{
    StateFrontier* st = static_cast<StateFrontier*>(state);

    if (st->IsLastEdge()) {
        Edge edge = st->GetCurrentEdge();
        if (mate_[edge.src] == mate_[edge.dest]) {
            return 1;
        } else {
            return 0;
        }
    } else {
        for (int i = 0; i < st->GetLeavingFrontierSize(); ++i) {
            mate_t v = st->GetLeavingFrontierValue(i); // フロンティアから抜ける頂点
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
        return -1;
    }
}

} // the end of the namespace
