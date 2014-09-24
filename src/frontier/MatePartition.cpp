//
// MatePartition.cpp
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

#include "MatePartition.hpp"

namespace frontier_dd {

using namespace std;

//*************************************************************************************************
// StatePartition

Mate* StatePartition::MakeEmptyMate()
{
   return new MatePartition(this);
}

void StatePartition::UnpackMate(ZDDNode* node, Mate* mate, int child_num)
{
    StateFrontierAux<FrontierComp>::UnpackMate(node, mate, child_num);

    FrontierComp* frontier_array = static_cast<MatePartition*>(mate)->frontier_array;

    for (int i = 0; i < GetEnteringFrontierSize(); ++i) {
        int v = GetEnteringFrontierValue(i);
        frontier_array[v].comp = v + GetNumberOfVertices();
        if (graph_->IsUsingVertexWeight()) {
            static_cast<MatePartition*>(mate)->comp_weight_array_[v + GetNumberOfVertices()] = graph_->GetVertexWeight(v);
        }
    }
}

int StatePartition::GetNextAuxSize()
{
    int n = GetNextFrontierSize();
    if (graph_->IsUsingVertexWeight()) {
        return n * n - n + 2 + 2 * n * sizeof(int) + sizeof(mate_t);
    } else {
        return n * n - n + 2;
    }
}

void StatePartition::Load(Mate* mate, byte* data)
{
    MatePartition* mate2 = static_cast<MatePartition*>(mate);

    int size = *reinterpret_cast<int*>(data);
    mate2->number_of_components_ = *reinterpret_cast<short*>(data + sizeof(int));

    int pos = sizeof(int) + sizeof(short);

    if (graph_->IsUsingVertexWeight()) {
        //size -= 2 * m->number_of_components_ - sizeof(mate_t);

        mate2->current_comp_num_ = *reinterpret_cast<mate_t*>(data + pos);
        pos += sizeof(mate_t);

        for (int i = 1; i <= mate2->current_comp_num_; ++i) {
            mate2->comp_weight_array_[i] = *reinterpret_cast<int*>(data + pos);
            pos += sizeof(int);
        }
        for (int i = mate2->current_comp_num_ + 1; i <= 2 * GetNumberOfVertices(); ++i) { // really need?
            mate2->comp_weight_array_[i] = 0;
        }
    }
    //printf("Load %d %d %d %d %d %d %d %d\n", data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]);
    //printf("Load pointer = %p, n = %d, # of comp = %d", data, n, m->number_of_components_);

    mate2->pair_count_ = 0;
    while (pos < size + static_cast<int>(sizeof(int))) {
        mate2->pair_array_[mate2->pair_count_] = num_to_pair_table_[*reinterpret_cast<short*>(data + pos)];
        //printf(", (%d, %d)", m->pair_array_[i].first, m->pair_array_[i].second);
        ++mate2->pair_count_;
        pos += sizeof(short);
    }
    //printf("\n");
}

void StatePartition::Store(Mate* mate, byte* data)
{
    MatePartition* mate2 = static_cast<MatePartition*>(mate);

    *reinterpret_cast<short*>(data + sizeof(int)) = mate2->number_of_components_;
    //printf("store pointer = %p, # of comp = %d", data, m->number_of_components_);

    int pos = sizeof(int) + sizeof(short);

    if (graph_->IsUsingVertexWeight()) {
        *reinterpret_cast<mate_t*>(data + pos) = mate2->current_comp_num_;
        pos += sizeof(mate_t);

        for (int i = 1; i <= mate2->current_comp_num_; ++i) {
            *reinterpret_cast<int*>(data + pos) = mate2->comp_weight_array_[i];
            pos += sizeof(int);
        }
    }
    for (int i = 0; i < mate2->pair_count_; ++i) {
        if (i < mate2->pair_count_ - 1 && mate2->sort_buff_[i] == mate2->sort_buff_[i + 1]) {
            continue;
        }
        *reinterpret_cast<short*>(data + pos) = mate2->sort_buff_[i];
        //printf(", %d(%d, %d)", PairToNum(m->pair_array_[i]), m->pair_array_[i].first, m->pair_array_[i].second);
        pos += sizeof(short);
    }
    *reinterpret_cast<int*>(data) = pos - sizeof(int);

    //printf("Store %d %d %d %d %d %d %d %d\n", data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]);
}

//*************************************************************************************************
// MatePartition

void MatePartition::UpdateMate(State* state, int child_num)
{
    Edge edge = state->GetCurrentEdge();
    StateFrontier<FrontierComp>* st = static_cast<StateFrontier<FrontierComp>*>(state);

    if (child_num == 0) { // Lo枝のとき
        if (frontier_array[edge.src].comp != frontier_array[edge.dest].comp) { // これは常にtrueのはず
            pair_array_[pair_count_].first = frontier_array[edge.src].comp;
            pair_array_[pair_count_].second = frontier_array[edge.dest].comp;
            ++pair_count_;
        }
    } else if (child_num == 1) { // Hi枝のとき
        int cmin, cmax;

        comp_manager_.UpdateMate(st, child_num, &cmin, &cmax);

        if (state->IsUsingVertexWeight()) {
            comp_weight_array_[cmin] += comp_weight_array_[cmax];
            comp_weight_array_[cmax] = 0;
        }

        for (int i = 0; i < pair_count_; ++i) {
            if (pair_array_[i].first == cmax) {
                pair_array_[i].first = cmin;
            }
            if (pair_array_[i].second == cmax) {
                pair_array_[i].second = cmin;
            }
        }
    }

    // 頂点がフロンティアから抜けるときの処理
    set<int> isolating_set;

    for (int i = 0; i < st->GetLeavingFrontierSize(); ++i) {
        mate_t v = st->GetLeavingFrontierValue(i); // フロンティアから抜ける頂点
        bool is_exist = false;
        for (int j = 0; j < st->GetNextFrontierSize(); ++j) {
            if (frontier_array[v].comp == frontier_array[st->GetNextFrontierValue(j)].comp) {
                is_exist = true; // 更新後のフロンティアに値 v をもつものが存在する
                break;
            }
        }
        if (!is_exist) { // 更新後のフロンティアに値 v をもつものが存在しない
            // mate_[v] の値が初回に現れたときだけ、number_of_components を1増加させる
            if (isolating_set.count(frontier_array[v].comp) == 0) {
                isolating_set.insert(frontier_array[v].comp);

                // delete frontier_array_[v]
                int new_count = 0;
                for (int i = 0; i < pair_count_; ++i) {
                    if (pair_array_[i].first != frontier_array[v].comp && pair_array_[i].second != frontier_array[v].comp) {
                        swap_pair_array_[new_count] = pair_array_[i];
                        ++new_count;
                    }
                }
                ShortPair* temp_pa = swap_pair_array_;
                swap_pair_array_ = pair_array_;
                pair_array_ = temp_pa;
                pair_count_ = new_count;
                ++number_of_components_;
            }
        }
    }

    /*cout << number_of_components_ << "# ";

    for (int i = 1; i <= st->GetNumberOfVertices(); ++i) {
        cout << frontier_array[i] << ", ";
    }

    cout << "#";
    for (int i = 0; i < pair_count_; ++i) {
        cout << sort_buff_[i] << ", ";
    }
    cout << endl;*/
}

void MatePartition::Rename(State* state)
{
    StatePartition* st = (StatePartition*)state;

    // start rename
    current_comp_num_ = comp_manager_.Rename(st);

    mate_t* calc_buff = comp_manager_.GetCalcBuff();

    for (int i = 0; i < pair_count_; ++i) {
        if (calc_buff[pair_array_[i].first] > 0) {
            pair_array_[i].first = calc_buff[pair_array_[i].first];
        }
        if (calc_buff[pair_array_[i].second] > 0) {
            pair_array_[i].second = calc_buff[pair_array_[i].second];
        }
    }
    int count_num = 0;
    for (int i = 0; i < pair_count_; ++i) {
        if (pair_array_[i].first != pair_array_[i].second) {
            sort_buff_[count_num] = StatePartition::PairToNum(pair_array_[i]);
            ++count_num;
        }
    }
    pair_count_ = count_num;
    // end rename

    if (state->IsUsingVertexWeight()) {
        for (int i = 1; i <= 2 * state->GetNumberOfVertices(); ++i) {
            swap_comp_weight_array_[i] = 0;
        }

        for (int i = 1; i <= 2 * state->GetNumberOfVertices(); ++i) {
            //cerr << "i = " << i << ", c[i] = " << calc_buff[i] << ", w[i] = " << comp_weight_array_[i] << endl;
            if (calc_buff[i] > 0) {
                swap_comp_weight_array_[calc_buff[i]] = comp_weight_array_[i];
            }
        }
        std::swap(comp_weight_array_, swap_comp_weight_array_);
    }

    std::sort(sort_buff_, sort_buff_ + pair_count_);
}

// mate update 前の終端判定。
// 0終端なら 0, 1終端なら 1, どちらでもない場合は -1 を返す。
int MatePartition::CheckTerminalPre(State* state, int child_num)
{
    Edge edge = state->GetCurrentEdge();

    if (child_num == 0) { // Lo枝のとき
        if (frontier_array[edge.src].comp == frontier_array[edge.dest].comp) {
            return 0;
        }
    } else if (child_num == 1) { // Hi枝のとき
        if (frontier_array[edge.src].comp != frontier_array[edge.dest].comp) {
            // check forbidden list

            int cmin, cmax;
            if (frontier_array[edge.src].comp < frontier_array[edge.dest].comp) {
                cmin = frontier_array[edge.src].comp;
                cmax = frontier_array[edge.dest].comp;
            } else {
                cmin = frontier_array[edge.dest].comp;
                cmax = frontier_array[edge.src].comp;
            }

            if (state->IsUsingVertexWeight()) {
                if (cmin != cmax && comp_weight_array_[cmin] + comp_weight_array_[cmax] >
                    static_cast<StatePartition*>(state)->GetMaxVertexWeight()) {
                    return 0;
                }
            }

            for (int i = 0; i < pair_count_; ++i) {
                if (pair_array_[i].first == cmin && pair_array_[i].second == cmax) {
                    return 0;
                }
            }
        }
    }
    return -1;
}

// mate update 後の終端判定。
// 0終端なら 0, 1終端なら 1, どちらでもない場合は -1 を返す。
int MatePartition::CheckTerminalPost(State* state)
{
    StatePartition* st = static_cast<StatePartition*>(state);

    if (state->IsLastEdge()) {
        if (st->IsLe()) {
            if (number_of_components_ <= st->GetComponentLimit()) {
                return 1;
            } else {
                return 0;
            }
        } else if (st->IsMe()) {
            if (number_of_components_ >= st->GetComponentLimit()) {
                return 1;
            } else {
                return 0;
            }
        } else {
            if (number_of_components_ == st->GetComponentLimit()) {
                return 1;
            } else {
                return 0;
            }
        }
    } else {
        if (st->IsLe() || !st->IsMe()) {
            // component の数が、設定した制限を越えた
            if (number_of_components_ > st->GetComponentLimit()) {
                return 0;
            } else {
                return -1;
            }
        } else {
            return -1;
        }
    }
}

std::string MatePartition::GetPreviousString(State* state)
{
    ostringstream oss;
    oss << MateFrontier<FrontierComp>::GetPreviousString(state);
    oss << ", # of comp. = " << number_of_components_ << ", ";

    if (state->IsUsingVertexWeight()) {
        oss << "weight = [";
        for (int i = 1; i <= current_comp_num_; ++i) {
            oss << comp_weight_array_[i];
            if (i < current_comp_num_) {
                oss << ", ";
            }
        }
        oss << "], ";
    }

    if (pair_count_ > 0) {
        oss << "pair = ";
    }
    for (int i = 0; i < pair_count_; ++i) {
        oss << "(" << pair_array_[i].first << ", " << pair_array_[i].second << ")";
        if (i < pair_count_ - 1) {
            oss << ", ";
        }
    }
    return oss.str();
}

std::string MatePartition::GetNextString(State* state)
{
    ostringstream oss;
    oss << MateFrontier<FrontierComp>::GetNextString(state);
    oss << ", # of comp. = " << number_of_components_ << ", ";

    if (state->IsUsingVertexWeight()) {
        oss << "weight = [";
        for (int i = 1; i <= current_comp_num_; ++i) {
            oss << comp_weight_array_[i];
            if (i < current_comp_num_) {
                oss << ", ";
            }
        }
        oss << "], ";
    }

    if (pair_count_ > 0) {
        oss << "pair = ";
    }
    for (int i = 0; i < pair_count_; ++i) {
        oss << "(" << pair_array_[i].first << ", " << pair_array_[i].second << ")";
        if (i < pair_count_ - 1) {
            oss << ", ";
        }
    }
    return oss.str();
}


} // the end of the namespace
