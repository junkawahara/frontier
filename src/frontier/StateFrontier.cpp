//
// StateFrontier.cpp
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

#include "StateFrontier.hpp"
#include "PseudoZDD.hpp"

namespace frontier_dd {

using namespace std;

bool StateFrontier::Equals(const ZDDNode& node1, const ZDDNode& node2) const
{
    // フロンティアに含まれる各頂点についてmate値が同じかどうか判定
    const mate_t* p1 = mate_buffer_.GetPointer(node1.p.pos);
    const mate_t* p2 = mate_buffer_.GetPointer(node2.p.pos);
    for (int i = 0; i < GetNextFrontierSize(); ++i) {
        if (p1[i] != p2[i]) {
            return false;
        }
    }
    return true;
}

intx StateFrontier::GetHashValue(const ZDDNode& node) const
{
    uintx hash_value = 0;

    // フロンティアに含まれる各頂点についてmate値が同じかどうか判定
    const mate_t* p = mate_buffer_.GetPointer(node.p.pos);
    for (int i = 0; i < GetNextFrontierSize(); ++i) {
        hash_value = hash_value * 15284356289ll + p[i];
    }
    return hash_value;
}

void StateFrontier::Pack(ZDDNode* node, mate_t* mate)
{
    int frontier_size = GetNextFrontierSize();
    mate_t* p = mate_buffer_.GetWritePointerAndSeekHead(frontier_size);
    for (int i = 0; i < frontier_size; ++i) {
        p[i] = mate[GetNextFrontierValue(i)];
    }
    node->p.pos = mate_buffer_.GetHeadIndex() - frontier_size;
}

void StateFrontier::UnpackAndSeek(ZDDNode* node, mate_t* mate)
{
    Unpack(node, mate);
    SeekTail();
}

void StateFrontier::Unpack(ZDDNode* , mate_t* mate)
{
    mate_t* p = mate_buffer_.GetReadPointer(0);
    for (int i = 0; i < GetPreviousFrontierSize(); ++i) {
        mate[GetPreviousFrontierValue(i)] = p[i];
    }
    for (int i = 0; i < GetEnteringFrontierSize(); ++i) {
        int v = GetEnteringFrontierValue(i);
        mate[v] = v;
    }
}

void StateFrontier::SeekTail()
{
    mate_buffer_.SeekTail(GetPreviousFrontierSize());
}

void StateFrontier::PrintFrontier(ostream& ost) const
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

void StateFrontier::Update()
{
    State::Update();

    const vector<Edge>& edge_array = graph_->GetEdgeArray();
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

bool StateFrontier::IsExistUnprocessedVertex() const
{
    const vector<Edge>& edge_array = graph_->GetEdgeArray();
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

bool StateFrontier::Find(int edge_number, int value) const
{
    const vector<Edge>& edge_array = graph_->GetEdgeArray();
    for (uint i = edge_number + 1; i < edge_array.size(); ++i) {
        if (value == edge_array[i].src || value == edge_array[i].dest) {
            return true;
        }
    }
    return false;
}

} // the end of the namespace
