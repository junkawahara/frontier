//
// FrontierManager.hpp
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

#ifndef FRONTIERMANAGER_HPP
#define FRONTIERMANAGER_HPP

#include <vector>
#include <algorithm>
#include <iostream>
#include <cstring> // for memcmp
#include <string>
#include <sstream>

#include "Graph.hpp"
#include "HyperGraph.hpp"

namespace frontier_lib {

//*************************************************************************************************
// FrontierManager: State にフロンティア関連の機能を付加したクラス
class FrontierManager {
private:
    // 現在の辺を処理する直前のフロンティアの頂点番号の配列
    std::vector<int> previous_frontier_array_;
    // 現在の辺を処理する際に新たにフロンティアに加えられる頂点番号の配列
    std::vector<int> entering_frontier_array_;
    // 現在の辺を処理した直後のフロンティアの頂点番号の配列
    std::vector<int> next_frontier_array_;
    // 現在の辺を処理する際にフロンティアから抜ける頂点番号の配列
    std::vector<int> leaving_frontier_array_;

    std::vector<int> both_frontier_array_;

    const Graph* const graph_;
    const HyperGraph* const hgraph_;
    const GraphInterface* const igraph_;

public:
    FrontierManager(const Graph* graph) : graph_(graph), hgraph_(NULL), igraph_(graph) { }

    FrontierManager(const HyperGraph* hgraph) : graph_(NULL), hgraph_(hgraph), igraph_(hgraph) { }

    void PrintFrontier(std::ostream& ost) const
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

    void Update(const Edge& current_edge, int current_edge_num)
    {
        std::vector<int> vertex_list;
        vertex_list.push_back(current_edge.src);
        vertex_list.push_back(current_edge.dest);

        Update(vertex_list, current_edge_num);
    }

    void Update(const HyperEdge& current_edge, int current_edge_num)
    {
        Update(current_edge.var_array, current_edge_num);
    }

    void Update(const std::vector<int>& vertex_list, int current_edge_num)
    {
        previous_frontier_array_ = next_frontier_array_;
        both_frontier_array_ = previous_frontier_array_;
        entering_frontier_array_.clear();

        for (uint i = 0; i < vertex_list.size(); ++i) {
            if (std::find(next_frontier_array_.begin(),
                          next_frontier_array_.end(), vertex_list[i]) == next_frontier_array_.end()) {
                next_frontier_array_.push_back(vertex_list[i]);
                both_frontier_array_.push_back(vertex_list[i]);
                entering_frontier_array_.push_back(vertex_list[i]);
            }
        }

        leaving_frontier_array_.clear();

        for (uint i = 0; i < vertex_list.size(); ++i) {
            if (!Find(current_edge_num, vertex_list[i])) {
                leaving_frontier_array_.push_back(vertex_list[i]);
                next_frontier_array_.erase(std::remove(next_frontier_array_.begin(),
                                                       next_frontier_array_.end(), vertex_list[i]),
                                           next_frontier_array_.end());
            }
        }
    }

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

    int GetBothFrontierSize() const
    {
        return static_cast<int>(both_frontier_array_.size());
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

    int GetBothFrontierValue(int index) const
    {
        return both_frontier_array_[index];
    }

    bool FindInEnteringFrontier(int element)
    {
        return std::find(entering_frontier_array_.begin(),
                  entering_frontier_array_.end(), element) != entering_frontier_array_.end();
    }

    bool IsExistUnprocessedVertex(int current_edge_num) const
    {
        if (igraph_->IsHyperGraph()) {
            for (int i = current_edge_num + 1; i < igraph_->GetNumberOfEdges(); ++i) {
                const HyperEdge& edge = hgraph_->GetHyperEdge(i);
                // not found
                for (uint j = 0; j < edge.var_array.size(); ++j) {
                    if (std::find(next_frontier_array_.begin(),
                                  next_frontier_array_.end(),
                                  edge.var_array[j]) == next_frontier_array_.end()) {
                        return true;
                    }
                }
            }
        } else {
            for (int i = current_edge_num + 1; i < igraph_->GetNumberOfEdges(); ++i) {
                const Edge& edge = graph_->GetEdge(i);
                // not found
                if (std::find(next_frontier_array_.begin(),
                              next_frontier_array_.end(),
                              edge.src) == next_frontier_array_.end()) {
                    return true;
                }
                // not found
                if (std::find(next_frontier_array_.begin(),
                              next_frontier_array_.end(),
                              edge.dest) == next_frontier_array_.end()) {
                    return true;
                }
            }
        }
        return false;
    }


private:
    bool Find(int edge_number, int value) const
    {
        if (igraph_->IsHyperGraph()) {
            for (int i = edge_number + 1; i < igraph_->GetNumberOfEdges(); ++i) {
                const HyperEdge& edge = hgraph_->GetHyperEdge(i);
                for (uint j = 0; j < edge.var_array.size(); ++j) {
                    if (value == edge.var_array[j]) {
                        return true;
                    }
                }
            }
        } else {
            for (int i = edge_number + 1; i < igraph_->GetNumberOfEdges(); ++i) {
                const Edge& edge = graph_->GetEdge(i);
                if (value == edge.src || value == edge.dest) {
                    return true;
                }
            }
        }
        return false;
    }
};

} // the end of the namespace

#endif // FRONTIERMANAGER_HPP
