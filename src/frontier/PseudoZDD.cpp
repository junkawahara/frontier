//
// PseudoZDD.cpp
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

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <map>

#include "PseudoZDD.hpp"

namespace frontier_dd {

using namespace std;

//*************************************************************************************************
// HashTable

HashTable::HashTable(intx size) : size_(size), flush_time_(1)
{
    Initialize();
}

HashTable::~HashTable()
{
    Deinitialize();
}

/* private */ void HashTable::Initialize()
{
    hash_flush_table_ = (mate_t*)calloc(size_, sizeof(mate_t)); // use calloc for padding 0
    if (hash_flush_table_ == NULL) {
        cerr << "Error: calloc for hash_flush_table_ failed!" << endl;
        exit(1);
    }

    hash_table_ = (intx*)malloc(size_ * sizeof(intx));
    if (hash_table_ == NULL) {
        cerr << "Error: malloc for hash_table_ failed!" << endl;
        exit(1);
    }
}

/* private */ void HashTable::Deinitialize()
{
    free(hash_table_);
    hash_table_ = NULL;
    free(hash_flush_table_);
    hash_flush_table_ = NULL;
}

void HashTable::Expand()
{
    Deinitialize();
    size_ *= 2;
    Initialize();
}

// hash_value を size_ で割った余りをアドレスとして、
// value をテーブルに格納する。
// 衝突が起きたときは、アドレスを+1して次を試す。
void HashTable::Set(uintx hash_value, intx value)
{
    hash_value %= size_;
    for (uintx i = hash_value; i < static_cast<uintx>(size_); ++i) {
        if (hash_flush_table_[i] != flush_time_) {
            hash_table_[i] = value;
            hash_flush_table_[i] = flush_time_;
            return;
        }
    }
    for (uintx i = 0; i < hash_value; ++i) {
        if (hash_flush_table_[i] != flush_time_) {
            hash_table_[i] = value;
            hash_flush_table_[i] = flush_time_;
            return;
        }
    }
    cerr << "Error: the hash is full!" << endl;
    exit(1);
}

intx HashTable::Get(uintx hash_value, State* state, const ZDDNode& node,
    const vector<ZDDNode>& node_array) const
{
    hash_value %= size_;

    while (hash_flush_table_[hash_value] == flush_time_) {
        intx index = hash_table_[hash_value];
        if (state->Equals(node, node_array[index])) {
            return index;
        }
        hash_value = (hash_value + 1) % size_;
    }
    return static_cast<intx>(-1);
}

//*************************************************************************************************
// FrontierAlgorithm

PseudoZDD* FrontierAlgorithm::Construct(State* state)
{
    PseudoZDD* zdd = new PseudoZDD();
    zdd->CreateRootNode(); // 根ノードの作成

    HashTable global_hash_table(INITIAL_HASH_SIZE_); // ノード検索用ハッシュ
    zdd->SetHashTable(&global_hash_table);

    // 各辺について、以下を実行する。
    for (int edge = 1; edge <= state->GetNumberOfEdges(); ++edge) {
        // 「状態」を更新する
        state->Update();

        // 次のレベルのノードがどこから始まるかを記録
        zdd->SetLevelStart();

        // 現在のレベルの各ノードに対するループ
        for (intx i = 0; i < zdd->GetCurrentLevelSize(); ++i) {
            ZDDNode* node = zdd->GetCurrentLevelNode(i);

            // Lo枝とHi枝について処理をする
            //（child_num が 0 のとき Lo枝、child_num が 1 のとき Hi枝についての処理）
            for (int child_num = 0; child_num < state->GetNumberOfChildren(); ++child_num) {
                // 子ノード
                ZDDNode* child_node = MakeChildNode(node, state, child_num, zdd);

                // 終端でないかどうかチェック
                if (child_node != zdd->GetZeroNode() && child_node != zdd->GetOneNode()) {
                    // child_node と「等価な」ノードを調べる
                    intx index = zdd->FindNodeFromNextLevel(*child_node, state);
                    if (index >= 0) { // 等価なノードが存在する
                        // 子ノード (child_node) は新たに作らないので解体
                        zdd->DestructNode(child_node, state);
                        // index 番目のノードとマージ
                        child_node = zdd->GetNode(index);
                    } else { // 等価なノードが存在しない
                        // 次レベルに child_node を加える
                        zdd->AddNodeToNextLevel(child_node, state);
                    }
                }
                zdd->SetChildNode(node, child_node, child_num);
            }
        }
        global_hash_table.Flush();
        state->PrintNodeNum(zdd->GetNumberOfNodes());
    }
    zdd->SetHashTable(NULL);
    return zdd;
}

// Lo枝またはHi枝の先の子ノードを計算
// child_num が 0 なら Lo枝、1 なら Hi枝
/*private*/ ZDDNode* FrontierAlgorithm::MakeChildNode(ZDDNode* node, State* state,
                                                      int child_num, PseudoZDD* zdd)
{
    Mate* mate = state->UnpackMate(node, child_num);

    int c = mate->CheckTerminalPre(state, child_num); // 終端に遷移するか事前にチェック
    if (c == 0) { // 0終端に行くとき
        return zdd->GetZeroNode(); // 0終端を返す
    } else if (c == 1) { // 1終端に行くとき
        return zdd->GetOneNode(); // 1終端を返す
    }

    mate->Update(state, child_num); // mate を更新する

    c = mate->CheckTerminalPost(state); // 終端に遷移するか再度チェック
    if (c == 0) { // 0終端に行くとき
        return zdd->GetZeroNode(); // 0終端を返す
    } else if (c == 1) { // 1終端に行くとき
        return zdd->GetOneNode(); // 1終端を返す
    } else {
        ZDDNode* child_node = zdd->CreateNode();
        state->PackMate(child_node, mate);
        return child_node;
    }
}

//*************************************************************************************************
// PseudoZDD

PseudoZDD::PseudoZDD() : solution_array_(NULL), node_array_pointer_(NULL)
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

PseudoZDD::~PseudoZDD()
{
    delete solution_array_;
}

ZDDNode* PseudoZDD::CreateNode()
{
    node_array_.push_back(ZDDNode());

    assert(node_array_pointer_ == &node_array_[0]); // for debug

    return &node_array_.back();
}

void PseudoZDD::DestructNode(ZDDNode* node, State* state)
{
    assert(&node_array_.back() == node);

    node_array_.pop_back(); // remove the tail element
    state->Undo();
}

void PseudoZDD::CreateRootNode()
{
    node_array_.push_back(ZDDNode());

    assert(node_array_.size() == 3);
    assert(level_first_array_.size() == 0);

    node_array_[2].p.pos_fixed = 0;
    node_array_[2].p.pos_frontier = 0;
    //node_array_[2].n.lo = -1;
    //node_array_[2].n.hi = -1;

    level_first_array_.push_back(2);
}

ZDDNode* PseudoZDD::GetCurrentLevelNode(intx index)
{
    // expand the inner buffer of std::vector
    if (node_array_.capacity() <= node_array_.size() * 3 / 2) {
        node_array_.reserve(node_array_.capacity() * 2);
    }

    node_array_pointer_ = &node_array_[0]; // for debug

    return &node_array_[level_first_array_[level_first_array_.size() - 2] + index];
}

intx PseudoZDD::FindNodeFromNextLevel(const ZDDNode& node, State* state) const
{
    uintx hash_value = state->GetHashValue(node);

    return global_hash_table_->Get(hash_value, state, node, node_array_);
}

void PseudoZDD::AddNodeToNextLevel(ZDDNode* node, State* state)
{
    assert(node == &node_array_.back());

    if (static_cast<intx>(node_array_.size() - level_first_array_.back())
            >= global_hash_table_->GetSize() / 2) {
        global_hash_table_->Expand();

        for (uintx i = level_first_array_.back(); i < node_array_.size(); ++i) {
            global_hash_table_->Set(state->GetHashValue(node_array_[i]), i);
        }
        cerr << "The hash is expanded." << endl;
    }
    global_hash_table_->Set(state->GetHashValue(*node), node_array_.size() - 1);
}

void PseudoZDD::SetChildNode(ZDDNode* node, ZDDNode* child_node, int child_num)
{
    if (child_num == 0) {
        node->n.lo = GetId(child_node);
    } else if (child_num == 1) {
        node->n.hi = GetId(child_node);
    } else {
        assert(false);
    }
}

void PseudoZDD::ReduceAsZDD()
{
    vector<intx> temp_array;
    vector<intx> node_count_array;
    vector<intx> new_level_array;

    temp_array.resize(node_array_.size());
    temp_array[0] = 0; // 0-terminal
    temp_array[1] = 1; // 1-terminal

    node_count_array.resize(level_first_array_.size());
    new_level_array.resize(level_first_array_.size());
    new_level_array[0] = 2;

    for (int i = static_cast<int>(level_first_array_.size()) - 2; i >= 0; --i) {
        map<pair<intx, intx>, intx> node_map;
        intx counter = level_first_array_[i];
        for (intx j = level_first_array_[i]; j < level_first_array_[i + 1]; ++j) {
            intx lo = temp_array[node_array_[j].n.lo];
            intx hi = temp_array[node_array_[j].n.hi];
            if (hi == 0) {
                temp_array[j] = lo;
            } else {
                map<pair<intx, intx>, intx>::iterator itor = node_map.find(make_pair(lo, hi));
                if (itor != node_map.end()) { // element found
                    temp_array[j] = (*itor).second;
                } else {
                    temp_array[j] = counter;
                    node_map.insert(make_pair(make_pair(lo, hi), counter));
                    node_array_[counter].n.lo = lo;
                    node_array_[counter].n.hi = hi;
                    ++counter;
                }
            }
        }
        node_count_array[i] = counter - level_first_array_[i];
    }

    intx sum = 2;
    for (uint i = 0; i < level_first_array_.size() - 1; ++i) {
        sum += node_count_array[i];
        new_level_array[i + 1] = sum;
    }

    map<intx, intx> node_map;
    node_map.insert(make_pair(0, 0));
    node_map.insert(make_pair(1, 1));

    for (uint i = 0; i < new_level_array.size() - 1; ++i) {
        for (intx j = new_level_array[i]; j < new_level_array[i + 1]; ++j) {
            node_map.insert(make_pair(level_first_array_[i] + j - new_level_array[i], j));
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

// I use FILE* pointer instead of cout for efficiency.
void PseudoZDD::OutputZDD(FILE* fp, bool is_hex) const
{
    if (is_hex) {
        for (uint i = 0; i < level_first_array_.size() - 1; ++i) {
            fprintf(fp, "#%d:\n", i + 1);
            for (intx j = level_first_array_[i]; j < level_first_array_[i + 1]; ++j) {
                fprintf(fp, PERCENT_X ":" PERCENT_X "," PERCENT_X "\n",
                        j, node_array_[j].n.lo, node_array_[j].n.hi);
            }
        }
    } else {
        for (uint i = 0; i < level_first_array_.size() - 1; ++i) {
            fprintf(fp, "#%d:\n", i + 1);
            for (intx j = level_first_array_[i]; j < level_first_array_[i + 1]; ++j) {
                fprintf(fp, PERCENT_D ":" PERCENT_D "," PERCENT_D "\n",
                        j, node_array_[j].n.lo, node_array_[j].n.hi);
            }
        }
    }
}

void PseudoZDD::OutputZDDForGraphviz(ostream& ost, bool is_print_zero) const
{
    ost << "digraph zdd {" << endl;
    for (uint i = 0; i < level_first_array_.size() - 1; ++i) {
        for (intx j = level_first_array_[i]; j < level_first_array_[i + 1]; ++j) {
            ost << "\tn" << j << " [label = \"" << (i + 1) << ", " << j << "\"];" << endl;
            if (is_print_zero || node_array_[j].n.lo != 0) {
                ost << "\tn" << j << " -> n" << node_array_[j].n.lo
                    << " [style = dashed];" << endl;
            }
            if (is_print_zero || node_array_[j].n.hi != 0) {
                ost << "\tn" << j << " -> n" << node_array_[j].n.hi << ";" << endl;
            }
        }
        ost << "\t{rank = same;";
        for (intx j = level_first_array_[i]; j < level_first_array_[i + 1]; ++j) {
            ost << " n" << j << ";";
        }
        ost << "}" << endl;
    }
    if (is_print_zero) {
        ost << "\tn0 [shape = box, label = \"0\"];" << endl;
    }
    ost << "\tn1 [shape = box, label = \"1\"];" << endl;
    ost << "\t{ rank = same;";
    if (is_print_zero) {
        ost << " n0;";
    }
    ost << " n1 }" << endl;
    ost << "}" << endl;
}

void PseudoZDD::OutputZDDForSapporoBDD(ostream& ost) const
{
    vector<bool> negative_array;
    negative_array.resize(node_array_.size());

    negative_array[0] = false;
    negative_array[1] = true;

    int n = level_first_array_.size() - 1;

    ost << "_i " << n << endl;
    ost << "_o 1" << endl;
    ost << "_n " << (node_array_.size() - 2) << endl; // minus 2 for 0/1-terminal

    for (int i = static_cast<int>(level_first_array_.size()) - 2; i >= 0; --i) {
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
            ost << endl;
        }
    }
    ost << (negative_array[2] ? "5" : "4") << endl;
}

// OutputAllSolutions の中で呼ばれる
/* private */void PseudoZDD::OutputElementRecursively(intx id, vector<int>* vec, FILE* fp) const
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

// 全解を出力
void PseudoZDD::OutputAllSolutions(FILE* fp) const
{
    vector<int> vec;
    OutputElementRecursively(2, &vec, fp); // 2 is the first id of the zdd root node.
}

// 一様ランダムサンプリングを sample_num 回行う
void PseudoZDD::OutputSamplingSolutions(FILE* fp, int sample_num)
{
    for (int i = 0; i < sample_num; ++i) {
        vector<int> vec;
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
bool PseudoZDD::Judge(int n, ...)
{
    va_list ap;
    va_start(ap, n);

    vector<int> v_array;

    for (int i = 0; i < n; ++i) {
        v_array.push_back(va_arg(ap, int));
    }

    va_end(ap);

    return Judge(v_array);
}

// sequence に与えるリストはソートされていなければならない
bool PseudoZDD::Judge(const vector<int>& sequence)
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
            while (current_level < static_cast<int>(level_first_array_.size()) - 1) {
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

void PseudoZDD::SampleUniformlyRandomly(vector<int>* result)
{
    if (solution_array_ == NULL) {
        SolutionArrayDerive<double>* solution_array = new SolutionArrayDerive<double>;
        solution_array_ = solution_array;
        solution_array->ComputeNumberOfSolutions(node_array_);
    }
    solution_array_->SampleUniformlyRandomly(node_array_, level_first_array_, result);
}

void PseudoZDD::ImportZDD(istream& ist, bool is_hex)
{
    intx id, lo, hi;
    intx count = 2;
    string s;
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

} // the end of the namespace
