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

#ifndef STATEFRONTIER_HPP
#define STATEFRONTIER_HPP

#include <vector>
#include <set>
#include <algorithm>
#include <string>
#include <iostream>
#include <cassert>
#include <cstdio>

#include "Global.hpp"
#include "Graph.hpp"
#include "ZDDNode.hpp"
#include "RBuffer.hpp"
#include "FrontierManager.hpp"
#include "State.hpp"
#include "Mate.hpp"
#include "PseudoZDD.hpp"

namespace frontier_lib {

class Mate;


//*************************************************************************************************
// StateFrontier:

template<typename MT>
class StateFrontier : public State {
protected:
    FrontierManager frontier_manager_;

    //static const short SEPARATOR_ = 32767;
    enum {SEPARATOR_ = 32767};

public:
    StateFrontier(Graph* graph) : State(graph), frontier_manager_(graph) { }

    virtual void StartNextEdge()
    {
        State::StartNextEdge();
        frontier_manager_.Update(GetCurrentEdge(), GetCurrentEdgeNumber());
    }

    virtual Mate* Initialize(ZDDNode* root_node)
    {
        MateS* mate = new MT(this);
        if (subsetting_dd_ != NULL) {
            mate->SetUseSubsetting(true, root_node);
        }
        return mate;
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
                return zdd->GetZeroTerminal();
            }
        }

        int c = this->CheckTerminalPre(m, child_num); // 終端に遷移するか事前にチェック
        if (c == 0) { // 0終端に行くとき
            return zdd->GetZeroTerminal(); // 0終端を返す
        } else if (c == 1) { // 1終端に行くとき
            return zdd->GetOneTerminal(); // 1終端を返す
        }

        this->UpdateMate(m, child_num); // mate を更新する

        c = this->CheckTerminalPost(m); // 終端に遷移するか再度チェック
        if (c == 0) { // 0終端に行くとき
            return zdd->GetZeroTerminal(); // 0終端を返す
        } else if (c == 1) { // 1終端に行くとき
            return zdd->GetOneTerminal(); // 1終端を返す
        } else {
            ZDDNode* child_node = zdd->CreateNode();
            return child_node;
        }
    }

    template<typename T>
    static void VectorVectorToVector(const std::vector<std::vector<T> >& vv,
                                     std::vector<T>* v)
    {
        for (uint i = 0; i < vv.size(); ++i) {
            for (uint j = 0; j < vv[i].size(); ++j) {
                v->push_back(vv[i][j]);
            }
            v->push_back(SEPARATOR_);
        }
    }

    template<typename T>
    static void VectorToVectorVector(const std::vector<T>& v,
                                     std::vector<std::vector<T> >* vv)
    {
        if (v.size() >= 1) {
            vv->push_back(std::vector<T>());
        }

        for (uint i = 0; i < v.size(); ++i) {
            if (v[i] == SEPARATOR_) {
                if (i < v.size() - 1) {
                    vv->push_back(std::vector<T>());
                }
            } else {
                vv->back().push_back(v[i]);
            }
        }
    }

protected:
    virtual void UpdateMate(MT* mate, int child_num) = 0;
    virtual int CheckTerminalPre(MT* mate, int child_num) = 0;
    virtual int CheckTerminalPost(MT* mate) = 0;
};


} // the end of the namespace

#endif // STATEFRONTIER_HPP
