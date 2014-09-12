//
// CompManager.hpp
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

#ifndef COMPMANAGER_HPP
#define COMPMANAGER_HPP

#include <set>


#include "StateFrontier.hpp"
#include "StateFrontierHyper.hpp"

namespace frontier_dd {

//*************************************************************************************************
// CompManager<T>
template<typename T>
class CompManager {
protected:
    T* frontier_array;

    mate_t* calc_buff_;
    FrontierComp* swap_frontier_array_;

public:
    CompManager(T* f_array, int n) : frontier_array(f_array)
    {
        calc_buff_ = new mate_t[2 * (n + 1)];
        swap_frontier_array_ = new FrontierComp[n + 1];
    }

    mate_t* GetCalcBuff()
    {
        return calc_buff_;
    }

    virtual void UpdateMate(StateFrontier<T>* state, int child_num, int* cmin = NULL, int* cmax = NULL)
    {
        int xmin, xmax;
        if (cmin == NULL) {
            cmin = &xmin;
        }
        if (cmax == NULL) {
            cmax = &xmax;
        }
        Edge edge = state->GetCurrentEdge();

        if (child_num == 0) { // Lo枝のとき
            
        } else if (child_num == 1) { // Hi枝のとき
            if (frontier_array[edge.src].comp < frontier_array[edge.dest].comp) {
                *cmin = frontier_array[edge.src].comp;
                *cmax = frontier_array[edge.dest].comp;
            } else {
                *cmin = frontier_array[edge.dest].comp;
                *cmax = frontier_array[edge.src].comp;
            }

            // 大きい値をすべて小さい値に書き換える
            for (int i = 0; i < state->GetPreviousFrontierSize(); ++i) {
                int v = state->GetPreviousFrontierValue(i);
                if (frontier_array[v].comp == *cmax) {
                    frontier_array[v].comp = *cmin;
                }
            }
            //for (int i = 0; i < state->GetNextFrontierSize(); ++i) {
            //    int v = state->GetNextFrontierValue(i);
            //    if (frontier_array[v].comp == *cmax) {
            //        frontier_array[v].comp = *cmin;
            //    }
            //}
            if (frontier_array[edge.src].comp == *cmax) {
                frontier_array[edge.src].comp = *cmin;
            }
            if (frontier_array[edge.dest].comp == *cmax) {
                frontier_array[edge.dest].comp = *cmin;
            }
        }
    }

    void Rename(StateFrontier<T>* state)
    {
        // start rename
        // clear buffer
        memset(calc_buff_, 0, 2 * (state->GetNumberOfVertices() + 1) * sizeof(mate_t));
        int count = 1;
        for (int i = 0; i < state->GetNextFrontierSize(); ++i) {
            int c = state->GetNextFrontierValue(i);
            if (frontier_array[c].comp > 0) {
                if (calc_buff_[frontier_array[c].comp] == 0) {
                    calc_buff_[frontier_array[c].comp] = count;
                    swap_frontier_array_[c].comp = count;
                    ++count;
                } else {
                    swap_frontier_array_[c].comp = calc_buff_[frontier_array[c].comp];
                }
            } else {
                swap_frontier_array_[c].comp = frontier_array[c].comp;
            }
        }
        for (int i = 0; i < state->GetNextFrontierSize(); ++i) {
            int c = state->GetNextFrontierValue(i);
            frontier_array[c].comp = swap_frontier_array_[c].comp;
        }
        //FrontierComp* temp_array = swap_frontier_array_;
        //swap_frontier_array_ = frontier_array;
        //frontier_array = temp_array;
        // end rename
    }
};

//*************************************************************************************************
// CompManagerHyper<T>
template<typename T>
class CompManagerHyper : public CompManager<T> {

public:
    CompManagerHyper(T* f_array, int n) : CompManager<T>(f_array, n)
    {
        // nothing
    }

    virtual void UpdateMate(StateFrontier<T>* state, int child_num, int* cmin = NULL, int* = NULL)
    {
        std::set<mate_t> comp_set;

        int xmin;
        if (cmin == NULL) {
            cmin = &xmin;
        }
        HyperEdge edge = static_cast<StateFrontierHyper<T>*>(state)->GetCurrentHyperEdge();

        if (child_num == 0) { // Lo枝のとき
            
        } else if (child_num == 1) { // Hi枝のとき

            for (uint i = 0; i < edge.var_array.size(); ++i) {
                comp_set.insert(CompManager<T>::frontier_array[edge.var_array[i]].comp);
            }
            *cmin = *std::min_element(comp_set.begin(), comp_set.end());

            // 大きい値をすべて小さい値に書き換える
            for (int i = 0; i < state->GetPreviousFrontierSize(); ++i) {
                int v = state->GetPreviousFrontierValue(i);
                if (comp_set.find(CompManager<T>::frontier_array[v].comp) != comp_set.end()) {
                    CompManager<T>::frontier_array[v].comp = *cmin;
                }
            }
            //for (int i = 0; i < st->GetNextFrontierSize(); ++i) {
            //    int v = st->GetNextFrontierValue(i);
            //    if (comp_set.find(frontier_array[v]) != comp_set.end()) {
            //        frontier_array[v] = *cmin;
            //    }
            //}
            for (uint i = 0; i < edge.var_array.size(); ++i) {
                int v = edge.var_array[i];
                if (comp_set.find(CompManager<T>::frontier_array[v].comp) != comp_set.end()) {
                    CompManager<T>::frontier_array[v].comp = *cmin;
                }
            }
        }
    }

    virtual void UpdateMateWithVSet(StateFrontier<T>* state, int child_num, std::vector<mate_t>** vset, int* cmin = NULL)
    {
        std::set<mate_t> comp_set;

        int xmin;
        if (cmin == NULL) {
            cmin = &xmin;
        }
        HyperEdge edge = static_cast<StateFrontierHyper<T>*>(state)->GetCurrentHyperEdge();

        if (child_num == 0) { // Lo枝のとき
            
        } else if (child_num == 1) { // Hi枝のとき

            for (uint i = 0; i < edge.var_array.size(); ++i) {
                comp_set.insert(CompManager<T>::frontier_array[edge.var_array[i]].comp);
            }
            *cmin = *std::min_element(comp_set.begin(), comp_set.end());

            // 大きい値をすべて小さい値に書き換える
            for (int i = 0; i < state->GetPreviousFrontierSize(); ++i) {
                int v = state->GetPreviousFrontierValue(i);
                if (comp_set.find(CompManager<T>::frontier_array[v].comp) != comp_set.end()) {
                    CompManager<T>::frontier_array[v].comp = *cmin;
                }
            }
            //for (int i = 0; i < st->GetNextFrontierSize(); ++i) {
            //    int v = st->GetNextFrontierValue(i);
            //    if (comp_set.find(frontier_array[v]) != comp_set.end()) {
            //        frontier_array[v] = *cmin;
            //    }
            //}
            for (uint i = 0; i < edge.var_array.size(); ++i) {
                int v = edge.var_array[i];
                if (comp_set.find(CompManager<T>::frontier_array[v].comp) != comp_set.end()) {
                    CompManager<T>::frontier_array[v].comp = *cmin;
                }
            }

            for (std::set<mate_t>::iterator itor = comp_set.begin(); itor != comp_set.end(); ++itor) {
                mate_t v = *itor;
                if (v != *cmin) {
                    for (uint i = 0; i < vset[v]->size(); ++i) {
                        vset[*cmin]->push_back((*vset[v])[i]);
                    }
                    vset[v]->clear();
                }                
            }
        }
    }
};

} // the end of the namespace

#endif // MATECCOMPONENT_HPP
