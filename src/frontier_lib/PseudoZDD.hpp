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

typedef void EnumeratingCallback(const std::vector<int>& vec);

struct EVElem {
public:
    intx node_id;
    int child_num;
    mate_t vertex_num;
    bool touched;

    EVElem(intx node_id_a, int child_num_a, mate_t vertex_num_a, bool touched_a)
    {
        node_id = node_id_a;
        child_num = child_num_a;
        vertex_num = vertex_num_a;
        touched = touched_a;
    }
};

//*************************************************************************************************
// PseudoZDD: フロンティア法によって構築される（既約とは限らない）ZDDを表すクラス。
class PseudoZDD {
public:
    ZDDNode* ZeroTerminal;
    ZDDNode* OneTerminal;
    
private:
    std::vector<std::pair<intx, intx> > node_array_;
    std::vector<intx> level_first_array_;
    SolutionArray* solution_array_;
    HashTable* global_hash_table_;
    intx number_of_nodes_;
    bool hdd_mode_; // output constructed ZDD to a HDD.
    std::string hdd_filename_;
    FILE* fp_;
    intx lo_node_;

    std::vector<EVElem> ev_array_;

    ZDDNode inner_parent_node_;
    ZDDNode inner_child_node_;

public:
    PseudoZDD() : solution_array_(NULL),
                  number_of_nodes_(2), // 0-terminal and 1-terminal
                  hdd_mode_(),
                  fp_(NULL)
    {
        node_array_.reserve(1 << 24);

        // create dummy ZeroNode
        node_array_.push_back(std::make_pair(-1, -1));

        // create dummy OneNode
        node_array_.push_back(std::make_pair(-1, -1));

        ZeroTerminal = new ZDDNode;
        ZeroTerminal->node_number = -10;
        OneTerminal = new ZDDNode;
        OneTerminal->node_number = ZeroTerminal->node_number - 1;
    }

    ~PseudoZDD()
    {
        delete solution_array_;
        delete ZeroTerminal;
        delete OneTerminal;
    }

    ZDDNode* CreateNode()
    {
        inner_child_node_.node_number = number_of_nodes_ - level_first_array_.back();
        ++number_of_nodes_;
        return &inner_child_node_;
    }

    void DestructNode(ZDDNode* /*node*/, State* state, Mate* mate)
    {
        --number_of_nodes_;
        state->Revert(mate);
    }

    ZDDNode* GetNode(intx index)
    {
        inner_child_node_.node_number = index;
        return &inner_child_node_;
    }

    ZDDNode* CreateRootNode()
    {
        level_first_array_.push_back(2);
        ++number_of_nodes_;

        inner_parent_node_.node_number = 0;
        return &inner_parent_node_;
    }

    intx GetNumberOfNodes() const
    {
        return number_of_nodes_;
    }

    void SetHashTable(HashTable* global_hash_table)
    {
        global_hash_table_ = global_hash_table;
    }

    void SetLevelStart()
    {
        if (hdd_mode_) {
            fprintf(fp_, "#" PERCENT_D ":\n", static_cast<intx>(level_first_array_.size()));
        }
        level_first_array_.push_back(number_of_nodes_);
    }

    intx GetCurrentLevelSize() const
    {
        return level_first_array_[level_first_array_.size() - 1]
            - level_first_array_[level_first_array_.size() - 2];
    }

    ZDDNode* GetCurrentLevelNode(intx index)
    {
        inner_parent_node_.node_number = index;
        return &inner_parent_node_;
    }

    intx FindNodeFromNextLevel(const ZDDNode& node, State* state, Mate* mate) const
    {
        uintx hash_value = state->GetHashValue(node, mate);
    
        return global_hash_table_->Get(hash_value, state, node, mate);
    }

    void AddNodeToNextLevel(ZDDNode* node, State* state, Mate* mate)
    {
        if (static_cast<intx>(number_of_nodes_ - 1 - level_first_array_.back())
                >= global_hash_table_->GetSize() / 2) {
            global_hash_table_->Expand();

            ZDDNode pnode;
            for (intx i = 0; i < number_of_nodes_ - 1 - level_first_array_.back(); ++i) {
                pnode.node_number = i;
                global_hash_table_->Set(state->GetHashValue(pnode, mate), i);
            }
            if (state->IsPrintProgress()) {
                state->ErasePrintedNodeNum();
                std::cerr << "Expanding hash." << std::endl;
            }
        }
        global_hash_table_->Set(state->GetHashValue(*node, mate), number_of_nodes_ - 1 - level_first_array_.back());
    }

