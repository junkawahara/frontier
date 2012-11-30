//
// StateFrontierHyper.cpp
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

#include <vector>
#include <algorithm>

#include "StateFrontierHyper.hpp"

namespace frontier_dd {

using namespace std;

StateFrontierHyper::StateFrontierHyper(HyperGraph* hgraph) : StateFrontier(NULL), hgraph_(hgraph),
    next_frontier_start_(1), next_frontier_width_(0)
{
    if (hgraph != NULL) {
        number_of_vertices_ = hgraph->GetNumberOfVertices();
        number_of_edges_ = hgraph->GetNumberOfEdges();
    } else {
        number_of_vertices_ = 0;
        number_of_edges_ = 0;
    }
}

void StateFrontierHyper::Update()
{
    State::Update();

    HyperEdge edge = GetCurrentEdge();

    previous_frontier_array_ = next_frontier_array_;
    previous_frontier_start_ = next_frontier_start_;
    previous_frontier_width_ = next_frontier_width_;

    entering_frontier_array_.clear();

    for (int i = 0; i < 1000; ++i) {
        current_edge_bits_[i] = 0u;
        leaving_frontier_bits_[i] = 0u;
    }

    for (int i = 1; i <= GetNumberOfVertices(); ++i) {
        if (edge.var_array[i]) {
            if (std::find(next_frontier_array_.begin(),
                          next_frontier_array_.end(), i) == next_frontier_array_.end()) {
                next_frontier_array_.push_back(i);
                entering_frontier_array_.push_back(i);
            }
        }
    }

    leaving_frontier_array_.clear();

    for (int i = 1; i <= GetNumberOfVertices(); ++i) {
        if (edge.var_array[i]) {
            if (!Find(current_edge_, i)) {
                leaving_frontier_array_.push_back(i);
                next_frontier_array_.erase(std::remove(next_frontier_array_.begin(),
                                                      next_frontier_array_.end(), i),
                                          next_frontier_array_.end());
            }
        }
    }

    int p_min, p_max, n_min, n_max, e_min, e_max;

    if (previous_frontier_array_.size() > 0) {
        p_min = *min_element(previous_frontier_array_.begin(), previous_frontier_array_.end());
        p_max = *max_element(previous_frontier_array_.begin(), previous_frontier_array_.end());
    } else {
        p_min = 99999999;
        p_max = -1;
    }

    if (entering_frontier_array_.size() > 0) {
        e_min = *min_element(entering_frontier_array_.begin(), entering_frontier_array_.end());
        e_max = *max_element(entering_frontier_array_.begin(), entering_frontier_array_.end());
    } else {
        e_min = 99999999;
        e_max = -1;
    }

    current_frontier_start_ = std::min(p_min, e_min);
    current_frontier_width_ = std::max(p_max, e_max) - current_frontier_start_ + 1;

    if (next_frontier_array_.size() > 0) {
        n_min = *min_element(next_frontier_array_.begin(), next_frontier_array_.end());
        n_max = *max_element(next_frontier_array_.begin(), next_frontier_array_.end());
    } else {
        n_min = current_frontier_start_ + current_frontier_width_;
        n_max = n_min - 1;
    }

    next_frontier_start_ = n_min;
    next_frontier_width_ = n_max - n_min + 1;

    for (uint i = current_frontier_start_; i < edge.var_array.size(); ++i) {
        if (edge.var_array[i]) {
            assert(static_cast<int>(i) - current_frontier_start_ >= 0);
            uint offset = i - current_frontier_start_;
            current_edge_bits_[offset / INTX_BITSIZE] |= (LSBIT << (offset % INTX_BITSIZE));
        }
    }

    for (uint i = 0; i < leaving_frontier_array_.size(); ++i) {
        assert(leaving_frontier_array_[i] - current_frontier_start_ >= 0);
        uint offset = leaving_frontier_array_[i] - current_frontier_start_;
        leaving_frontier_bits_[offset / INTX_BITSIZE] |= (LSBIT << (offset % INTX_BITSIZE));
    }

    // PrintFrontier(cerr);

    // cerr << "# " << previous_frontier_start_ << ", " << previous_frontier_width_ << ", "
    //      << current_frontier_start_ << ", " << current_frontier_width_ << ", "
    //      << next_frontier_start_ << ", " << next_frontier_width_ << endl;

    // for (int i = 0; i < 64; ++i) {
    //     cerr << (((current_edge_bits_[0] & (LSBIT << i)) != 0) ? 1 : 0);
    // }
    // cerr << endl;

    // for (int i = 0; i < 64; ++i) {
    //     cerr << (((leaving_frontier_bits_[0] & (LSBIT << i)) != 0) ? 1 : 0);
    // }
    // cerr << endl;
}

bool StateFrontierHyper::Equals(const ZDDNode& node1, const ZDDNode& node2) const
{
    // フロンティアに含まれる各頂点についてmate値が同じかどうか判定
    const uintx* p1 = mate_buffer_.GetPointer(node1.p.pos_frontier);
    const uintx* p2 = mate_buffer_.GetPointer(node2.p.pos_frontier);
    for (int i = 0; i < GetNextFrontierWidthWord(); ++i) {
        if (p1[i] != p2[i]) {
            return false;
        }
    }
    return true;
}

intx StateFrontierHyper::GetHashValue(const ZDDNode& node) const
{
    uintx hash_value = 0;

    // フロンティアに含まれる各頂点についてmate値が同じかどうか判定
    const uintx* p = mate_buffer_.GetPointer(node.p.pos_frontier);
    for (int i = 0; i < GetNextFrontierWidthWord(); ++i) {
        hash_value = hash_value * 15284356289ll + p[i];
    }
    return hash_value;
}

void StateFrontierHyper::Pack(ZDDNode* node, uintx* mate)
{
    int size = GetNextFrontierWidthWord();
    uintx* p = mate_buffer_.GetWritePointerAndSeekHead(size);

    int offset = next_frontier_start_ - current_frontier_start_;
    assert(offset >= 0);

    for (int i = 0; i < size; ++i) {
        int c = offset / INTX_BITSIZE + i;
        p[i] = (((mate[c] & ~leaving_frontier_bits_[c]) >> (offset % INTX_BITSIZE))
            | ((mate[c + 1] & ~leaving_frontier_bits_[c + 1])
                << (INTX_BITSIZE - offset % INTX_BITSIZE)));
    }
    node->p.pos_frontier = mate_buffer_.GetHeadIndex() - size;
}

void StateFrontierHyper::Unpack(ZDDNode* , uintx* mate)
{
    assert (previous_frontier_start_ == current_frontier_start_);

    uintx* p = mate_buffer_.GetReadPointer(0);
    int size = GetPreviousFrontierWidthWord();
    for (int i = 0; i < size; ++i) {
        mate[i] = p[i];
    }
    mate[size] = 0;
    mate_buffer_.SeekTail(size);
}

void StateFrontierHyper::PrintFrontier(ostream& ost) const
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

bool StateFrontierHyper::Find(int edge_number, int value) const
{
    const vector<HyperEdge>& edge_array = hgraph_->GetEdgeArray();
    for (uint i = edge_number + 1; i < edge_array.size(); ++i) {
        if (edge_array[i].var_array[value]) {
            return true;
        }
    }
    return false;
}

} // the end of the namespace
