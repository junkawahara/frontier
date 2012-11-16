//
// MateMTPath.cpp
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
#include <iostream>

#include "MateMTPath.hpp"

namespace frontier_dd {

using namespace std;

//*************************************************************************************************
// TerminalManager

void TerminalManager::Parse(const string& filename)
{
    int x, y;
    ifstream ifs(filename.c_str());
    while (ifs >> x) {
        if (!(ifs >> y)) {
            cerr << "File format error in TerminalManager" << endl;
            exit(1);
        }
        if (x > y) {
            swap(x, y);
        }
        pair_array_.push_back(make_pair(x, y));
    }
}

bool TerminalManager::Exists(int v) const
{
    for (uint i = 0; i < pair_array_.size(); ++i)
    {
        if (pair_array_[i].first == v || pair_array_[i].second == v) {
            return true;
        }
    }
    return false;
}

bool TerminalManager::ExistsPair(int v1, int v2) const
{
    if (v1 > v2) {
        swap(v1, v2);
    }
    return find(pair_array_.begin(), pair_array_.end(), make_pair(v1, v2)) != pair_array_.end();
}

//*************************************************************************************************
// StateMTPath
StateMTPath::StateMTPath(Graph* graph) : StatePathMatching(graph)
{
    // delete the instance of MatePathMatching created in the constructor of StatePathMatching
    delete global_mate_;

    global_mate_ = new MateMTPath(this);
}

StateMTPath::~StateMTPath()
{
    delete global_mate_;

    // for destructor of StatePathMatching
    global_mate_ = NULL;
}

void StateMTPath::ParseTerminal(const std::string& filename)
{
    terminal_manager_.Parse(filename);
}

//*************************************************************************************************
// MateMTPath

// mate update 前の終端判定。
// 0終端なら 0, 1終端なら 1, どちらでもない場合は -1 を返す。
int MateMTPath::CheckTerminalPre(State* state, int child_num)
{
    if (child_num == 0) { // Lo枝のとき
        return -1;
    } else { // Hi枝のとき
        Edge edge = state->GetCurrentEdge();

        if (mate_[edge.src] == 0 || mate_[edge.dest] == 0) {
            // 分岐が発生
            return 0;
        } else if (mate_[edge.src] == edge.dest) {
            // サイクルが完成
            return 0;
        } else {
            const TerminalManager& tm = static_cast<StateMTPath*>(state)->GetTerminalManager();
            if (tm.Exists(edge.src)) { // src がヒント頂点
                if (mate_[edge.src] != edge.src) { // src の次数が 1 以上
                    return 0;
                }
            }
            if (tm.Exists(edge.dest)) { // dest がヒント頂点
                if (mate_[edge.dest] != edge.dest) { // dest の次数が 1 以上
                    return 0;
                }
            }
            // src と dest がともにヒント頂点
            if (tm.Exists(mate_[edge.src]) && tm.Exists(mate_[edge.dest])) {
                // (src, dest) がヒントペアではない（すなわち src と dest のヒント数字が異なる）
                if (!tm.ExistsPair(mate_[edge.src], mate_[edge.dest])) {
                    return 0;
                }
            }
            return -1;
        }
    }
}

// mate update 後の終端判定。
// 0終端なら 0, 1終端なら 1, どちらでもない場合は -1 を返す。
int MateMTPath::CheckTerminalPost(State* state)
{
    StateMTPath* st = static_cast<StateMTPath*>(state);

    for (int i = 0; i < st->GetLeavingFrontierSize(); ++i) {
        mate_t v = st->GetLeavingFrontierValue(i);
        if (st->IsHamilton()) { // ハミルトンパスの場合
            if (mate_[v] == v) { // v の次数が 0
                return 0;
            // v がヒント頂点ではなく、かつ v の次数が 1
            } else if (!st->GetTerminalManager().Exists(v) && mate_[v] != 0) {
                return 0;
            }
        } else {
            if (st->GetTerminalManager().Exists(v)) { // v がヒント頂点
                if (mate_[v] == v) { // v の次数が 0
                    return 0;
                }
            } else {
                if (mate_[v] != v && mate_[v] != 0) { // v の次数が 1
                    return 0;
                }
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
