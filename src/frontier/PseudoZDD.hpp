//
// PseudoZDD.hpp
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

#ifndef PSEUDOZDD_HPP
#define PSEUDOZDD_HPP

#include <vector>
#include <algorithm>
#include <cstdarg>
#include <cstdio>
#include <climits>
#include <iostream>
#include <fstream>

#include "Global.hpp"
#include "State.hpp"
#include "ZDDNode.hpp"
#include "SolutionArray.hpp"

namespace frontier_dd {

//*************************************************************************************************
// HashTable: HashTableを表すクラス
// 内部ハッシュ法により実装している。
class HashTable {
private:
    mate_t* hash_flush_table_;
    intx* hash_table_;
    intx size_;
    mate_t flush_time_;
public:
    HashTable(intx size);
    ~HashTable();
    void Expand();
    void Set(uintx hash_value, intx value);
    intx Get(uintx hash_value, State* state, const ZDDNode& node,
        const std::vector<ZDDNode>& node_array) const;

    intx GetSize() const
    {
        return size_;
    }

    void Flush()
    {
        ++flush_time_;
    }

private:
    void Initialize();
    void Deinitialize();
};

class PseudoZDD;

//*************************************************************************************************
// FrontierAlgorithm: フロンティア法によるZDDの構築を行うクラス。
// このクラスの Construct メンバ関数を呼び出すことで、アルゴリズムが開始して、
// ZDDが構築される。
class FrontierAlgorithm {
private:
    static const intx INITIAL_HASH_SIZE_ = (1ll << 26);

public:
    static PseudoZDD* Construct(State* state);

private:
    static ZDDNode* MakeChildNode(ZDDNode* node, Mate* mate, State* state,
                                  int child_num, PseudoZDD* zdd);
};

//*************************************************************************************************
// PseudoZDD: フロンティア法によって構築される（既約とは限らない）ZDDを表すクラス。
class PseudoZDD {
private:
    std::vector<ZDDNode> node_array_;
    std::vector<intx> level_first_array_;
    SolutionArray* solution_array_;
    HashTable* global_hash_table_;
    intx lo_node_id_;

    ZDDNode* node_array_pointer_; // for debug

public:
    PseudoZDD();
    ~PseudoZDD();
    ZDDNode* CreateNode();
    void DestructNode(ZDDNode* node, State* state);

    ZDDNode* GetZeroNode()
    {
        return &node_array_[0];
    }

    ZDDNode* GetOneNode()
    {
        return &node_array_[1];
    }

    ZDDNode* GetNode(intx index)
    {
        return &node_array_[index];
    }

    void CreateRootNode();

    void SetHashTable(HashTable* global_hash_table)
    {
        global_hash_table_ = global_hash_table;
    }

    void SetLevelStart()
    {
        level_first_array_.push_back(node_array_.size());
    }

    intx GetCurrentLevelSize() const
    {
        return level_first_array_[level_first_array_.size() - 1]
            - level_first_array_[level_first_array_.size() - 2];
    }

    ZDDNode* GetCurrentLevelNode(intx index);
    intx FindNodeFromNextLevel(const ZDDNode& node, State* state) const;
    void AddNodeToNextLevel(ZDDNode* node, State* state);
    void SetChildNode(ZDDNode* node, ZDDNode* child_node, int child_num);

    intx GetId(ZDDNode* node) const
    {
        intx id = node - &node_array_[0];
        assert (&node_array_[id] == node);
        return id;
    }

    void ReduceAsZDD();
    void OutputZDD(FILE* /*fp*/, bool is_hex) const;
    void OutputZDDForGraphviz(std::ostream& ost, bool is_print_zero) const;
    void OutputZDDForSapporoBDD(std::ostream& ost) const;
    void OutputAllSolutions(FILE* /*fp*/) const;
    void OutputSamplingSolutions(FILE* /*fp*/, int sample_num);

    intx GetNumberOfNodes() const
    {
        return static_cast<intx>(node_array_.size());
    }

    // 引数に与えるリストはソートされていなければならない
    bool Judge(int n, ...);
    bool Judge(const std::vector<int>& sequence);
    void SampleUniformlyRandomly(std::vector<int>* result);

    void ImportZDD(std::istream& ist, bool is_hex);

private:
    void OutputElementRecursively(intx id, std::vector<int>* vec, FILE* fp) const;

public:
    template <typename T>
    T ComputeNumberOfSolutions()
    {
        delete solution_array_;
        SolutionArrayDerive<T>* solution_array = new SolutionArrayDerive<T>;
        solution_array_ = solution_array;
        return solution_array->ComputeNumberOfSolutions(node_array_);
    }

    uintx ComputeNumberOfSolutionsOF()
    {
        delete solution_array_;
        SolutionArrayDerive<uintx>* solution_array = new SolutionArrayDerive<uintx>;
        solution_array_ = solution_array;
        try {
            return solution_array->ComputeNumberOfSolutionsOF(node_array_);
        } catch (...) {
            delete solution_array_;
            solution_array_ = NULL;
            throw;
        }
    }
};

} // the end of the namespace

#endif // PSEUDOZDD_HPP
