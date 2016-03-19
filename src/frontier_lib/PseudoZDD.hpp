//
// PseudoZDD.hpp
//
// Copyright (c) 2012 -- 2016 Jun Kawahara
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
#include <map>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <cassert>
#include <climits>
#include <iostream>
#include <fstream>

#include "Global.hpp"
#include "State.hpp"
#include "ZDDNode.hpp"
#include "SolutionArray.hpp"
#include "Mate.hpp"
#include "HashTable.hpp"

namespace frontier_lib {

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
    PseudoZDD() : solution_array_(NULL)
    {
        node_array_.reserve(1 << 24);
    
        // create ZeroNode
        node_array_.push_back(ZDDNode());
        //node_array_[0].p.pos_fixed = 0;
        //node_array_[0].p.pos_frontier = -1;
        node_array_[0].n.lo = -1;
        node_array_[0].n.hi = -1;
    
        // create OneNode
        node_array_.push_back(ZDDNode());
        //node_array_[1].p.pos_fixed = -1;
        //node_array_[1].p.pos_frontier = -1;
        node_array_[1].n.lo = -1;
        node_array_[1].n.hi = -1;
    }

    ~PseudoZDD()
    {
        delete solution_array_;
    }

    ZDDNode* CreateNode()
    {
        node_array_.push_back(ZDDNode());
    
        assert(node_array_pointer_ == &node_array_[0]); // for debug
    
        return &node_array_.back();
    }

    void DestructNode(ZDDNode* node, State* state, Mate* mate)
    {
        assert(&node_array_.back() == node);
    
        node_array_.pop_back(); // remove the tail element
        state->Revert(mate);
    }

    ZDDNode* GetZeroTerminal()
    {
        return &node_array_[0];
    }

    ZDDNode* GetOneTerminal()
    {
        return &node_array_[1];
    }

    ZDDNode* GetNode(intx index)
    {
        return &node_array_[index];
    }

    ZDDNode* CreateRootNode()
    {
        node_array_.push_back(ZDDNode());

        assert(node_array_.size() == 3);
        assert(level_first_array_.size() == 0);

        node_array_[2].p.pos_fixed = 0;
        node_array_[2].p.pos_frontier = 0;
        //node_array_[2].n.lo = -1;
        //node_array_[2].n.hi = -1;

        level_first_array_.push_back(2);

        return &node_array_[2];
    }

    intx GetNumberOfNodes() const
    {
        return static_cast<intx>(node_array_.size());
    }

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

    ZDDNode* GetCurrentLevelNode(intx index)
    {
        // expand the inner buffer of std::vector
        if (node_array_.capacity() <= node_array_.size() * 3 / 2) {
            node_array_.reserve(node_array_.capacity() * 2);
        }
    
        node_array_pointer_ = &node_array_[0]; // for debug
    
        return &node_array_[level_first_array_[level_first_array_.size() - 2] + index];
    }

    intx FindNodeFromNextLevel(const ZDDNode& node, State* state, Mate* mate) const
    {
        uintx hash_value = state->GetHashValue(node, mate);
    
        return global_hash_table_->Get(hash_value, state, node, node_array_, mate);
    }

    void AddNodeToNextLevel(ZDDNode* node, State* state, Mate* mate)
    {
        assert(node == &node_array_.back());
    
        if (static_cast<intx>(node_array_.size() - level_first_array_.back())
                >= global_hash_table_->GetSize() / 2) {
            global_hash_table_->Expand();
    
            for (uintx i = level_first_array_.back(); i < node_array_.size(); ++i) {
                global_hash_table_->Set(state->GetHashValue(node_array_[i], mate), i);
            }
            if (state->IsPrintProgress()) {
                state->ErasePrintedNodeNum();
                std::cerr << "Expanding hash." << std::endl;
            }
        }
        global_hash_table_->Set(state->GetHashValue(*node, mate), node_array_.size() - 1);
    }

    intx GetChildNodeId(intx node_id, int child_num)
    {
        return (child_num == 0 ? node_array_[node_id].n.lo : node_array_[node_id].n.hi);
    }

