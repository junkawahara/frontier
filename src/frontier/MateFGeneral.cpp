//
// MateFGeneral.cpp
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

#include "MateFGeneral.hpp"

namespace frontier_dd {

using namespace std;

//*************************************************************************************************
// StateFGeneral

Mate* StateFGeneral::MakeEmptyMate()
{
   return new MateFGeneral(this);
}

void StateFGeneral::UnpackMate(ZDDNode* node, Mate* mate, int child_num)
{
    StateFrontierAux<FrontierDegComp>::UnpackMate(node, mate, child_num);

    FrontierDegComp* frontier_array = static_cast<MateFGeneral*>(mate)->frontier_array;

    for (int i = 0; i < GetEnteringFrontierSize(); ++i) {
        int v = GetEnteringFrontierValue(i);
        frontier_array[v].deg = 0;
        frontier_array[v].comp = v + GetNumberOfVertices();

        if (IsInPorS(v)) {
            static_cast<MateFGeneral*>(mate)->vset[v]->push_back(v + GetNumberOfVertices());
        }
    }
}

int StateFGeneral::GetNextAuxSize()
{
    int n = GetNextFrontierSize();
    return 3 * sizeof(short) + 2 * n * sizeof(short);
}

void StateFGeneral::Load(Mate* mate, byte* data)
{
    MateFGeneral* m = static_cast<MateFGeneral*>(mate);

    int n = *reinterpret_cast<int*>(data) + sizeof(int);
    int pos = sizeof(int);

	if (is_using_cc_) {
		m->cc = *reinterpret_cast<short*>(data + pos);
		pos += sizeof(short);
	}
	if (is_using_cycle_) {
		m->cycle = (*reinterpret_cast<short*>(data + pos) == 1);
		pos += sizeof(short);
	}
	if (is_using_noe_) {
		m->noe = *reinterpret_cast<short*>(data + pos);
		pos += sizeof(short);
	}

    //printf("Load: %p: %d %d %d: ", data, m->cc, m->cycle, m->noe);

    int number = 0;

    while (pos < n) {
        ++number;
        m->vset[number]->clear();
        int xx = *reinterpret_cast<short*>(data + pos);
        pos += sizeof(short);
        //printf("%d, ", xx);
		while (pos < n && xx != 9999) {
            m->vset[number]->push_back(xx);
            xx = *reinterpret_cast<short*>(data + pos);
			//printf("%d, ", xx);
            pos += sizeof(short);
        }
    }
    for (int i = number + 1; i <= GetNumberOfVertices(); ++i) {
        m->vset[i]->clear();
    }
    m->vset_count = number;
    //printf("vset_c = %d\n", m->vset_count);
}

void StateFGeneral::Store(Mate* mate, byte* data)
{
    MateFGeneral* m = static_cast<MateFGeneral*>(mate);
    int pos = sizeof(int);

	if (is_using_cc_) {
		*reinterpret_cast<short*>(data + pos) = m->cc;
		pos += sizeof(short);
	}
	if (is_using_cycle_) {
		*reinterpret_cast<short*>(data + pos) = (m->cycle ? 1 : 0);
		pos += sizeof(short);
	}
	if (is_using_noe_) {
		*reinterpret_cast<short*>(data + pos) = m->noe;
		pos += sizeof(short);
	}

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
// MateFGeneral

void MateFGeneral::UpdateMate(State* state, int child_num)
{
    Edge edge = state->GetCurrentEdge();
    StateFGeneral* st = static_cast<StateFGeneral*>(state);

    if (child_num == 0) { // Lo枝のとき

    } else if (child_num == 1) { // Hi枝のとき  Line 7
        ++noe;
        ++frontier_array[edge.src].deg;
        ++frontier_array[edge.dest].deg;

        if (frontier_array[edge.src].comp == frontier_array[edge.dest].comp) { // Line 11-12
            cycle = true;
		}
		else {
			int cmin, cmax;
            comp_manager_.UpdateMate(st, child_num, &cmin, &cmax);

			for (unsigned int i = 0; i < vset[cmax]->size(); ++i) {
				vset[cmin]->push_back((*vset[cmax])[i]);
			}
			vset[cmax]->clear();
		}
    }

    // 頂点がフロンティアから抜けるときの処理
    set<int> isolating_set;

    for (int i = 0; i < st->GetLeavingFrontierSize(); ++i) {
        mate_t v = st->GetLeavingFrontierValue(i); // フロンティアから抜ける頂点
        bool f = false;
        for (int j = 0; j < st->GetNextFrontierSize(); ++j) {
            if (frontier_array[v].comp == frontier_array[st->GetNextFrontierValue(j)].comp) {
                f = true; // 更新後のフロンティアに値 v をもつものが存在する
                break;
            }
        }
        if (!f) { // 更新後のフロンティアに値 v をもつものが存在しない
            // mate_[v] の値が初回に現れたときだけ、number_of_components を1増加させる
            if (isolating_set.count(frontier_array[v].comp) == 0) {
                isolating_set.insert(frontier_array[v].comp);

                if (!st->IsIgnoreIsolated() || frontier_array[v].deg > 0) {
                    ++cc;
                }
            }
        }
    }

    
    /*cout << number_of_components_ << "# ";

    for (int i = 1; i <= st->GetNumberOfVertices(); ++i) {
        cout << frontier_array[i] << ", ";
    }
    cout << endl;*/
}

void MateFGeneral::Rename(State* state)
{
    StateFGeneral* st = static_cast<StateFGeneral*>(state);

    comp_manager_.Rename(st);

    mate_t* calc_buff = comp_manager_.GetCalcBuff();

    int count = 0;
    for (int i = 1; i <= 2 * state->GetNumberOfVertices(); ++i) {
        if (calc_buff[i] > 0) {
            ++count;
            vector<mate_t>* temp = vset[i];
            vset[i] = swap_vset[calc_buff[i]];
            swap_vset[calc_buff[i]] = temp;
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
int MateFGeneral::CheckTerminalPre(State* state, int child_num)
{
    StateFGeneral* st = static_cast<StateFGeneral*>(state);
    Edge edge = state->GetCurrentEdge();

    if (child_num == 0) { // Lo枝のとき

    } else if (child_num == 1) { // Hi枝のとき
        if (!IsIn(1, st->Q_) && frontier_array[edge.src].comp == frontier_array[edge.dest].comp) {
            return 0;
        }
		for (unsigned int i = 0; i < st->S_.size(); ++i) {
			if (IsIn(st->S_[i].first, *vset[frontier_array[edge.src].comp]) &&
				IsIn(st->S_[i].second, *vset[frontier_array[edge.dest].comp])) {
				return 0;
			}
			if (IsIn(st->S_[i].first, *vset[frontier_array[edge.dest].comp]) &&
				IsIn(st->S_[i].second, *vset[frontier_array[edge.src].comp])) {
				return 0;
			}
		}
    }
    return -1;
}

// mate update 後の終端判定。
// 0終端なら 0, 1終端なら 1, どちらでもない場合は -1 を返す。
int MateFGeneral::CheckTerminalPost(State* state)
{
    StateFGeneral* st = static_cast<StateFGeneral*>(state);
    Edge edge = state->GetCurrentEdge();

    //printf("deg = %d %d %d %d, ", frontier_array[1].deg, frontier_array[2].deg, frontier_array[3].deg, frontier_array[4].deg);
    //printf("comp = %d %d %d %d, ", frontier_array[1].comp, frontier_array[2].comp, frontier_array[3].comp, frontier_array[4].comp);

    // Line 7-8
    if (frontier_array[edge.src].deg > *max_element(st->D_[edge.src].begin(), st->D_[edge.src].end())
        || frontier_array[edge.dest].deg > *max_element(st->D_[edge.dest].begin(), st->D_[edge.dest].end())) {
            return 0;
    }

    // Line 9-10
    //for (unsigned int i = 0; i < st->S_.size(); ++i) {
    //    for (unsigned int j = 1; j <= vset_count; ++j) {
    //        if (IsIn(st->S_[i].first, *vset[j]) && IsIn(st->S_[i].second, *vset[j])) {
    //            return 0;
    //        }
    //    }
    //}

    // Line 11-22
    for (int i = 0; i < st->GetLeavingFrontierSize(); ++i) {
        mate_t u = st->GetLeavingFrontierValue(i); // フロンティアから抜ける頂点 u
        //printf("yy%d: ", u);
        if (!IsIn(frontier_array[u].deg, st->D_[u])) { // Line 12-13
            return 0;
        }
        //printf("zz: ");
        bool f = false;
        for (int j = 0; j < st->GetNextFrontierSize(); ++j) { // Line 15-17
            mate_t z = st->GetNextFrontierValue(j);
            //printf("(%d, %d, %d, %d)", u, z, frontier_array[u].comp, frontier_array[z].comp);
            if (frontier_array[u].comp == frontier_array[z].comp) { // Line 16-17
                f = true;
                break;
            }
        }
        //printf("pp: %s", (f ? "t" : "f"));
		if (!f) { // Line 18
			if (st->IsUsingCC() && cc > *max_element(st->C_.begin(), st->C_.end())) { // Line 19-20
				return 0;
			}
			// Line 21-22
			for (unsigned int i = 0; i < st->P_.size(); ++i) {
				int vv = FindComponentNumber(vset, vset_count, st->P_[i].first);
				int vvv = FindComponentNumber(vset, vset_count, st->P_[i].second);
                //printf("xx%d, %d, %d, %d\n", vv, vvv, u, frontier_array[u].comp);
				if (vv == frontier_array[u].comp && vv != vvv) {
					return 0;
				}
				if (vvv == frontier_array[u].comp && vv != vvv) {
					return 0;
				}
			}
		}
    }

    // Line 23-31
    if (state->IsLastEdge()) {
        if (st->IsUsingCC() && !IsIn(cc, st->C_)) { // Line 24-25
            return 0;
        } else if (st->IsUsingNoe() && !IsIn(noe, st->T_)) { // Line 26-27
            return 0;
        } else if (st->IsUsingCycle() && !IsIn(0, st->Q_) && !cycle) {
            return 0;
        } else {
            return 1;
        }
    }
    return -1; // Line 32
}

template<>
std::string MateFrontier<FrontierDegComp>::GetPreviousString(State* state)
{
    StateFrontier<mate_t>* st = (StateFrontier<mate_t>*)state;
    std::ostringstream oss;
    std::vector<mate_t> vec;

    oss << "deg[";
    for (int i = 0; i < st->GetPreviousFrontierSize(); ++i) {
        mate_t u = st->GetPreviousFrontierValue(i);

        vec.push_back(frontier_array[u].deg);
        oss << u << ", ";
    }
    oss << "] = [";

    for (unsigned int i = 0; i < vec.size(); ++i) {
        oss << vec[i] << ", ";
    }
    oss << "], ";

    return oss.str();
}

template<>
std::string MateFrontier<FrontierDegComp>::GetNextString(State* )
{
    std::string str;

    return str;
}

std::string MateFGeneral::GetPreviousString(State* state)
{
    ostringstream oss;
    oss << MateFrontier<FrontierDegComp>::GetPreviousString(state);
    //oss << ", # of comp. = " << number_of_components_ << ", ";

    // under const.

    return oss.str();
}


std::string MateFGeneral::GetNextString(State* state)
{
    ostringstream oss;
    oss << MateFrontier<FrontierDegComp>::GetNextString(state);
    //oss << ", # of comp. = " << number_of_components_ << ", ";

    // under const.

    return oss.str();
}



} // the end of the namespace
