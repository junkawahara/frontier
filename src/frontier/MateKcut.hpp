//
// MateKcut.hpp
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

#ifndef MATEKCUT_HPP
#define MATEKCUT_HPP

#include "MateSForest.hpp"

namespace frontier_dd {

struct MateConfKcut {
    int number_of_components;
    double number_of_cuts;
};

//*************************************************************************************************
// StateKcut: 高々 k 本の辺からなるカットのための State
class StateKcut : public StateForestBase<MateConfKcut> {
private:
    double max_cut_size_;
public:
    StateKcut(Graph* graph, double max_cut_size);
    ~StateKcut();

    double GetMaxCutSize() const
    {
        return max_cut_size_;
    }

    virtual Mate* UnpackMate(ZDDNode* node, int child_num);
};

//*************************************************************************************************
// MateKcut
class MateKcut : public MateForestBase<MateConfKcut> {
public:
    MateKcut(State* state);

    void Update(State* state, int child_num);
    int CheckTerminalPre(State* state, int child_num);
    int CheckTerminalPost(State* state);
};

} // the end of the namespace

#endif // MATEKCUT_HPP
