//
// StateHyper.hpp
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

#ifndef STATEHYPER_HPP
#define STATEHYPER_HPP

#include <vector>
#include <set>
#include <algorithm>
#include <string>
#include <iostream>
#include <cassert>
#include <cstdio>

#include "Global.hpp"
#include "HyperGraph.hpp"
#include "State.hpp"

namespace frontier_lib {

//*************************************************************************************************
// State: アルゴリズム動作時の「状態」を表すクラス
class StateHyper : public State {
protected:
    const HyperGraph* const hgraph_;

public:
    StateHyper(HyperGraph* hgraph) : State(hgraph->GetNumberOfVertices(),
                                           hgraph->GetNumberOfEdges()), hgraph_(hgraph) { }

    virtual ~StateHyper() { }

    const HyperEdge& GetCurrentHyperEdge() const
    {
        return hgraph_->GetHyperEdge(current_edge_);
    }

    virtual bool IsUsingVertexWeight() const
    {
        return hgraph_->IsUsingVertexWeight();
    }
};


} // the end of the namespace

#endif // STATEHYPER_HPP
