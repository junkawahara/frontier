//
// StateSForest.cpp
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
#include <algorithm>
#include <sstream>

#include "StateSForest.hpp"
#include "../frontier_lib/PseudoZDD.hpp"

namespace frontier_lib {

using namespace std;

//*************************************************************************************************
// StateSForest

void StateSForest::UpdateMate(MateSForest* mate, int child_num)
{
    if (child_num == 1) { // Hi枝のとき
        ConnectComponents(mate);
    }
}

// mate update 前の終端判定。
// 0終端なら 0, 1終端なら 1, どちらでもない場合は -1 を返す。
int StateSForest::CheckTerminalPre(MateSForest* mate, int child_num)
{
    if (child_num == 0) { // Lo枝のとき
        return -1;
    } else {
        Edge edge = GetCurrentEdge();
        if (mate->frontier[edge.src].comp == mate->frontier[edge.dest].comp) { // cycle generated
            return 0;
        } else {
            return -1;
        }
    }
}

// mate update 後の終端判定。
// 0終端なら 0, 1終端なら 1, どちらでもない場合は -1 を返す。
int StateSForest::CheckTerminalPost(MateSForest* /*mate*/)
{
    if (IsLastEdge()) {
        return 1;
    } else {
        return -1;
    }
}

} // the end of the namespace
