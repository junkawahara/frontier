//
// MateComponent.cpp
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

#include "MateComponent.hpp"

namespace frontier_dd {

using namespace std;

//*************************************************************************************************
// StateComponent
StateComponent::StateComponent(Graph* graph, short component_limit, bool is_le, bool is_me)
    : StateForestBase<MateConfComponent>(graph), component_limit_(component_limit),
        is_le_(is_le), is_me_(is_me)
{
    // delete the instance of MateForestBase created in the constructor of StateForestBase
    delete global_mate_;

    global_mate_ = new MateComponent(this);

    MateConfComponent initial_conf;
    initial_conf.number_of_components = 0;

    StateFrontierFixed<MateConfComponent>::Initialize(initial_conf);
}

StateComponent::~StateComponent()
{
    delete global_mate_;

    // for destructor of StateForestBase
    global_mate_ = NULL;
}

Mate* StateComponent::UnpackMate(ZDDNode* node, int child_num)
{
    if (child_num == 0) {
        StateFrontierFixed<MateConfComponent>::UnpackAll(node, global_mate_->GetMateArray(),
            global_mate_->GetConf());
        StateFrontierFixed<MateConfComponent>::SeekTailFrontier();
    } else {
        StateFrontierFixed<MateConfComponent>::UnpackFixed(node, global_mate_->GetConf());
        StateFrontierFixed<MateConfComponent>::SeekTailFixed();
    }
    return global_mate_;
}

//*************************************************************************************************
// MateComponent
MateComponent::MateComponent(State* state) : MateForestBase<MateConfComponent>(state)
{
    conf_.number_of_components = 0;
}

void MateComponent::Update(State* state, int child_num)
{
    Edge edge = state->GetCurrentEdge();

    if (child_num == 1) { // Hi枝の処理
        int c1 = mate_[edge.src];
        int c2 = mate_[edge.dest];

        int cmax = (c1 < c2 ? c2 : c1);
        int cmin = (c1 < c2 ? c1 : c2);

        // c1 と c2 のうち、大きい値をすべて小さい値に書き換える
        for (int i = 1; i <= state->GetNumberOfVertices(); ++i) {
            if (mate_[i] == cmax) {
                mate_[i] = cmin;
            }
        }
    }

    // 頂点がフロンティアから抜けるときの処理
    set<int> isolating_set;

    StateFrontier* st = static_cast<StateFrontier*>(state);

    for (int i = 0; i < st->GetLeavingFrontierSize(); ++i) {
        mate_t v = st->GetLeavingFrontierValue(i); // フロンティアから抜ける頂点
        bool is_exist = false;
        for (int j = 0; j < st->GetNextFrontierSize(); ++j) {
            if (mate_[v] == mate_[st->GetNextFrontierValue(j)]) {
                is_exist = true; // 更新後のフロンティアに値 v をもつものが存在する
                break;
            }
        }
        if (!is_exist) { // 更新後のフロンティアに値 v をもつものが存在しない
            // mate_[v] の値が初回に現れたときだけ、number_of_components を1増加させる
            if (isolating_set.count(mate_[v]) == 0) {
                isolating_set.insert(mate_[v]);
                ++conf_.number_of_components;
            }
        }
    }
}

// mate update 前の終端判定。
// 0終端なら 0, 1終端なら 1, どちらでもない場合は -1 を返す。
int MateComponent::CheckTerminalPre(State* , int )
{
    return -1;
}

// mate update 後の終端判定。
// 0終端なら 0, 1終端なら 1, どちらでもない場合は -1 を返す。
int MateComponent::CheckTerminalPost(State* state)
{
    StateComponent* st = static_cast<StateComponent*>(state);

    if (state->IsLastEdge()) {
        if (st->IsLe()) {
            if (conf_.number_of_components <= st->GetComponentLimit()) {
                return 1;
            } else {
                return 0;
            }
        } else if (st->IsMe()) {
            if (conf_.number_of_components >= st->GetComponentLimit()) {
                return 1;
            } else {
                return 0;
            }
        } else {
            if (conf_.number_of_components == st->GetComponentLimit()) {
                return 1;
            } else {
                return 0;
            }
        }
    } else {
        if (st->IsLe() || !st->IsMe()) {
            // component の数が、設定した制限を越えた
            if (conf_.number_of_components > st->GetComponentLimit()) {
                return 0;
            } else {
                return -1;
            }
        } else {
            return -1;
        }
    }
}

} // the end of the namespace
