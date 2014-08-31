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
    StateFrontierAux<FrontierVarFGeneral>::UnpackMate(node, mate, child_num);

    FrontierVarFGeneral* mate_array = static_cast<MateFGeneral*>(mate)->frontier_array;

    for (int i = 0; i < GetEnteringFrontierSize(); ++i) {
        int v = GetEnteringFrontierValue(i);
        mate_array[v].deg = 0;
        mate_array[v].comp = v;

        if (IsInPorS(v)) {
            static_cast<MateFGeneral*>(mate)->vset[v]->push_back(v);
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

    m->cc = *reinterpret_cast<short*>(data + pos);
    pos += sizeof(short);
    m->cycle = (*reinterpret_cast<short*>(data + pos) == 1);
    pos += sizeof(short);
    m->noe = *reinterpret_cast<short*>(data + pos);
    pos += sizeof(short);

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

    *reinterpret_cast<short*>(data + pos) = m->cc;
    pos += sizeof(short);
    *reinterpret_cast<short*>(data + pos) = (m->cycle ? 1 : 0);
    pos += sizeof(short);
    *reinterpret_cast<short*>(data + pos) = m->noe;
    pos += sizeof(short);

    for (unsigned int i = 1; i <= m->vset_count; ++i) {
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

//int cmp(const void* a, const void* b) {
//    return *static_cast<short*>(a) - *static_cast<short*>(b);
//}

void MateFGeneral::UpdateMate(State* state, int child_num)
{
    Edge edge = state->GetCurrentEdge();
    StateFrontier<mate_t>* st = static_cast<StateFrontier<mate_t>*>(state);

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
			if (frontier_array[edge.src].comp < frontier_array[edge.dest].comp) {
				cmin = frontier_array[edge.src].comp;
				cmax = frontier_array[edge.dest].comp;
			}
			else {
				cmin = frontier_array[edge.dest].comp;
				cmax = frontier_array[edge.src].comp;
			}

			// 大きい値をすべて小さい値に書き換える (next と previous の両方）
			for (int i = 0; i < st->GetPreviousFrontierSize(); ++i) {
				int v = st->GetPreviousFrontierValue(i);
				if (frontier_array[v].comp == cmax) {
					frontier_array[v].comp = cmin;
				}
			}
			for (int i = 0; i < st->GetNextFrontierSize(); ++i) {
				int v = st->GetNextFrontierValue(i);
				if (frontier_array[v].comp == cmax) {
					frontier_array[v].comp = cmin;
				}
			}

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

                ++cc;
            }
        }
    }

    // start rename
    // clear buffer
    memset(calc_buff_, 0, (st->GetNumberOfVertices() + 1) * sizeof(mate_t));
    int count = 0;
    for (int i = 0; i < st->GetNextFrontierSize(); ++i) {
        int c = st->GetNextFrontierValue(i);
        if (frontier_array[c].comp > 0) {
            if (calc_buff_[frontier_array[c].comp] == 0) {
				++count;
                calc_buff_[frontier_array[c].comp] = count;
                swap_frontier_array_[c] = count;

				vector<mate_t>* temp = vset[frontier_array[c].comp];
				vset[frontier_array[c].comp] = swap_vset[count];
				swap_vset[count] = temp;
            } else {
                swap_frontier_array_[c] = calc_buff_[frontier_array[c].comp];
            }
        } else {
            swap_frontier_array_[c] = frontier_array[c].comp;
        }
    }
    for (int i = 0; i < st->GetNextFrontierSize(); ++i) {
        int c = st->GetNextFrontierValue(i);

        frontier_array[c].comp = swap_frontier_array_[c];
    }

	std::vector<mate_t>** temp2 = vset;
	vset = swap_vset;
	swap_vset = temp2;

	for (int i = 1; i <= state->GetNumberOfVertices(); ++i) {
		swap_vset[i]->clear();
	}

	vset_count = count;
    // end rename

    /*cout << number_of_components_ << "# ";

    for (int i = 1; i <= st->GetNumberOfVertices(); ++i) {
        cout << frontier_array[i] << ", ";
    }
    cout << endl;*/
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
        if (!IsIn(frontier_array[u].deg, st->D_[u])) { // Line 12-13
            return 0;
        }
        bool f = false;
        for (int j = 0; j < st->GetNextFrontierSize(); ++j) { // Line 15-17
            mate_t z = st->GetNextFrontierValue(j);
            if (frontier_array[u].comp == frontier_array[z].comp) { // Line 16-17
                f = true;
                break;
            }
        }
		if (!f) { // Line 18
			if (cc > *max_element(st->C_.begin(), st->C_.end())) { // Line 19-20
				return 0;
			}
			// Line 21-22
			for (unsigned int i = 0; i < st->P_.size(); ++i) {
				int vv = FindComponentNumber(vset, vset_count, st->P_[i].first);
				int vvv = FindComponentNumber(vset, vset_count, st->P_[i].second);
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
        if (!IsIn(cc, st->C_)) { // Line 24-25
            return 0;
        } else if (!IsIn(noe, st->T_)) { // Line 26-27
            return 0;
        } else if (!IsIn(0, st->Q_) && !cycle) {
            return 0;
        } else {
            return 1;
        }
    }
    return -1; // Line 32
}

} // the end of the namespace
