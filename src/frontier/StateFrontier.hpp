//
// StateFrontier.hpp
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

#ifndef STATEFRONTIER_HPP
#define STATEFRONTIER_HPP

#include <vector>
#include <iostream>
#include <cstring> // for memcmp
#include <string>
#include <sstream>

#include "Global.hpp"
#include "State.hpp"
#include "ZDDNode.hpp"

namespace frontier_dd {

//*************************************************************************************************
// FrontierXXX

struct FrontierDeg {
    mate_t deg;
};

struct FrontierComp {
    mate_t comp;
};

struct FrontierDegComp {
    mate_t deg;
    mate_t comp;
};

//*************************************************************************************************
// StateFrontier: State にフロンティア関連の機能を付加したクラス
template<typename T>
class StateFrontier : public State {
protected:
    RBuffer<T> mate_buffer_;

    // 現在の辺を処理する直前のフロンティアの頂点番号の配列
    std::vector<int> previous_frontier_array_;
    // 現在の辺を処理する際に新たにフロンティアに加えられる頂点番号の配列
    std::vector<int> entering_frontier_array_;
    // 現在の辺を処理した直後のフロンティアの頂点番号の配列
    std::vector<int> next_frontier_array_;
    // 現在の辺を処理する際にフロンティアから抜ける頂点番号の配列
    std::vector<int> leaving_frontier_array_;

public:
    StateFrontier(Graph* graph) : State(graph) { }

    virtual void StartNextEdge();

    virtual bool Equals(const ZDDNode& node1, const ZDDNode& node2) const;
    virtual intx GetHashValue(const ZDDNode& node) const;

    virtual void PackMate(ZDDNode* node, Mate* mate);
    virtual void UnpackMate(ZDDNode* node, Mate* mate, int child_num);

    virtual void Revert()
    {
        mate_buffer_.BackHead(GetNextFrontierSize());
    }

    void PrintFrontier(std::ostream& ost) const;
    bool IsExistUnprocessedVertex() const;

    int GetPreviousFrontierSize() const
    {
        return static_cast<int>(previous_frontier_array_.size());
    }

    int GetEnteringFrontierSize() const
    {
        return static_cast<int>(entering_frontier_array_.size());
    }

    int GetNextFrontierSize() const
    {
        return static_cast<int>(next_frontier_array_.size());
    }

    int GetLeavingFrontierSize() const
    {
        return static_cast<int>(leaving_frontier_array_.size());
    }

    int GetPreviousFrontierValue(int index) const
    {
        return previous_frontier_array_[index];
    }

    int GetEnteringFrontierValue(int index) const
    {
        return entering_frontier_array_[index];
    }

    int GetNextFrontierValue(int index) const
    {
        return next_frontier_array_[index];
    }

    int GetLeavingFrontierValue(int index) const
    {
        return leaving_frontier_array_[index];
    }

private:
    bool Find(int edge_number, int value) const;
};

template<typename T>
class MateFrontier : public Mate {
public:
    T* frontier_array;

public:
    MateFrontier<T>(State* state)
    {
        frontier_array = new T[state->GetNumberOfVertices() + 1];
    }

    virtual ~MateFrontier<T>()
    {
        if (frontier_array != NULL) {
            delete[] frontier_array;
        }
    }

