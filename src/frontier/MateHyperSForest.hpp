//
// MateHyperSForest.hpp
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

#ifndef MATEHYPERSFOREST_HPP
#define MATEHYPERSFOREST_HPP

#include "StateFrontierHyper.hpp"
#include "PseudoZDD.hpp"
#include "CompManager.hpp"

namespace frontier_dd {

class MateHyperSForest;

//*************************************************************************************************
// StateHyperSForest:
class StateHyperSForest : public StateFrontierHyper<FrontierComp> {
protected:

public:
    StateHyperSForest(HyperGraph* hgraph) : StateFrontierHyper<FrontierComp>(hgraph) { }

    virtual ~StateHyperSForest() { }

    virtual void Load(Mate* mate, byte* data);
    virtual void Store(Mate* mate, byte* data);

    virtual Mate* MakeEmptyMate();
    //virtual void PackMate(ZDDNode* node, Mate* mate);
    virtual void UnpackMate(ZDDNode* node, Mate* mate, int child_num);
};

//*************************************************************************************************
// MateHyperSForest
class MateHyperSForest : public MateFrontierHyper<FrontierComp> {
public:
    CompManagerHyper<FrontierComp> comp_manager_;

public:
    MateHyperSForest(State* state) : MateFrontierHyper<FrontierComp>(state),
                                     comp_manager_(frontier_array, state->GetNumberOfVertices())
    {
        // nothing
    }

    virtual void UpdateMate(State* state, int child_num);
    virtual int CheckTerminalPre(State* state, int child_num);
    virtual int CheckTerminalPost(State* state);
    virtual void Rename(State* state);

    virtual std::string GetPreviousString(State* state);
    virtual std::string GetNextString(State* state);
};

} // the end of the namespace

#endif // MATEHYPERSForest_HPP
