//
// StateFrontierCompHyper.hpp
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

#ifndef STATEFRONTIERCOMPHYPER_HPP
#define STATEFRONTIERCOMPHYPER_HPP

#include <map>
#include <set>

#include "Global.hpp"
#include "HyperGraph.hpp"
#include "StateFrontierHyper.hpp"
#include "FrontierComp.hpp"

namespace frontier_lib {

template<typename MT>
class StateFrontierCompHyper : public StateFrontierHyper<MT> {
protected:
    std::map<int, int> transmap_;

public:
    StateFrontierCompHyper(HyperGraph* hgraph) : StateFrontierHyper<MT>(hgraph) { }

    ~StateFrontierCompHyper() { }

    virtual void UnpackMate(ZDDNode* node, Mate* mate, int child_num)
    {
        StateFrontierHyper<MT>::UnpackMate(node, mate, child_num);

        MT* m = static_cast<MT*>(mate);

        for (int i = 0; i < StateFrontierHyper<MT>::frontier_manager_.GetEnteringFrontierSize(); ++i) {
            int v = StateFrontierHyper<MT>::frontier_manager_.GetEnteringFrontierValue(i);
            m->frontier[v].comp = v + StateHyper::GetNumberOfVertices();
        }
    }

    // Lo枝またはHi枝の先の子ノードを計算
    // child_num が 0 なら Lo枝、1 なら Hi枝
    virtual ZDDNode* MakeNewNode(ZDDNode* /*node*/, Mate* mate,
                                 int child_num, PseudoZDD* zdd)
    {
        MT* m = static_cast<MT*>(mate);

        if (m->IsUseSubsetting()) {
            if (State::DoSubsetting(child_num, m) == 0) {
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
            RenameComp(m);

            ZDDNode* child_node = zdd->CreateNode();
            return child_node;
        }
    }

    virtual void ConnectComponents(MT* mate, int* cmin = NULL, int* cmax = NULL)
    {
        int xmin, xmax;
        if (cmin == NULL) {
            cmin = &xmin;
        }
        if (cmax == NULL) {
            cmax = &xmax;
        }
        *cmin = INT_MAX;
        *cmax = INT_MIN;

        const HyperEdge& edge = StateHyper::GetCurrentHyperEdge();

        std::set<int> comp_set;
        for (uint i = 0; i < edge.var_array.size(); ++i) {
            int c = mate->frontier[edge.var_array[i]].comp;
            comp_set.insert(c);
            if (c < *cmin) {
                *cmin = c;
            }
            if (c > *cmax) {
                *cmax = c;
            }
        }

        // 大きい値をすべて小さい値に書き換える
        for (int i = 0; i < StateFrontierHyper<MT>::frontier_manager_.GetBothFrontierSize(); ++i) {
            int v = StateFrontierHyper<MT>::frontier_manager_.GetBothFrontierValue(i);
            if (comp_set.find(mate->frontier[v].comp) != comp_set.end()) {
                mate->frontier[v].comp = *cmin;
            }
        }
    }

    // returned value: the number of components
    virtual mate_t RenameComp(MT* mate)
    {
        // start rename

        int plus_count = 1;
        int minus_count = -1;

        transmap_.clear();

        for (int i = 0; i < StateFrontierHyper<MT>::frontier_manager_.GetNextFrontierSize(); ++i) {
            int c = StateFrontierHyper<MT>::frontier_manager_.GetNextFrontierValue(i);
            if (mate->frontier[c].comp > 0) {
                if (transmap_.find(mate->frontier[c].comp) != transmap_.end()) { // exist
                    mate->frontier[c].comp = transmap_[mate->frontier[c].comp];
                } else { // not exist
                    transmap_[mate->frontier[c].comp] = plus_count;
                    mate->frontier[c].comp = plus_count;
                    ++plus_count;
                }
            } else if (mate->frontier[c].comp < 0) {
                if (transmap_.find(mate->frontier[c].comp) != transmap_.end()) { // exist
                    mate->frontier[c].comp = transmap_[mate->frontier[c].comp];
                } else { // not exist
                    transmap_[mate->frontier[c].comp] = minus_count;
                    mate->frontier[c].comp = minus_count;
                    --minus_count;
                }
            }
        }
        // end rename

        return plus_count - 1;
    }
};


} // the end of the namespace

#endif // STATEFRONTIERCOMPHYPER_HPP