    void SetChildNode(ZDDNode* node, ZDDNode* child_node, int child_num)
    {
        if (child_num == 0) {
            lo_node_id_ = GetId(child_node);
            //node->n.lo = GetId(child_node);
        } else if (child_num == 1) {
            node->n.lo = lo_node_id_;
            node->n.hi = GetId(child_node);
        } else {
            assert(false);
        }
    }

    intx GetId(ZDDNode* node) const
    {
        intx id = node - &node_array_[0];
        assert (&node_array_[id] == node);
        return id;
    }

    int GetLevel(intx node_id)
    {
        if (node_id <= 1) {
            return 0;
        }

        for (uint i = 0; i < level_first_array_.size() - 1; ++i) {
            if (level_first_array_[i] <= node_id && node_id < level_first_array_[i + 1]) {
                return i + 1;
            }
        }
        return static_cast<int>(level_first_array_.size());
    }

    void SwitchArcs()
    {
        for (uintx i = 0; i < node_array_.size(); ++i) {
            std::swap(node_array_[i].n.lo, node_array_[i].n.hi);
        }
    }

    void ReduceAsZDD()
    {
        std::vector<intx> temp_array;
        std::vector<intx> node_count_array;
        std::vector<intx> new_level_array;
    
        temp_array.resize(node_array_.size());
        temp_array[0] = 0; // 0-terminal
        temp_array[1] = 1; // 1-terminal
    
        node_count_array.resize(level_first_array_.size());
        new_level_array.resize(level_first_array_.size());
        new_level_array[0] = 2;
    
        for (int i = static_cast<int>(level_first_array_.size()) - 3; i >= 0; --i) {
            std::map<std::pair<intx, intx>, intx> node_map;
            intx counter = level_first_array_[i];
            for (intx j = level_first_array_[i]; j < level_first_array_[i + 1]; ++j) {
                intx lo = temp_array[node_array_[j].n.lo];
                intx hi = temp_array[node_array_[j].n.hi];
                if (hi == 0) {
                    temp_array[j] = lo;
                } else {
                    std::map<std::pair<intx, intx>, intx>::iterator itor = node_map.find(std::make_pair(lo, hi));
                    if (itor != node_map.end()) { // element found
                        temp_array[j] = (*itor).second;
                    } else {
                        temp_array[j] = counter;
                        node_map.insert(std::make_pair(std::make_pair(lo, hi), counter));
                        node_array_[counter].n.lo = lo;
                        node_array_[counter].n.hi = hi;
                        ++counter;
                    }
                }
            }
            node_count_array[i] = counter - level_first_array_[i];
        }
    
        intx sum = 2;
        for (uint i = 0; i < level_first_array_.size() - 2; ++i) {
            sum += node_count_array[i];
            new_level_array[i + 1] = sum;
        }
        new_level_array[new_level_array.size() - 1] = new_level_array[new_level_array.size() - 2];
    
        std::map<intx, intx> node_map;
        node_map.insert(std::make_pair(0, 0));
        node_map.insert(std::make_pair(1, 1));
    
        for (uint i = 0; i < new_level_array.size() - 2; ++i) {
            for (intx j = new_level_array[i]; j < new_level_array[i + 1]; ++j) {
                node_map.insert(std::make_pair(level_first_array_[i] + j - new_level_array[i], j));
                node_array_[j] = node_array_[level_first_array_[i] + j - new_level_array[i]];
            }
        }
    
        for (uint i = 2; i < node_array_.size(); ++i) {
            node_array_[i].n.lo = node_map[node_array_[i].n.lo];
            node_array_[i].n.hi = node_map[node_array_[i].n.hi];
        }
    
        node_array_.resize(new_level_array.back());
        //std::vector<ZDDNode>().swap(node_array_);
    
        level_first_array_ = new_level_array;
    }