    intx GetChildNodeId(intx node_id, int child_num)
    {
        return (child_num == 0 ? node_array_[node_id].first : node_array_[node_id].second);
    }

    void SetHddMode(std::string filename)
    {
        std::cerr << "set hdd " << filename << std::endl;
        hdd_filename_ = filename;
        hdd_mode_ = true;
        fp_ = fopen(filename.c_str(), "w");
        if (fp_ == NULL) {
            std::cerr << "error: file " << filename << " cannot be opened." << std::endl;
            exit(1);
        }
    }

    void HddTerminate()
    {
        if (hdd_mode_) {
            fclose(fp_);
        }
    }

    void SetChildNode(ZDDNode* node, ZDDNode* child_node, int child_num)
    {
        if (hdd_mode_) {
            if (child_num == 0) {
                lo_node_ = child_node->node_number;
            } else if (child_num == 1) {
                fprintf(fp_, PERCENT_D ":" PERCENT_D "," PERCENT_D "\n",
                             node->node_number, lo_node_, child_node->node_number);
            } else {
                assert(false);
            }
        } else {
            if (child_num == 0) {
                intx lo = child_node->node_number;
                if (lo >= 0) { // non-terminal
                    lo += level_first_array_.back();
                } else {
                    lo = ZeroTerminal->node_number - lo;
                }
                node_array_.push_back(std::make_pair(lo, -9999));
            } else if (child_num == 1) {
                node_array_.back().second = child_node->node_number;
                if (child_node->node_number >= 0) { // non-terminal
                    node_array_.back().second += level_first_array_.back();
                } else {
                    node_array_.back().second = ZeroTerminal->node_number - child_node->node_number;
                }
            } else {
                assert(false);
            }
        }
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

    int GetMaxLevel() const
    {
        return static_cast<int>(level_first_array_.size()) - 2;
    }

    void SwitchArcs()
    {
        for (uintx i = 0; i < node_array_.size(); ++i) {
            std::swap(node_array_[i].first, node_array_[i].second);
        }
    }

    void SetEV(intx node_id, int child_num, mate_t vertex_num, bool touched)
    {
        ev_array_.push_back(EVElem(level_first_array_[level_first_array_.size() - 2] + node_id, child_num, vertex_num, touched));
        //std::cout << (level_first_array_[level_first_array_.size() - 2] + node_id) << "\t" << child_num << "\t" << vertex_num << "\t" << (touched ? 1 : 0) << std::endl;
    }

    void TranslateToEV()
    {
        std::vector<std::pair<intx, intx> > temp_array;
        temp_array.push_back(std::make_pair(-1, -1)); // for terminals
        temp_array.push_back(std::make_pair(-1, -1));
        std::map<intx, intx> node_map;

        std::vector<intx> vertex_number_array;
        std::vector<std::vector<int> > level_array;
        vertex_number_array.resize(level_first_array_.size() * 2);
        level_array.resize(level_first_array_.size());

        for (size_t i = 0; i < ev_array_.size(); ++i) {
            mate_t v = ev_array_[i].vertex_num;
            if (vertex_number_array[v] == 0) { // first
                int level = GetLevel(ev_array_[i].node_id) - 1;
                level_array[level].push_back(v);
            }
            ++vertex_number_array[v];
        }
        /*for (size_t i = 0; i < level_array.size(); ++i) {
            std::cerr << "i = " << i << "; ";
            for (size_t j = 0; j < level_array[i].size(); ++j) {
                std::cerr << "l[" << i << "][" << j << "] = " << level_array[i][j] << ", ";
            }
            std::cerr << std::endl;
        }*/
        std::vector<intx> node_num_array;
        node_num_array.push_back(2);
        intx node_num = 2;

        for (size_t i = 0; i < level_array.size() - 1; ++i) {
            node_num += level_first_array_[i + 1] - level_first_array_[i];
            node_num_array.push_back(node_num);
            for (size_t j = 0; j < level_array[i].size(); ++j) {
                node_num += vertex_number_array[level_array[i][j]];
                node_num_array.push_back(node_num);
            }
        }
        temp_array.resize(node_num_array.back());
        for (size_t i = 2; i < temp_array.size(); ++i) {
            temp_array[i].first = 0;
            temp_array[i].second = 0;
        }
        //for (size_t i = 0; i < node_num_array.size(); ++i) {
        //    std::cerr << "na[" << i << "] = " << node_num_array[i] << std::endl;
        //}

        intx level_pos = 0;
        intx evelem_pos = 0;
        for (uint i = 0; i < level_first_array_.size() - 2; ++i) {
            intx vn_num = 0;
            for (intx j = level_first_array_[i]; j < level_first_array_[i + 1]; ++j) {
                const std::pair<intx, intx>& original = node_array_[j];
                intx dest_node_id = (j - level_first_array_[i]) + node_num_array[level_pos];
                intx lo = (original.first >= 2 ? ((original.first - level_first_array_[i + 1])
                                                  + node_num_array[level_pos + level_array[i].size() + 1]) : original.first);
                intx hi = (original.second >= 2 ? ((original.second - level_first_array_[i + 1])
                                                   + node_num_array[level_pos + level_array[i].size() + 1]) : original.second);
                //temp_array[dest_node_id].first = lo;
                //temp_array[dest_node_id].second = hi;
                //std::cerr << "*(" << dest_node_id << ", " << lo << ", " << hi << ")" << std::endl;
                int num = 1;
                intx parent = dest_node_id;
                bool lohi = false;
                if (lo > 0) {
                    while (evelem_pos < static_cast<intx>(ev_array_.size()) && ev_array_[evelem_pos].node_id == j
                           && ev_array_[evelem_pos].child_num == 0) {

                        //std::cerr << "lp = " << level_pos << ", num = " << num << ", vn_num = " << vn_num << std::endl;
                        if (lohi) {
                            temp_array[parent].second = node_num_array[level_pos + num] + vn_num;
                        } else {
                            temp_array[parent].first = node_num_array[level_pos + num] + vn_num;
                            //std::cerr << "parent = " << parent << ", val = " << node_num_array[level_pos + num] + vn_num << std::endl;
                        }
                        parent = node_num_array[level_pos + num] + vn_num;
                        lohi = ev_array_[evelem_pos].touched;
                        ++num;
                        ++evelem_pos;
                    }
                    if (lohi) {
                        temp_array[parent].second = lo;
                    } else {
                        temp_array[parent].first = lo;
                    }
                    ++vn_num;
                } else {
                    temp_array[parent].first = lo;
                }
                parent = dest_node_id;
                lohi = true;
                num = 1;
                if (hi > 0) {
                    while (evelem_pos < static_cast<intx>(ev_array_.size()) && ev_array_[evelem_pos].node_id == j
                           && ev_array_[evelem_pos].child_num == 1) {

                        if (lohi) {
                            temp_array[parent].second = node_num_array[level_pos + num] + vn_num;
                        } else {
                            temp_array[parent].first = node_num_array[level_pos + num] + vn_num;
                        }
                        parent = node_num_array[level_pos + num] + vn_num;
                        lohi = ev_array_[evelem_pos].touched;
                        ++num;
                        ++evelem_pos;
                    }
                    if (lohi) {
                        temp_array[parent].second = hi;
                    } else {
                        temp_array[parent].first = hi;
                    }
                    ++vn_num;
                } else {
                    temp_array[parent].second = hi;
                }
            }
            level_pos += level_array[i].size() + 1;
        }
        //for (size_t i = 2; i < temp_array.size(); ++i) {
        //    std::cerr << "(" << i << ", " << temp_array[i].first << ", " << temp_array[i].second << ")" << std::endl;
        //}
        node_array_ = temp_array;
        number_of_nodes_ = temp_array.size();
        level_first_array_ = node_num_array;
    }

    void ReduceAsZDD()
    {
        if (hdd_mode_) { // not implement
            std::cerr << "ReduceAsZDD is not implemented." << std::endl;
            return;
        }
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
                intx lo = temp_array[node_array_[j].first];
                intx hi = temp_array[node_array_[j].second];
                if (hi == 0) {
                    temp_array[j] = lo;
                } else {
                    std::map<std::pair<intx, intx>, intx>::iterator itor = node_map.find(std::make_pair(lo, hi));
                    if (itor != node_map.end()) { // element found
                        temp_array[j] = (*itor).second;
                    } else {
                        temp_array[j] = counter;
                        node_map.insert(std::make_pair(std::make_pair(lo, hi), counter));
                        node_array_[counter].first = lo;
                        node_array_[counter].second = hi;
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
            node_array_[i].first = node_map[node_array_[i].first];
            node_array_[i].second = node_map[node_array_[i].second];
        }

        node_array_.resize(new_level_array.back());
        number_of_nodes_ = node_array_.size();
        //std::vector<ZDDNode>().swap(node_array_);

        level_first_array_ = new_level_array;
    }

    void OutputZDD(FILE* fp, bool is_hex = false) const
    {
        if (is_hex) {
            for (uint i = 0; i < level_first_array_.size() - 1 - 1; ++i) { // ignore terminals
                fprintf(fp, "#%d:\n", i + 1);
                for (intx j = level_first_array_[i]; j < level_first_array_[i + 1]; ++j) {
                    fprintf(fp, PERCENT_X ":" PERCENT_X "," PERCENT_X "\n",
                            j, node_array_[j].first, node_array_[j].second);
                }
            }
        } else {
            for (uint i = 0; i < level_first_array_.size() - 1 - 1; ++i) { // ignore terminals
                fprintf(fp, "#%d:\n", i + 1);
                for (intx j = level_first_array_[i]; j < level_first_array_[i + 1]; ++j) {
                    fprintf(fp, PERCENT_D ":" PERCENT_D "," PERCENT_D "\n",
                            j, node_array_[j].first, node_array_[j].second);
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
                if (is_print_zero || node_array_[j].first != 0) {
                    ost << "\tn" << j << " -> n" << node_array_[j].first
                        << " [style = dashed];" << std::endl;
                }
                if (is_print_zero || node_array_[j].second != 0) {
                    ost << "\tn" << j << " -> n" << node_array_[j].second << ";" << std::endl;
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
                negative_array[j] = negative_array[node_array_[j].first];
                ost << (j * 2) << " " << (n - i) << " ";
                if (node_array_[j].first <= 1) {
                    ost << "F";
                } else {
                    ost << (node_array_[j].first * 2);
                }
                ost << " ";
                if (node_array_[j].second == 1) {
                    ost << "T";
                } else if (negative_array[node_array_[j].second]) {
                    ost << (node_array_[j].second * 2 + 1);
                } else {
                    ost << (node_array_[j].second * 2);
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

    void Enumerate(const std::vector<bool>& use_array, EnumeratingCallback callback) const
    {
        assert(static_cast<intx>(use_array.size()) >= GetMaxLevel());

        std::vector<int> vec;
        Enumerate(2, &vec, use_array, callback);
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
                    current_node = node_array_[current_node].second;
                    ++seq_pos;
                } else { // not match
                    current_node = node_array_[current_node].first;
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
            current_node = node_array_[current_node].first;
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

        // create dummy ZeroNode
        node_array_.push_back(std::make_pair(-1, -1));
    
        // create dummy OneNode
        node_array_.push_back(std::make_pair(-1, -1));
    
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
                node_array_.push_back(std::make_pair(lo, hi));
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
            OutputElementRecursively(node_array_[id].second, vec, fp);
            vec->pop_back();
    
            OutputElementRecursively(node_array_[id].first, vec, fp);
        }
    }

    void Enumerate(intx id, std::vector<int>* vec, const std::vector<bool>& use_array, EnumeratingCallback callback) const
    {
        if (id == 0) {
            return;
        }
        if (id == 1) {
            callback(*vec);
            return;
        } else {
            int top = 0;
    
            for (uint i = 0; i < level_first_array_.size() - 1; ++i) {
                if (level_first_array_[i] <= id && id < level_first_array_[i + 1]) {
                    top = i + 1;
                    break;
                }
            }

            if (use_array[top - 1]) {
                vec->push_back(top);
            }
            Enumerate(node_array_[id].second, vec, use_array, callback);
            if (use_array[top - 1]) {
                vec->pop_back();
            }
    
            Enumerate(node_array_[id].first, vec, use_array, callback);
        }
    }

public:
    template <typename T>
    T ComputeNumberOfSolutions()
    {
        if (hdd_mode_) { // not implement
            std::cerr << "ComputeNumberOfSolutions is not implemented." << std::endl;
            return T();
        }
        delete solution_array_;
        SolutionArrayDerive<T>* solution_array = new SolutionArrayDerive<T>;
        solution_array_ = solution_array;
        return solution_array->ComputeNumberOfSolutions(node_array_);
    }

    uintx ComputeNumberOfSolutionsOF()
    {
        if (hdd_mode_) { // not implement
            std::cerr << "ComputeNumberOfSolutionsOF is not implemented." << std::endl;
            return 0;
        }
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
