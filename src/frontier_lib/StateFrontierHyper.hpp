//
// StateFrontier.hpp
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

#ifndef STATEFRONTIERHYPER_HPP
#define STATEFRONTIERHYPER_HPP

#include <vector>
#include <set>
#include <algorithm>
#include <string>
#include <iostream>
#include <cassert>
#include <cstdio>

#include "Global.hpp"
#include "HyperGraph.hpp"
#include "ZDDNode.hpp"
#include "RBuffer.hpp"
#include "FrontierManager.hpp"
#include "StateHyper.hpp"
#include "Mate.hpp"
#include "PseudoZDD.hpp"


namespace frontier_lib {

class Mate;

//*************************************************************************************************
// StateFrontierHyper:

template<typename MT>
class StateFrontierHyper : public StateHyper {
protected:
    FrontierManager frontier_manager_;

public:
    StateFrontierHyper(HyperGraph* hgraph) : StateHyper(hgraph), frontier_manager_(hgraph) { }

    virtual void StartNextEdge()
    {
        StateHyper::StartNextEdge();
        frontier_manager_.Update(GetCurrentHyperEdge(), GetCurrentEdgeNumber());
    }

    virtual Mate* Initialize(ZDDNode* /*root_node*/)
    {
        return new MT(this);
    }

    virtual bool Equals(const ZDDNode& node1, const ZDDNode& node2, Mate* mate) const
    {
        return mate->Equals(node1, node2, frontier_manager_);
    }

    virtual intx GetHashValue(const ZDDNode& node, Mate* mate) const
    {
        return mate->GetHashValue(node, frontier_manager_);
    }

    virtual void PackMate(ZDDNode* node, Mate* mate)
    {
        mate->PackMate(node, frontier_manager_);
    }

    virtual void UnpackMate(ZDDNode* node, Mate* mate, int child_num)
    {
        mate->UnpackMate(node, child_num, frontier_manager_);
    }

    virtual void Revert(Mate* mate)
    {
        mate->Revert(frontier_manager_);
    }

    virtual ZDDNode* MakeNewNode(ZDDNode* /*node*/, Mate* mate,
                                 int child_num, PseudoZDD* zdd)
    {
        MT* m = static_cast<MT*>(mate);

        if (m->IsUseSubsetting()) {
            if (DoSubsetting(child_num, m) == 0) {
                return zdd->ZeroTerminal;
            }
        }

        int c = this->CheckTerminalPre(m, child_num); // 終端に遷移するか事前にチェック
        if (c == 0) { // 0終端に行くとき
            return zdd->ZeroTerminal; // 0終端を返す
        } else if (c == 1) { // 1終端に行くとき
            return zdd->OneTerminal; // 1終端を返す
        }

        this->UpdateMate(m, child_num); // mate を更新する

        c = this->CheckTerminalPost(m); // 終端に遷移するか再度チェック
        if (c == 0) { // 0終端に行くとき
            return zdd->ZeroTerminal; // 0終端を返す
        } else if (c == 1) { // 1終端に行くとき
            return zdd->OneTerminal; // 1終端を返す
        } else {
            ZDDNode* child_node = zdd->CreateNode();
            return child_node;
        }
    }

protected:
    virtual void UpdateMate(MT* mate, int child_num) = 0;
    virtual int CheckTerminalPre(MT* mate, int child_num) = 0;
    virtual int CheckTerminalPost(MT* mate) = 0;
};


} // the end of the namespace

#endif // STATEFRONTIERHYPER_HPP
