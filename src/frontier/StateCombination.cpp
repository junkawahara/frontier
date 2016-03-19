//
// MateCombination.cpp
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

#include "StateCombination.hpp"

namespace frontier_lib {

using namespace std;

//*************************************************************************************************
// StateCombination

void StateCombination::UpdateMate(MateCombination* mate, int child_num)
{
    if (child_num == 1) {
        mate->data += GetCurrentEdge().weight;
    }
}

// mate update 前の終端判定。
// 0終端なら 0, 1終端なら 1, どちらでもない場合は -1 を返す。
int StateCombination::CheckTerminalPre(MateCombination* mate, int child_num)
{
    if (child_num == 1) {
        if (mate->data + GetCurrentEdge().weight > number_of_edges_range_.second) {
            return 0;
        }
    }
    return -1;
}

// mate update 後の終端判定。
// 0終端なら 0, 1終端なら 1, どちらでもない場合は -1 を返す。
int StateCombination::CheckTerminalPost(MateCombination* mate)
{
    if (IsLastEdge()) {
        if (number_of_edges_range_.first <= mate->data &&
            mate->data <= number_of_edges_range_.second) {
            return 1;
        } else {
            return 0;
        }
    } else {
        return -1;
    }
}

} // the end of the namespace
