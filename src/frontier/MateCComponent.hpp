//
// MateCComponent.hpp
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

#ifndef MATECCOMPONENT_HPP
#define MATECCOMPONENT_HPP

#include "StateFrontierAux.hpp"
#include "PseudoZDD.hpp"

namespace frontier_dd {

class MateCComponent;

struct VarTypeCComponent {
    int size;
    double sum;
};

struct ShortPair {
    short first;
    short second;

	ShortPair() { }

	ShortPair(short f, short s)
	{
		first = f;
		second = s;
	}
};

//*************************************************************************************************
// StateCComponent: 連結成分のための State
class StateCComponent : public StateFrontierAux<mate_t> {
protected:
    short component_limit_;
    bool is_le_; // true なら component 数を component_limit_ 以下に制限
    bool is_me_; // true なら component 数を component_limit_ 以上に制限
    // is_le_, is_me_ ともに false なら component 数を component_limit_ に制限

    ShortPair* num_to_pair_table_;

public:
    StateCComponent(Graph* graph, short component_limit, bool is_le, bool is_me)
        : StateFrontierAux<mate_t>(graph), component_limit_(component_limit),
          is_le_(is_le), is_me_(is_me)
    {
        byte initial_conf[sizeof(int) + 2];
        *reinterpret_cast<int*>(initial_conf) = 2;
        *reinterpret_cast<short*>(initial_conf + sizeof(int)) = 0;
        StateFrontierAux<mate_t>::Initialize(initial_conf, 2);

        int n = graph->GetNumberOfVertices();

        num_to_pair_table_ = new ShortPair[n * (n + 1) / 2 + 1];

        int count = 1;
        for (int j = 1; j <= n; ++j) {
            for (int i = 1; i <= j; ++i) {
                num_to_pair_table_[count].first = i;
                num_to_pair_table_[count].second = j;
                ++count;
            }
        }
    }

    virtual ~StateCComponent() { }

    static short PairToNum(const ShortPair& pair)
    {
        if (pair.first < pair.second) {
            return pair.second * (pair.second - 1) / 2 + pair.first;
        } else {
            return pair.first * (pair.first - 1) / 2 + pair.second;
        }
    }

    void TestPairTable()
    {
        int n = GetNumberOfVertices();

        std::cout << "TestPairTable start" << std::endl;
        for (int i = 1; i <= n * (n + 1) / 2; ++i) {
            std::cout << num_to_pair_table_[i].first << ", " << num_to_pair_table_[i].second << std::endl;
        }

        for (int j = 1; j <= n; ++j) {
            for (int i = 1; i <= n; ++i) {
                if (i < j) {
                    std::cout << i << ", " << j << ": " << j * (j - 1) / 2 + i << ", "
                              << num_to_pair_table_[j * (j - 1) / 2 + i].first << ", "
                              << num_to_pair_table_[j * (j - 1) / 2 + i].second << std::endl;
                } else {
                    std::cout << i << ", " << j << ": " << i * (i - 1) / 2 + j << ", "
                              << num_to_pair_table_[i * (i - 1) / 2 + j].first << ", "
                              << num_to_pair_table_[i * (i - 1) / 2 + j].second << std::endl;
                }
            }
        }
    }

    short GetComponentLimit() const
    {
        return component_limit_;
    }

    bool IsLe() const
    {
        return is_le_;
    }

    bool IsMe() const
    {
        return is_me_;
    }

    virtual int GetNextAuxSize();
    virtual void Load(Mate* mate, byte* data);
    virtual void Store(Mate* mate, byte* data);

    virtual Mate* MakeEmptyMate();
    //virtual void PackMate(ZDDNode* node, Mate* mate);
    virtual void UnpackMate(ZDDNode* node, Mate* mate, int child_num);
};

//*************************************************************************************************
// MateCComponent
class MateCComponent : public MateFrontier<mate_t> {
public:
    short number_of_components_;
    ShortPair* pair_array_;
    int pair_count_;

    ShortPair* swap_pair_array_;
    mate_t* calc_buff_;
    mate_t* swap_frontier_array_;
    short* sort_buff_;

public:
    MateCComponent(State* state) : MateFrontier<mate_t>(state), number_of_components_(0), pair_count_(0)
    {
        int n = state->GetNumberOfVertices();
        pair_array_ = new ShortPair[n * (n - 1) / 2];

        swap_pair_array_ = new ShortPair[n * (n - 1) / 2];
        calc_buff_ = new mate_t[n + 1];
        swap_frontier_array_ = new mate_t[n + 1];
        sort_buff_ = new short[n * (n - 1) / 2];
    }

    virtual void UpdateMate(State* state, int child_num);
    virtual int CheckTerminalPre(State* state, int child_num);
    virtual int CheckTerminalPost(State* state);
};

} // the end of the namespace

#endif // MATECCOMPONENT_HPP
