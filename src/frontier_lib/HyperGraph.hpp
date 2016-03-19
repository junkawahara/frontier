//
// HyperGraph.hpp
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

#ifndef HYPERGRAPH_HPP
#define HYPERGRAPH_HPP

#include <vector>
#include <cstdlib>
#include <iostream>
#include <sstream>

#include "GraphInterface.hpp"

namespace frontier_lib {

//*************************************************************************************************
// HyperEdge: ハイパーグラフの辺を表すクラス
struct HyperEdge {
public:
    std::vector<int> var_array;
    int weight;
};

//*************************************************************************************************
// HyperGraph: ハイパーグラフを表すクラス
class HyperGraph : public GraphInterface {
private:
    std::vector<HyperEdge> edge_array_;

public:
    HyperGraph() : GraphInterface(true) { }

    const HyperEdge& GetHyperEdge(int index) const
    {
        return edge_array_[index];
    }

    virtual bool IsIncluded(int vertex) const
    {
        for (size_t i = 0; i < edge_array_.size(); ++i) {
            for (size_t j = 0; j < edge_array_[i].var_array.size(); ++j) {
                if (edge_array_[i].var_array[j] == vertex) {
                    return true;
                }
            }
        }
        return false;
    }

    virtual void LoadAdjacencyList(std::istream& /*ist*/, bool /*is_simple*/ = false,
                                   bool /*is_directed*/ = false)
    {
        std::cerr << "HyperGraph::LoadAdjacencyList is not implemented." << std::endl;
        exit(1);
    }

    virtual void LoadAdjacencyMatrix(std::istream& /*ist*/, bool /*is_simple*/ = false,
                                     bool /*is_directed*/ = false)
    {
        std::cerr << "HyperGraph::LoadAdjacencyMatrix is not implemented." << std::endl;
        exit(1);
    }

    virtual void LoadIncidenceMatrix(std::istream& ist, bool /*is_simple*/ = false,
                                     bool /*is_directed*/ = false)
    {
        bool is_first = true;
        std::string s;
        while (std::getline(ist, s)) {
            if (s == "") {
                break;
            }
            HyperEdge edge;
            std::istringstream iss(s);
            int x;
            while (iss >> x) {
                if (x > 0) {
                    edge.var_array.push_back(x);
                }
            }
            if (is_first) {
                is_first = false;
                number_of_vertices_ = edge.var_array.size() - 1;
            } else {
                if (static_cast<int>(edge.var_array.size() - 1) != number_of_vertices_) {
                    std::cerr << "input error!" << std::endl;
                    exit(1);
                }
            }
            edge_array_.push_back(edge);
        }
        number_of_edges_ = static_cast<int>(edge_array_.size());
    }

    virtual void LoadEdgeList(std::istream& ist, bool /*is_simple*/ = false,
                              bool /*is_directed*/ = false)
    {
        std::string s;
    
        std::getline(ist, s);
    
        std::istringstream iss0(s);
    
        iss0 >> number_of_vertices_;
    
        while (std::getline(ist, s)) {
            if (s == "") {
                continue;
            }
            HyperEdge edge;
            std::istringstream iss(s);
            int x;
            while (iss >> x) {
                edge.var_array.push_back(x);
            }
            edge_array_.push_back(edge);
        }
        number_of_edges_ = static_cast<int>(edge_array_.size());
    }

    virtual void SetWeightToEach(std::istream& ist)
    {
        int c;
        bool is_end = false;
    
        for (int i = 0; i < number_of_vertices_; ++i)
        {
            if (is_end) {
                edge_array_[i].weight = 1;
            } else {
                if (ist >> c) {
                    edge_array_[i].weight = c;
                } else {
                    is_end = true;
                    edge_array_[i].weight = 1;
                }
            }
        }
    }

    virtual void SetVertexWeight(std::istream& /*ist*/)
    {
        std::cerr << "HyperGraph::SetVertexWeight is not implemented." << std::endl;
        exit(1);
    }

    virtual bool IsUsingVertexWeight() const
    {
        std::cerr << "HyperGraph::IsUsingVertexWeight is not implemented." << std::endl;
        exit(1);
        return 0;
    }

    virtual void PrintAdjacencyList(std::ostream& /*ost*/) const
    {
        std::cerr << "HyperGraph::PrintAdjacencyList is not implemented." << std::endl;
        exit(1);
    }

    virtual void PrintAdjacencyMatrix(std::ostream& /*ost*/) const
    {
        std::cerr << "HyperGraph::PrintAdjacencyMatrix is not implemented." << std::endl;
        exit(1);
    }

    virtual void PrintIncidenceMatrix(std::ostream& ost) const
    {
        for (uint i = 0; i < edge_array_.size(); ++i) {
            for (uint j = 0; j < edge_array_[i].var_array.size(); ++j) {
                ost << (edge_array_[i].var_array[j] ? "1" : "0") << " ";
            }
            ost << std::endl;
        }
    }

    virtual void PrintEdgeList(std::ostream& /*ost*/) const
    {
        std::cerr << "HyperGraph::PrintEdgeList is not implemented." << std::endl;
        exit(1);
    }

    virtual void PrintForGraphviz(std::ostream& /*ost*/) const
    {
        std::cerr << "HyperGraph::PrintForGraphviz is not implemented." << std::endl;
        exit(1);
    }

    virtual void PrintForGraphviz(std::ostream& /*ost*/,
                                  const std::vector<int>& /*bold_edge_list*/) const
    {
        std::cerr << "HyperGraph::PrintForGraphviz is not implemented." << std::endl;
        exit(1);
    }
};

} // the end of the namespace

#endif // HYPERGRAPH_HPP
