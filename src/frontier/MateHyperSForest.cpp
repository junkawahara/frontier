//
// MateHyperSForest.cpp
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

#include "MateHyperSForest.hpp"

namespace frontier_dd {

using namespace std;

//*************************************************************************************************
// StateHyperSForest

Mate* StateHyperSForest::MakeEmptyMate()
{
   return new MateHyperSForest(this);
}

void StateHyperSForest::UnpackMate(ZDDNode* node, Mate* mate, int child_num)
{
    StateFrontierHyper<FrontierComp>::UnpackMate(node, mate, child_num);

    FrontierComp* frontier_array = static_cast<MateHyperSForest*>(mate)->frontier_array;

    for (int i = 0; i < GetEnteringFrontierSize(); ++i) {
        int v = GetEnteringFrontierValue(i);
        frontier_array[v].comp = v + GetNumberOfVertices();
    }
}

void StateHyperSForest::Load(Mate* , byte* )
{
    // nothing
}

void StateHyperSForest::Store(Mate* , byte* )
{
    // nothing
}

//*************************************************************************************************
// MateHyperSForest

void MateHyperSForest::UpdateMate(State* state, int child_num)
{
    StateHyperSForest* st = static_cast<StateHyperSForest*>(state);
    HyperEdge edge = st->GetCurrentHyperEdge(); 
    set<mate_t> vset;

    comp_manager_.UpdateMate(st, child_num);

    // 頂点がフロンティアから抜けるときの処理
    /*set<int> isolating_set;

    for (int i = 0; i < st->GetLeavingFrontierSize(); ++i) {
        mate_t v = st->GetLeavingFrontierValue(i); // フロンティアから抜ける頂点
        bool is_exist = false;
        for (int j = 0; j < st->GetNextFrontierSize(); ++j) {
            if (frontier_array[v] == frontier_array[st->GetNextFrontierValue(j)]) {
                is_exist = true; // 更新後のフロンティアに値 v をもつものが存在する
                break;
            }
        }
        if (!is_exist) { // 更新後のフロンティアに値 v をもつものが存在しない
            // mate_[v] の値が初回に現れたときだけ、number_of_components を1増加させる
            if (isolating_set.count(frontier_array[v]) == 0) {
                isolating_set.insert(frontier_array[v]);

                //++number_of_components_;
            }
        }
        }*/

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

void MateHyperSForest::Rename(State* state)
{
    comp_manager_.Rename(static_cast<StateFrontierHyper<FrontierComp>*>(state));
}

// mate update 前の終端判定。
// 0終端なら 0, 1終端なら 1, どちらでもない場合は -1 を返す。
int MateHyperSForest::CheckTerminalPre(State* state, int child_num)
{
    StateHyperSForest* st = static_cast<StateHyperSForest*>(state);
    HyperEdge edge = st->GetCurrentHyperEdge();
    std::set<mate_t> vset;

    if (child_num == 0) { // Lo枝のとき

    } else if (child_num == 1) { // Hi枝のとき
        for (uint i = 0; i < edge.var_array.size(); ++i) {
            if (vset.find(frontier_array[edge.var_array[i]].comp) != vset.end()) {
                return 0;
            } else {
                vset.insert(frontier_array[edge.var_array[i]].comp);
            }
        }
    }
    return -1;
}

// mate update 後の終端判定。
// 0終端なら 0, 1終端なら 1, どちらでもない場合は -1 を返す。
int MateHyperSForest::CheckTerminalPost(State* state)
{
    if (state->IsLastEdge()) {
        return 1;
    } else {
        return -1;
    }
}

template<>
std::string MateFrontierHyper<FrontierComp>::GetPreviousString(State* state)
{
    StateFrontierHyper<FrontierComp>* st = (StateFrontierHyper<FrontierComp>*)state;
    std::ostringstream oss;
    std::vector<mate_t> vec;

    oss << "comp[";
    for (int i = 0; i < st->GetPreviousFrontierSize(); ++i) {
        mate_t u = st->GetPreviousFrontierValue(i);

        vec.push_back(frontier_array[u].comp);
        oss << u;

        if (i < st->GetPreviousFrontierSize() - 1) {
            oss << ", ";
        }
    }
    oss << "] = [";

    for (unsigned int i = 0; i < vec.size(); ++i) {
        oss << vec[i];
        if (i < vec.size() - 1) {
            oss << ", ";
        }
    }
    oss << "]";

    return oss.str();
}

template<>
std::string MateFrontierHyper<FrontierComp>::GetNextString(State* state)
{
    StateFrontierHyper<FrontierComp>* st = (StateFrontierHyper<FrontierComp>*)state;
    std::ostringstream oss;
    std::vector<mate_t> vec;

    oss << "comp[";
    for (int i = 0; i < st->GetNextFrontierSize(); ++i) {
        mate_t u = st->GetNextFrontierValue(i);

        vec.push_back(frontier_array[u].comp);
        oss << u;

        if (i < st->GetNextFrontierSize() - 1) {
            oss << ", ";
        }
    }
    oss << "] = [";

    for (unsigned int i = 0; i < vec.size(); ++i) {
        oss << vec[i];
        if (i < vec.size() - 1) {
            oss << ", ";
        }
    }
    oss << "]";

    return oss.str();
}

std::string MateHyperSForest::GetPreviousString(State* state)
{
    return MateFrontierHyper<FrontierComp>::GetPreviousString(state);
}

std::string MateHyperSForest::GetNextString(State* state)
{
    return MateFrontierHyper<FrontierComp>::GetNextString(state);
}


} // the end of the namespace
