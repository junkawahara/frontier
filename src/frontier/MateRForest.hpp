//
// MateRForest.hpp
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

#ifndef MATERFOREST_HPP
#define MATERFOREST_HPP

#include "MateSForest.hpp"
#include "RootManager.hpp"

namespace frontier_dd {

//*************************************************************************************************
// StateRForest: 根付き全域木のための State
class StateRForest : public StateSForest {
private:
    RootManager* root_mgr_;
public:
    StateRForest(Graph* graph);
    virtual ~StateRForest();

    const RootManager& GetRootManager() const
    {
        return *root_mgr_;
    }

    void SetRootManager(RootManager* root_mgr)
    {
        root_mgr_ = root_mgr;
    }

    virtual Mate* UnpackMate(ZDDNode* /*node*/, int child_num);
};

//*************************************************************************************************
// MateRForest
class MateRForest : public MateSForest {
public:
    MateRForest(State* state);
    virtual int CheckTerminalPre(State* state, int child_num);
    virtual int CheckTerminalPost(State* state);
};

} // the end of the namespace

#endif // MATERFOREST_HPP