    virtual std::string GetPreviousString(State* ) { return std::string(""); }
    virtual std::string GetNextString(State* ) { return std::string(""); }
};

template<> std::string MateFrontier<mate_t>::GetPreviousString(State* state);
template<> std::string MateFrontier<mate_t>::GetNextString(State* state);
template<> std::string MateFrontier<FrontierComp>::GetPreviousString(State* state);
template<> std::string MateFrontier<FrontierComp>::GetNextString(State* state);


template<typename T>
bool StateFrontier<T>::Equals(const ZDDNode& node1, const ZDDNode& node2) const
{
    // フロンティアに含まれる各頂点についてmate値が同じかどうか判定
    const T* p1 = mate_buffer_.GetPointer(node1.p.pos_frontier);
    const T* p2 = mate_buffer_.GetPointer(node2.p.pos_frontier);

    return memcmp(p1, p2, GetNextFrontierSize() * sizeof(T)) == 0;
}

template<typename T>
intx StateFrontier<T>::GetHashValue(const ZDDNode& node) const
{
    uintx hash_value = 0;

    // フロンティアに含まれる各頂点についてmate値が同じかどうか判定
    const byte* p = reinterpret_cast<const byte*>(mate_buffer_.GetPointer(node.p.pos_frontier));
    for (uint i = 0; i < GetNextFrontierSize() * sizeof(T); ++i) {
        hash_value = hash_value * 15284356289ll + p[i];
    }
    return hash_value;
}

template<typename T>
void StateFrontier<T>::PackMate(ZDDNode* node, Mate* mate)
{
    T* frontier_array = static_cast<MateFrontier<T>*>(mate)->frontier_array;

    int frontier_size = GetNextFrontierSize();
    T* p = mate_buffer_.GetWritePointerAndSeekHead(frontier_size);
    for (int i = 0; i < frontier_size; ++i) {
        p[i] = frontier_array[GetNextFrontierValue(i)];
    }
    node->p.pos_frontier = mate_buffer_.GetHeadIndex() - frontier_size;
}

template<typename T>
void StateFrontier<T>::UnpackMate(ZDDNode* , Mate* mate, int child_num)
{
    T* frontier_array = static_cast<MateFrontier<T>*>(mate)->frontier_array;

    T* p = mate_buffer_.GetReadPointer(0);
    for (int i = 0; i < GetPreviousFrontierSize(); ++i) {
        frontier_array[GetPreviousFrontierValue(i)] = p[i];
    }

    if (child_num == 1) {
        mate_buffer_.SeekTail(GetPreviousFrontierSize());
    }
}

template<typename T>
void StateFrontier<T>::PrintFrontier(std::ostream& ost) const
{
    ost << "[";
    for (uint i = 0; i < previous_frontier_array_.size(); ++i) {
        ost << previous_frontier_array_[i];
        if (i < previous_frontier_array_.size() - 1) {
            ost << ", ";
        }
    }
    ost << "], [";
    for (uint i = 0; i < entering_frontier_array_.size(); ++i) {
        ost << entering_frontier_array_[i];
        if (i < entering_frontier_array_.size() - 1) {
            ost << ", ";
        }
    }
    ost << "], [";
    for (uint i = 0; i < next_frontier_array_.size(); ++i) {
        ost << next_frontier_array_[i];
        if (i < next_frontier_array_.size() - 1) {
            ost << ", ";
        }
    }
    ost << "], [";
    for (uint i = 0; i < leaving_frontier_array_.size(); ++i) {
        ost << leaving_frontier_array_[i];
        if (i < leaving_frontier_array_.size() - 1) {
            ost << ", ";
        }
    }
    ost << "]";
}

template<typename T>
void StateFrontier<T>::StartNextEdge()
{
    State::StartNextEdge();

    const std::vector<Edge>& edge_array = graph_->GetEdgeArray();
    int src = edge_array[current_edge_].src;
    int dest = edge_array[current_edge_].dest;

    previous_frontier_array_ = next_frontier_array_;
    entering_frontier_array_.clear();

    if (std::find(next_frontier_array_.begin(),
                  next_frontier_array_.end(), src) == next_frontier_array_.end()) {
        next_frontier_array_.push_back(src);
        entering_frontier_array_.push_back(src);
    }
    if (std::find(next_frontier_array_.begin(),
                  next_frontier_array_.end(), dest) == next_frontier_array_.end()) {
        next_frontier_array_.push_back(dest);
        entering_frontier_array_.push_back(dest);
    }

    leaving_frontier_array_.clear();

    if (!Find(current_edge_, src)) {
        leaving_frontier_array_.push_back(src);
        next_frontier_array_.erase(std::remove(next_frontier_array_.begin(),
                                              next_frontier_array_.end(), src),
                                  next_frontier_array_.end());
    }
    if (!Find(current_edge_, dest)) {
        leaving_frontier_array_.push_back(dest);
        next_frontier_array_.erase(std::remove(next_frontier_array_.begin(),
                                              next_frontier_array_.end(), dest),
                                  next_frontier_array_.end());
    }
}

template<typename T>
bool StateFrontier<T>::IsExistUnprocessedVertex() const
{
    const std::vector<Edge>& edge_array = graph_->GetEdgeArray();
    for (uint i = current_edge_ + 1; i < edge_array.size(); ++i) {
        // not found
        if (std::find(next_frontier_array_.begin(),
                      next_frontier_array_.end(),
                      edge_array[i].src) == next_frontier_array_.end()) {
            return true;
        }
        // not found
        if (std::find(next_frontier_array_.begin(),
                      next_frontier_array_.end(),
                      edge_array[i].dest) == next_frontier_array_.end()) {
            return true;
        }
    }
    return false;
}

template<typename T>
bool StateFrontier<T>::Find(int edge_number, int value) const
{
    const std::vector<Edge>& edge_array = graph_->GetEdgeArray();
    for (uint i = edge_number + 1; i < edge_array.size(); ++i) {
        if (value == edge_array[i].src || value == edge_array[i].dest) {
            return true;
        }
    }
    return false;
}

} // the end of the namespace

#endif // STATEFRONTIER_HPP
