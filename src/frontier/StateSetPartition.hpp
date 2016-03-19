//
// StateSetPartition.hpp
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

#ifndef STATESETPARTITION_HPP
#define STATESETPARTITION_HPP

#include <vector>

#include "../frontier_lib/StateFrontierHyper.hpp"
#include "../frontier_lib/PseudoZDD.hpp"

namespace frontier_lib {

//*************************************************************************************************
// StateSetPartition: 集合分割のための State
class StateSetPartition : public StateFrontierHyper<MateF<byte> > {
protected:
    typedef MateF<byte> MateSetPartition;

public:
    StateSetPartition(HyperGraph* graph) : StateFrontierHyper<MateSetPartition>(graph) { }
    virtual ~StateSetPartition() { }

    virtual void UnpackMate(ZDDNode* node, Mate* mate, int child_num);

protected:
    virtual void UpdateMate(MateSetPartition* mate, int child_num);
    virtual int CheckTerminalPre(MateSetPartition* mate, int child_num);
    virtual int CheckTerminalPost(MateSetPartition* mate);
};

} // the end of the namespace

#endif // STATESETPARTITION_HPP