    void OutputZDD(FILE* fp, bool is_hex) const
    {
        if (is_hex) {
            for (uint i = 0; i < level_first_array_.size() - 1 - 1; ++i) { // ignore terminals
                fprintf(fp, "#%d:\n", i + 1);
                for (intx j = level_first_array_[i]; j < level_first_array_[i + 1]; ++j) {
                    fprintf(fp, PERCENT_X ":" PERCENT_X "," PERCENT_X "\n",
                            j, node_array_[j].n.lo, node_array_[j].n.hi);
                }
            }
        } else {
            for (uint i = 0; i < level_first_array_.size() - 1 - 1; ++i) { // ignore terminals
                fprintf(fp, "#%d:\n", i + 1);
                for (intx j = level_first_array_[i]; j < level_first_array_[i + 1]; ++j) {
                    fprintf(fp, PERCENT_D ":" PERCENT_D "," PERCENT_D "\n",
                            j, node_array_[j].n.lo, node_array_[j].n.hi);
                }
            }
        }
    }

    void OutputZDDForGraphviz(std::ostream& ost, bool is_print_zero) const
    {
        ost << "digraph zdd {" << std::endl;
        for (uint i = 0; i < level_first_array_.size() - 1 - 1; ++i) { // ignore terminals
            for (intx j = level_first_array_[i]; j < level_first_array_[i + 1]; ++j) {
                ost << "\tn" << j << " [label = \"" << (i + 1) << ", " << j << "\"];" << std::endl;
                if (is_print_zero || node_array_[j].n.lo != 0) {
                    ost << "\tn" << j << " -> n" << node_array_[j].n.lo
                        << " [style = dashed];" << std::endl;
                }
                if (is_print_zero || node_array_[j].n.hi != 0) {
                    ost << "\tn" << j << " -> n" << node_array_[j].n.hi << ";" << std::endl;
                }
            }
            ost << "\t{rank = same;";
            for (intx j = level_first_array_[i]; j < level_first_array_[i + 1]; ++j) {
                ost << " n" << j << ";";
            }
            ost << "}" << std::endl;
        }
        if (is_print_zero) {
            ost << "\tn0 [shape = box, label = \"0\"];" << std::endl;
        }
        ost << "\tn1 [shape = box, label = \"1\"];" << std::endl;
        ost << "\t{ rank = same;";
        if (is_print_zero) {
            ost << " n0;";
        }
        ost << " n1 }" << std::endl;
        ost << "}" << std::endl;
    }

    void OutputZDDForSapporoBDD(std::ostream& ost) const
    {
        std::vector<bool> negative_array;
        negative_array.resize(node_array_.size());
    
        negative_array[0] = false;
        negative_array[1] = true;
    
        int n = level_first_array_.size() - 1;
    
        ost << "_i " << n << std::endl;
        ost << "_o 1" << std::endl;
        ost << "_n " << (node_array_.size() - 2) << std::endl; // minus 2 for 0/1-terminal
    
        for (int i = static_cast<int>(level_first_array_.size()) - 2 - 1; i >= 0; --i) { // ignore terminals
            for (intx j = level_first_array_[i]; j < level_first_array_[i + 1]; ++j) {
                negative_array[j] = negative_array[node_array_[j].n.lo];
                ost << (j * 2) << " " << (n - i) << " ";
                if (node_array_[j].n.lo <= 1) {
                    ost << "F";
                } else {
                    ost << (node_array_[j].n.lo * 2);
                }
                ost << " ";
                if (node_array_[j].n.hi == 1) {
                    ost << "T";
                } else if (negative_array[node_array_[j].n.hi]) {
                    ost << (node_array_[j].n.hi * 2 + 1);
                } else {
                    ost << (node_array_[j].n.hi * 2);
                }
                ost << std::endl;
            }
        }
        ost << (negative_array[2] ? "5" : "4") << std::endl;
    }

    void OutputAllSolutions(FILE* fp) const
    {
        std::vector<int> vec;
        OutputElementRecursively(2, &vec, fp); // 2 is the first id of the zdd root node.
    }

    void OutputSamplingSolutions(FILE* fp, int sample_num)
    {
        for (int i = 0; i < sample_num; ++i) {
            std::vector<int> vec;
            SampleUniformlyRandomly(&vec);
            for (uint j = 0; j < vec.size(); ++j) {
                fprintf(fp, "%d", vec[j]);
                if (j < vec.size() - 1) {
                    fprintf(fp, " ");
                }
            }
            fprintf(fp, "\n");
        }
    }

