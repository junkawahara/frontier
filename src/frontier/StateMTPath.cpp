//
// StateMTPath.cpp
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

#include <cstring>
#include <cassert>
#include <iostream>

#include "StateMTPath.hpp"

namespace frontier_lib {

using namespace std;

// mate update 前の終端判定。
// 0終端なら 0, 1終端なら 1, どちらでもない場合は -1 を返す。
int StateMTPath::CheckTerminalPre(MateMTPath* mate, int child_num)
{
    if (child_num == 0) { // Lo枝のとき
        return -1;
    } else { // Hi枝のとき
        Edge edge = GetCurrentEdge();

        if (mate->frontier[edge.src] == 0 || mate->frontier[edge.dest] == 0) {
            // 分岐が発生
            return 0;
        } else if (mate->frontier[edge.src] == edge.dest) {
            // サイクルが完成
            return 0;
        } else {
            const TerminalManager& tm = GetTerminalManager();
            if (tm.Exists(edge.src)) { // src がヒント頂点
                if (mate->frontier[edge.src] != edge.src) { // src の次数が 1 以上
                    return 0;
                }
            }
            if (tm.Exists(edge.dest)) { // dest がヒント頂点
                if (mate->frontier[edge.dest] != edge.dest) { // dest の次数が 1 以上
                    return 0;
                }
            }
            // src と dest がともにヒント頂点
            if (tm.Exists(mate->frontier[edge.src]) && tm.Exists(mate->frontier[edge.dest])) {
                // (src, dest) がヒントペアではない（すなわち src と dest のヒント数字が異なる）
                if (!tm.ExistsPair(mate->frontier[edge.src], mate->frontier[edge.dest])) {
                    return 0;
                }
            }
            return -1;
        }
    }
}

// mate update 後の終端判定。
// 0終端なら 0, 1終端なら 1, どちらでもない場合は -1 を返す。
int StateMTPath::CheckTerminalPost(MateMTPath* mate)
{
    for (int i = 0; i < frontier_manager_.GetLeavingFrontierSize(); ++i) {
        mate_t v = frontier_manager_.GetLeavingFrontierValue(i);
        if (IsHamilton()) { // ハミルトンパスの場合
            if (mate->frontier[v] == v) { // v の次数が 0
                return 0;
            // v がヒント頂点ではなく、かつ v の次数が 1
            } else if (!GetTerminalManager().Exists(v) && mate->frontier[v] != 0) {
                return 0;
            }
        } else {
            if (GetTerminalManager().Exists(v)) { // v がヒント頂点
                if (mate->frontier[v] == v) { // v の次数が 0
                    return 0;
                }
            } else {
                if (mate->frontier[v] != v && mate->frontier[v] != 0) { // v の次数が 1
                    return 0;
                }
            }
        }
    }

    if (IsLastEdge()) {
        return 1;
    } else {
        return -1;
    }
}

} // the end of the namespace
