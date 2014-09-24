//
// Graph.hpp
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

#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <vector>
#include <iostream>

#include "GraphInterface.hpp"

namespace frontier_dd {

//*************************************************************************************************
// Edge: グラフの辺を表すクラス
struct Edge {
public:
    int src;
    int dest;
    double weight;
    Edge(int src, int dest);
    Edge(int src, int dest, double weight);
};

//*************************************************************************************************
// Graph: グラフを表すクラス
class Graph : public GraphInterface {
private:
    bool is_directed_;
    bool is_simple_;
    std::vector<Edge> edge_array_;
    std::vector<int> vertex_weight_array_;
    std::vector<std::vector<double> > dist_matrix_;

public:
    const std::vector<Edge>& GetEdgeArray() const
    {
        return edge_array_;
    }

    virtual void LoadAdjacencyList(std::istream& ist, bool is_simple = false,
        bool is_directed = false);
    virtual void LoadAdjacencyMatrix(std::istream& ist, bool is_simple = false,
        bool is_directed = false);
    virtual void LoadIncidenceMatrix(std::istream& ist, bool is_simple = false,
        bool is_directed = false);
    virtual void LoadEdgeList(std::istream& ist, bool is_simple = false,
        bool is_directed = false);

    virtual void SetWeightToEach(std::istream& ist);
    virtual void SetVertexWeight(std::istream& ist);

    virtual int GetVertexWeight(int v)
    {
        return vertex_weight_array_[v];
    }

    virtual int IsUsingVertexWeight()
    {
        return vertex_weight_array_.size() > 0;
    }

    virtual void PrintAdjacencyList(std::ostream& ost) const;
    virtual void PrintAdjacencyMatrix(std::ostream& ost) const;
    virtual void PrintIncidenceMatrix(std::ostream& ost) const;
    virtual void PrintEdgeList(std::ostream& ost) const;
    virtual void PrintForGraphviz(std::ostream& ost) const;
    virtual void PrintForGraphviz(std::ostream& ost,
        const std::vector<int>& bold_edge_list) const;

    void RearrangeByBreadthFirst(int start_vertex);
    void AddDummyVertex();
    void FloydWarshall();
    void PrintDistMatrix() const;
    void RemoveVertices(const std::vector<int>& remove_array,
        const std::vector<int>& new_var_array);

    const std::vector<std::vector<double> >& GetDistMatrix() const
    {
        return dist_matrix_;
    }

private:
    void LoadAdjacencyListUndirected(std::istream& ist);
    void LoadAdjacencyListDirected(std::istream& ist);
};

} // the end of the namespace

#endif // GRAPH_HPP