    // 引数に与えるリストはソートされていなければならない
    bool Judge(int n, ...)
    {
        va_list ap;
        va_start(ap, n);
    
        std::vector<int> v_array;
    
        for (int i = 0; i < n; ++i) {
            v_array.push_back(va_arg(ap, int));
        }
    
        va_end(ap);
    
        return Judge(v_array);
    }

    bool Judge(const std::vector<int>& sequence)
    {
        int current_level = 0;
        intx current_node = 2;
        uint seq_pos = 0;
    
        while (seq_pos < sequence.size() && current_node >= 2) {
            if (current_level <= sequence[seq_pos]) {
                if (current_level == sequence[seq_pos]) { // match
                    current_node = node_array_[current_node].n.hi;
                    ++seq_pos;
                } else { // not match
                    current_node = node_array_[current_node].n.lo;
                }
                while (current_level < static_cast<int>(level_first_array_.size()) - 1 - 1) { // ignore terminals
                    if (current_node < level_first_array_[current_level + 1]) {
                        break;
                    }
                    ++current_level;
                }
            } else { // current_level > sequence[seq_pos]
                return false;
            }
        }
        while (current_node >= 2) {
            current_node = node_array_[current_node].n.lo;
        }
        return current_node == 1;
    }

    void SampleUniformlyRandomly(std::vector<int>* result)
    {
        if (solution_array_ == NULL) {
            SolutionArrayDerive<double>* solution_array = new SolutionArrayDerive<double>;
            solution_array_ = solution_array;
            solution_array->ComputeNumberOfSolutions(node_array_);
        }
        solution_array_->SampleUniformlyRandomly(node_array_, level_first_array_, result);
    }

    void ImportZDD(std::istream& ist, bool is_hex)
    {
        intx id, lo, hi;
        intx count = 2;
        std::string s;
        node_array_.clear();
    
        // create ZeroNode
        node_array_.push_back(ZDDNode());
        //node_array_[0].p.pos_fixed = 0;
        //node_array_[0].p.pos_frontier = -1;
        node_array_[0].n.lo = -1;
        node_array_[0].n.hi = -1;
    
        // create OneNode
        node_array_.push_back(ZDDNode());
        //node_array_[1].p.pos_fixed = 1;
        //node_array_[1].p.pos_frontier = -1;
        node_array_[1].n.lo = -1;
        node_array_[1].n.hi = -1;
    
        level_first_array_.clear();
    
        while (std::getline(ist, s)) { // skip until first '#'
            if (s[0] == '#') {
                level_first_array_.push_back(count);
                break;
            }
        }
    
        while (std::getline(ist, s)) {
            if (s[0] == '#') {
                level_first_array_.push_back(count);
            } else {
                if (is_hex) {
                    sscanf(s.c_str(), PERCENT_X ":" PERCENT_X "," PERCENT_X, &id, &lo, &hi);
                } else {
                    sscanf(s.c_str(), PERCENT_D ":" PERCENT_D "," PERCENT_D, &id, &lo, &hi);
                }
                assert(id == count);
                ++count;
                node_array_.push_back(ZDDNode());
                node_array_.back().n.lo = lo;
                node_array_.back().n.hi = hi;
            }
        }
        level_first_array_.push_back(count);
    }

private:
    void OutputElementRecursively(intx id, std::vector<int>* vec, FILE* fp) const
    {
        if (id == 0) {
            return;
        }
        if (id == 1) {
            for (uint i = 0; i < vec->size(); ++i) {
                fprintf(fp, "%d", (*vec)[i]);
                if (i < vec->size() - 1) {
                    fprintf(fp, " ");
                }
            }
            fprintf(fp, "\n");
            return;
        } else {
            int top = 0;
    
            for (uint i = 0; i < level_first_array_.size() - 1; ++i) {
                if (level_first_array_[i] <= id && id < level_first_array_[i + 1]) {
                    top = i + 1;
                    break;
                }
            }
    
            vec->push_back(top);
            OutputElementRecursively(node_array_[id].n.hi, vec, fp);
            vec->pop_back();
    
            OutputElementRecursively(node_array_[id].n.lo, vec, fp);
        }
    }

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
