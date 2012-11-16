//
// MateRcut.hpp
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

#ifndef MATERCUT_HPP
#define MATERCUT_HPP

#include "MateSForest.hpp"
#include "RootManager.hpp"

namespace frontier_dd {

//*************************************************************************************************
// MateConfRcut
struct MateConfRcut {
    double number_of_cuts;
};

//*************************************************************************************************
// StateRcut: 根を指定したカットのための State
class StateRcut : public StateForestBase<MateConfRcut> {
private:
    RootManager* root_mgr_;
    double max_cut_size_;
public:
    StateRcut(Graph* graph, double max_cut_size);
    virtual ~StateRcut();

    const RootManager* GetRootManager() const
    {
        return root_mgr_;
    }

    void SetRootManager(RootManager* root_mgr)
    {
        root_mgr_ = root_mgr;
    }

    double GetMaxCutSize() const
    {
        return max_cut_size_;
    }

    virtual Mate* UnpackMate(ZDDNode* node, int child_num);
};

//*************************************************************************************************
// MateRcut: mate を表すクラス
class MateRcut : public MateForestBase<MateConfRcut> {
public:
    MateRcut(State* state);

    virtual void Update(State* state, int child_num);
    virtual int CheckTerminalPre(State* state, int child_num);
    virtual int CheckTerminalPost(State* state);
};

} // the end of the namespace

#endif // MATERCUT_HPP
