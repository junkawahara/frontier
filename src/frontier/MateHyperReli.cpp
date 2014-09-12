//
// MateHyperReli.cpp
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

#include <cstring>
#include <cassert>
#include <algorithm>
#include <sstream>

#include "MateHyperReli.hpp"

namespace frontier_dd {

using namespace std;

//*************************************************************************************************
// StateHyperReli

Mate* StateHyperReli::MakeEmptyMate()
{
   return new MateHyperReli(this);
}

void StateHyperReli::UnpackMate(ZDDNode* node, Mate* mate, int child_num)
{
    StateFrontierHyperAux<FrontierComp>::UnpackMate(node, mate, child_num);
    MateHyperReli* m = static_cast<MateHyperReli*>(mate);

    FrontierComp* frontier_array = m->frontier_array;

    for (int i = 0; i < GetEnteringFrontierSize(); ++i) {
        int v = GetEnteringFrontierValue(i);
        frontier_array[v].comp = v + GetNumberOfVertices();

        if (IsInP(v)) {
            m->vset[v + GetNumberOfVertices()]->push_back(v);
            if (v + GetNumberOfVertices() > m->vset_count) {
                m->vset_count = v + GetNumberOfVertices();
            }
        }
    }
}

int StateHyperReli::GetNextAuxSize()
{
    int n = GetNextFrontierSize();
    return 2 * n * sizeof(short);
}

void StateHyperReli::Load(Mate* mate, byte* data)
{
    MateHyperReli* m = static_cast<MateHyperReli*>(mate);

    int n = *reinterpret_cast<int*>(data) + sizeof(int);
    int pos = sizeof(int);

    int number = 0;

    for (int i = 1; i <= 2 * GetNumberOfVertices(); ++i) {
        m->vset[i]->clear();
    }

    while (pos < n) {
        ++number;
        int xx = *reinterpret_cast<short*>(data + pos);
        pos += sizeof(short);
        //cerr << "xx = " << xx << ", ";
		while (pos < n && xx != 9999) {
            m->vset[number]->push_back(xx);
            xx = *reinterpret_cast<short*>(data + pos);
            //cerr << "xx = " << xx << ", ";
            pos += sizeof(short);
        }
    }
    for (int i = number + 1; i <= GetNumberOfVertices(); ++i) {
        m->vset[i]->clear();
    }
    m->vset_count = number;
    //printf("vset_c = %d\n", m->vset_count);
}

void StateHyperReli::Store(Mate* mate, byte* data)
{
    MateHyperReli* m = static_cast<MateHyperReli*>(mate);
    int pos = sizeof(int);

    for (int i = 1; i <= m->vset_count; ++i) {
        for (unsigned int j = 0; j < m->vset[i]->size(); ++j) {
            *reinterpret_cast<short*>(data + pos) = (*m->vset[i])[j];
            pos += sizeof(short);
        }
        *reinterpret_cast<short*>(data + pos) = 9999; // sentinel 
        pos += sizeof(short);
    }

    *reinterpret_cast<int*>(data) = pos - sizeof(int);

    /*printf("Store: %p: ", data);
    for (int i = 0; i < pos; i += 2) {
        printf("%d ", *reinterpret_cast<short*>(data + i));
    }
    printf("\n");*/
}

//*************************************************************************************************
// MateHyperReli

void MateHyperReli::UpdateMate(State* state, int child_num)
{
    StateHyperReli* st = static_cast<StateHyperReli*>(state);
    HyperEdge edge = st->GetCurrentHyperEdge();

    comp_manager_.UpdateMateWithVSet(st, child_num, vset);
}

void MateHyperReli::Rename(State* state)
{
    comp_manager_.Rename(static_cast<StateFrontierHyper<FrontierComp>*>(state));

    mate_t* calc_buff = comp_manager_.GetCalcBuff();

    int count = 0;
    for (int i = 1; i <= 2 * state->GetNumberOfVertices(); ++i) {
        if (calc_buff[i] > 0) {
            vector<mate_t>* temp = vset[i];
            vset[i] = swap_vset[calc_buff[i]];
            swap_vset[calc_buff[i]] = temp;
            ++count;
        }
    }

	std::vector<mate_t>** temp2 = vset;
	vset = swap_vset;
	swap_vset = temp2;

	for (int i = 1; i <= 2 * state->GetNumberOfVertices(); ++i) {
		swap_vset[i]->clear();
	}

	vset_count = count;
    // end rename
}

// mate update 前の終端判定。
// 0終端なら 0, 1終端なら 1, どちらでもない場合は -1 を返す。
int MateHyperReli::CheckTerminalPre(State* state, int child_num)
{
    StateHyperReli* st = static_cast<StateHyperReli*>(state);
    HyperEdge edge = st->GetCurrentHyperEdge();
    std::set<mate_t> vset;

    if (child_num == 0) { // Lo枝のとき

    } else if (child_num == 1) { // Hi枝のとき
        // check cycle
        //for (uint i = 0; i < edge.var_array.size(); ++i) {
        //    if (vset.find(frontier_array[edge.var_array[i]].comp) != vset.end()) {
        //        return 0;
        //    } else {
        //        vset.insert(frontier_array[edge.var_array[i]].comp);
        //    }
        //}
    }
    return -1;
}

// mate update 後の終端判定。
// 0終端なら 0, 1終端なら 1, どちらでもない場合は -1 を返す。
int MateHyperReli::CheckTerminalPost(State* state)
{
    StateHyperReli* st = static_cast<StateHyperReli*>(state);

    const RootManager* root_mgr = st->GetRootManager();

    for (int i = 0; i < st->GetLeavingFrontierSize(); ++i) {
        mate_t u = st->GetLeavingFrontierValue(i); // フロンティアから抜ける頂点 u

        bool f = false;
        for (int j = 0; j < st->GetNextFrontierSize(); ++j) {
            mate_t z = st->GetNextFrontierValue(j);
            if (frontier_array[u].comp == frontier_array[z].comp) {
                f = true;
                break;
            }
        }

		if (!f) {
            mate_t num = frontier_array[u].comp;

            bool is_same = false;
            bool is_different = false;

            for (int i = 0; i < root_mgr->GetSize(); ++i) {
                mate_t v = FindComponentNumber(vset, vset_count, root_mgr->Get(i));
                                               //frontier_array[reli_list[i]].comp);

                if (v == num) {
                    is_same = true;
                } else {
                    is_different = true;
                }
                if (is_same && is_different) {
                    break;
                }
            }
            if (is_same && is_different) {
                return 0;
            } else if (is_same && !is_different) {
                return 1;
            }
        }
    }

    if (state->IsLastEdge()) {
        return 1;
    } else {
        return -1;
    }
}

std::string MateHyperReli::GetPreviousString(State* state)
{
    ostringstream oss;
    oss << MateFrontierHyper<FrontierComp>::GetPreviousString(state);

    oss << ", vsc = " << vset_count;

    for (int i = 1; i <= vset_count; ++i) {
        oss << ", <";
        for (uint j = 0; j < vset[i]->size(); ++j) {
            oss << (*vset[i])[j];
            if (j < vset[i]->size() - 1) {
                oss << ", ";
            }
        }
        oss << ">";
    }

    return oss.str();
}

std::string MateHyperReli::GetNextString(State* state)
{
    ostringstream oss;
    oss << MateFrontierHyper<FrontierComp>::GetNextString(state);

    oss << ", vsc = " << vset_count;

    for (int i = 1; i <= vset_count; ++i) {
        oss << ", <";
        for (uint j = 0; j < vset[i]->size(); ++j) {
            oss << (*vset[i])[j];
            if (j < vset[i]->size() - 1) {
                oss << ", ";
            }
        }
        oss << ">";
    }

    return oss.str();
}


} // the end of the namespace
