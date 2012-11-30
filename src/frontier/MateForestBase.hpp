//
// MateForestBase.hpp
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

#ifndef MATEFORESTBASE_HPP
#define MATEFORESTBASE_HPP

#include "StateFrontierFixed.hpp"
#include "PseudoZDD.hpp"

namespace frontier_dd {

template <typename T>
class MateForestBase;

//*************************************************************************************************
// StateForestBase: 全域森、全域木、連結成分など、「森型の mate」を扱うための共通処理を行うクラス
template <typename T>
class StateForestBase : public StateFrontierFixed<T> {
protected:
    MateForestBase<T>* global_mate_;
    mate_t* calc_buff_;
    mate_t* mate_buff_;

public:
    StateForestBase(Graph* graph) : StateFrontierFixed<T>(graph)
    {
        global_mate_ = new MateForestBase<T>(this);
        mate_buff_ = new mate_t[graph->GetNumberOfVertices() + 1];
        calc_buff_ = new mate_t[graph->GetNumberOfVertices() + 1];
    }

    virtual ~StateForestBase()
    {
        delete[] global_mate_;
        delete[] calc_buff_;
    }

    virtual void PackMate(ZDDNode* node, Mate* mate)
    {
        MateForestBase<T>* mt = static_cast<MateForestBase<T>*>(mate);

        mate_t* m = mt->GetMateArray();

        // "sort" start
        // clear buffer
        memset(calc_buff_, 0, (State::GetNumberOfVertices() + 1) * sizeof(mate_t));
        int count = 1;
        for (int i = 0; i < StateFrontier::GetNextFrontierSize(); ++i) {
            int c = StateFrontier::GetNextFrontierValue(i);
            if (m[c] > 0) {
                if (calc_buff_[m[c]] == 0) {
                    calc_buff_[m[c]] = count;
                    mate_buff_[c] = count;
                    ++count;
                } else {
                    mate_buff_[c] = calc_buff_[m[c]];
                }
            } else {
                mate_buff_[c] = m[c];
            }
        }
        // "sort" end

        StateFrontierFixed<T>::PackAll(node, mate_buff_, mt->GetConf());
    }

    virtual Mate* UnpackMate(ZDDNode* node, int child_num)
    {
        if (child_num == 0) { // Hi枝の場合は既にunpackされているので、無視する
            StateFrontierFixed<T>::UnpackAll(node, global_mate_->GetMateArray(),
                global_mate_->GetConf());
        } else {
            StateFrontierFixed<T>::UnpackAll(node, global_mate_->GetMateArray(),
                global_mate_->GetConf());
            StateFrontierFixed<T>::SeekTailAll();
        }
        return global_mate_;
    }
};

//*************************************************************************************************
// MateForestBase
template <typename T>
class MateForestBase : public Mate {
protected:
    mate_t* mate_;
    T conf_;

public:
    MateForestBase(State* state)
    {
        mate_ = new mate_t[state->GetNumberOfVertices() + 1];
    }

    virtual ~MateForestBase()
    {
        if (mate_ != NULL) {
            delete[] mate_;
        }
    }

    mate_t* GetMateArray()
    {
        return mate_;
    }

    T* GetConf()
    {
        return &conf_;
    }

    void Update(State* state, int child_num)
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
    }

    // mate update 前の終端判定。
    // 0終端なら 0, 1終端なら 1, どちらでもない場合は -1 を返す。
    int CheckTerminalPre(State* state, int child_num)
    {
        if (child_num == 0) { // Lo枝のとき
            return -1;
        } else {
            Edge edge = state->GetCurrentEdge();
            if (mate_[edge.src] == mate_[edge.dest]) { // cycle generated
                return 0;
            } else {
                return -1;
            }
        }
    }

    // mate update 後の終端判定。
    // 0終端なら 0, 1終端なら 1, どちらでもない場合は -1 を返す。
    int CheckTerminalPost(State* state)
    {
        if (state->IsLastEdge()) {
            return 1;
        } else {
            return -1;
        }
    }
};

} // the end of the namespace

#endif // MATEFORESTBASE_HPP
