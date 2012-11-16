//
// StateFrontierHyper.hpp
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

#ifndef STATEFRONTIERHYPER_HPP
#define STATEFRONTIERHYPER_HPP

#include <iostream>

#include "ZDDNode.hpp"
#include "HyperGraph.hpp"
#include "StateFrontier.hpp"

namespace frontier_dd {

//*************************************************************************************************
// StateFrontier: StateFrontier のハイパーグラフ版
class StateFrontierHyper : public StateFrontier {
protected:
    HyperGraph* hgraph_;
    RBuffer<uintx> mate_buffer_;

    int previous_frontier_start_;
    int previous_frontier_width_;
    int current_frontier_start_;
    int current_frontier_width_;
    int next_frontier_start_;
    int next_frontier_width_;

    uintx* current_edge_bits_;
    uintx* leaving_frontier_bits_;

public:
    StateFrontierHyper(HyperGraph* hgraph);

    virtual int GetNumberOfVertices()
    {
        return hgraph_->GetNumberOfVertices();
    }

    virtual int GetNumberOfEdges()
    {
        return static_cast<int>(hgraph_->GetNumberOfEdges());
    }

    HyperEdge GetCurrentEdge()
    {
        return hgraph_->GetEdgeArray()[current_edge_];
    }

    const std::vector<HyperEdge>& GetEdgeArray() const
    {
        return hgraph_->GetEdgeArray();
    }

    uintx GetCurrentEdgeBits(int index)
    {
        return current_edge_bits_[index];
    }

    uintx GetLeavingFrontierBits(int index)
    {
        return leaving_frontier_bits_[index];
    }

    virtual void Update();

    virtual bool Equals(const ZDDNode& node1, const ZDDNode& node2) const;
    virtual intx GetHashValue(const ZDDNode& node) const;

    virtual void Undo()
    {
        mate_buffer_.BackHead(GetNextFrontierWidthWord());
    }

    void Pack(ZDDNode* node, uintx* mate);
    void Unpack(ZDDNode* node, uintx* mate);
    void PrintFrontier(std::ostream& ost) const;

    int GetPreviousFrontierWidthWord() const
    {
        return (previous_frontier_width_ > 0) ?
            (previous_frontier_width_ - 1) / INTX_BITSIZE + 1 : 0;
    }

    int GetCurrentFrontierWidthWord() const
    {
        return (current_frontier_width_ > 0) ?
            (current_frontier_width_ - 1) / INTX_BITSIZE + 1 : 0;
    }

    int GetNextFrontierWidthWord() const
    {
        return (next_frontier_width_ > 0) ?
            (next_frontier_width_ - 1) / INTX_BITSIZE + 1 : 0;
    }

private:
    bool Find(int edge_number, int value) const;
};

} // the end of the namespace

#endif // STATEFRONTIERHYPER_HPP
