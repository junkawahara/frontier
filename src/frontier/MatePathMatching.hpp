//
// MatePathMatching.hpp
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

#ifndef MATEPATHMATCHING_HPP
#define MATEPATHMATCHING_HPP

#include "StateFrontier.hpp"
#include "PseudoZDD.hpp"

namespace frontier_dd {

class MatePathMatching;

//*************************************************************************************************
// StatePathMatching: アルゴリズム動作時の「状態」を表すクラス
class StatePathMatching : public StateFrontier {
protected:
    bool is_hamilton_;

    MatePathMatching* global_mate_;

public:
    StatePathMatching(Graph* graph);
    virtual ~StatePathMatching();

    bool IsHamilton() const
    {
        return is_hamilton_;
    }

    void SetHamilton(bool is_hamilton)
    {
        is_hamilton_ = is_hamilton;
    }

    virtual void PackMate(ZDDNode* node, Mate* mate);
    virtual Mate* UnpackMate(ZDDNode* node, int child_num);
};

//*************************************************************************************************
// MatePathMatching: mate を表すクラス
class MatePathMatching : public Mate {
protected:
    mate_t* mate_;
public:
    MatePathMatching(State* state);
    virtual ~MatePathMatching();

    mate_t* GetMateArray()
    {
        return mate_;
    }

    virtual void Update(State* state, int child_num);
    virtual int CheckTerminalPre(State* state, int child_num);
    virtual int CheckTerminalPost(State* state);
};

} // the end of the namespace

#endif // MATEPATHMATCHING_HPP
