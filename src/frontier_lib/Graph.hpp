//
// Graph.hpp
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

#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <vector>
#include <list>
#include <algorithm>
#include <iostream>
#include <sstream>

#include "GraphInterface.hpp"

namespace frontier_lib {

//*************************************************************************************************
// Edge: グラフの辺を表すクラス
struct Edge {
public:
    int src;
    int dest;
    int weight;

    Edge(int src, int dest)
    {
        this->src = src;
        this->dest = dest;
        this->weight = 1;
    }

    Edge(int src, int dest, int weight)
    {
        this->src = src;
        this->dest = dest;
        this->weight = weight;
    }
};

//*************************************************************************************************
// Graph: グラフを表すクラス
class Graph : public GraphInterface {
private:
    bool is_directed_;
    bool is_simple_;
    std::vector<Edge> edge_array_;
    std::vector<int> vertex_weight_array_;
    std::vector<std::vector<int> > dist_matrix_;

public:
    Graph() : GraphInterface(false) { }

    const Edge& GetEdge(int index) const
    {
        return edge_array_[index];
    }

    virtual bool IsIncluded(int vertex) const
    {
        for (size_t i = 0; i < edge_array_.size(); ++i) {
            if (edge_array_[i].src == vertex) {
                return true;
            }
            if (edge_array_[i].dest == vertex) {
                return true;
            }
        }
        return false;
    }

    virtual void LoadAdjacencyList(std::istream& ist, bool is_simple = false,
        bool is_directed = false)
    {
        is_simple_ = is_simple;
        is_directed_ = is_directed;

        number_of_vertices_ = 0;

        if (is_directed) { // directed graph
            LoadAdjacencyListDirected(ist);
        } else {
            LoadAdjacencyListUndirected(ist);
        }
        number_of_edges_ = static_cast<int>(edge_array_.size());
    }

    virtual void LoadAdjacencyMatrix(std::istream& /*ist*/, bool is_simple = false,
        bool is_directed = false)
    {
        is_simple_ = is_simple;
        is_directed_ = is_directed;

        std::cerr << "Loading an adjacency matrix is not implemented." << std::endl;
        exit(1);
    }

    virtual void LoadIncidenceMatrix(std::istream& /*ist*/, bool is_simple = false,
        bool is_directed = false)
    {
        is_simple_ = is_simple;
        is_directed_ = is_directed;

        std::cerr << "Loading an incidence matrix is not implemented." << std::endl;
        exit(1);
    }

    virtual void LoadEdgeList(std::istream& ist, bool is_simple = false,
        bool is_directed = false)
    {
        int src, dest;
        int weight;

        number_of_vertices_ = -1;

        is_simple_ = is_simple;
        is_directed_ = is_directed;

        std::string s;
        std::getline(ist, s);
        std::istringstream iss1(s);

        iss1 >> src;

        if (iss1 >> dest) { // The first line is an edge.
            if (!(iss1 >> weight)) {
                weight = 1;
            }
            edge_array_.push_back(Edge(src, dest, weight));
        } else { // The first line is the number of vertices.
            number_of_vertices_ = src;
        }

        while (std::getline(ist, s)) {
            std::istringstream iss2(s);
            iss2 >> src >> dest;

            if (!(iss2 >> weight)) {
                weight = 1;
            }
            edge_array_.push_back(Edge(src, dest, weight));
        }
        number_of_edges_ = static_cast<int>(edge_array_.size());

        if (number_of_vertices_ == -1) { // set the maximum vertex index to number_of_vertices_
            for (uint i = 0; i < edge_array_.size(); ++i) {
                if (edge_array_[i].src > number_of_vertices_) {
                    number_of_vertices_ = edge_array_[i].src;
                }
                if (edge_array_[i].dest > number_of_vertices_) {
                    number_of_vertices_ = edge_array_[i].dest;
                }
            }
        }
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

    virtual void SetVertexWeight(std::istream& ist)
    {
        int c;
        bool is_end = false;

        vertex_weight_array_.clear();
        vertex_weight_array_.push_back(0);

        for (int i = 0; i < number_of_vertices_; ++i) {
            if (is_end) {
                vertex_weight_array_.push_back(1);
            } else {
                if (ist >> c) {
                    vertex_weight_array_.push_back(c);
                } else {
                    is_end = true;
                    vertex_weight_array_.push_back(1);
                }
            }
        }
    }

    virtual int GetVertexWeight(int v) const
    {
        return vertex_weight_array_[v];
    }

    virtual bool IsUsingVertexWeight() const
    {
        return vertex_weight_array_.size() > 0;
    }

    virtual void PrintAdjacencyList(std::ostream& ost) const
    {
        for (int i = 1; i <= number_of_vertices_; ++i) {
            std::vector<int> vertex_array;
            for (uint j = 0; j < edge_array_.size(); ++j) {
                if (edge_array_[j].src == i && i < edge_array_[j].dest) {
                    vertex_array.push_back(edge_array_[j].dest);
                }
                if (edge_array_[j].dest == i && i < edge_array_[j].src) {
                    vertex_array.push_back(edge_array_[j].src);
                }
            }
            sort(vertex_array.begin(), vertex_array.end());
            for (uint j = 0; j < vertex_array.size(); ++j) {
                ost << vertex_array[j];
                if (j < vertex_array.size() - 1) {
                    ost << " ";
                }
            }
            ost << std::endl;
        }
    }

    virtual void PrintAdjacencyMatrix(std::ostream& /*ost*/) const
    {
        std::cerr << "Printing an adjacency matrix is not implemented." << std::endl;
        exit(1);
    }

    virtual void PrintIncidenceMatrix(std::ostream& /*ost*/) const
    {
        std::cerr << "Printing an incidence matrix is not implemented." << std::endl;
        exit(1);
    }

    virtual void PrintEdgeList(std::ostream& ost) const
    {
        ost << number_of_vertices_ << std::endl;
        for (uint i = 0; i < edge_array_.size(); ++i) {
            ost << edge_array_[i].src << " " << edge_array_[i].dest
                << " " << edge_array_[i].weight << std::endl;
        }
    }

    virtual void PrintForGraphviz(std::ostream& ost) const
    {
        PrintForGraphviz(ost, std::vector<int>());
    }

    virtual void PrintForGraphviz(std::ostream& ost,
        const std::vector<int>& bold_edge_list) const
    {
        ost << "graph G {" << std::endl;
        for (int i = 1; i <= number_of_vertices_; ++i) {
            ost << "\t" << i << ";" << std::endl;
        }
        for (uint i = 0; i < edge_array_.size(); ++i) {
            ost << "\t" << edge_array_[i].src << " -- " << edge_array_[i].dest
                << " [label=" << (i + 1);
            if (find(bold_edge_list.begin(), bold_edge_list.end(), i + 1) != bold_edge_list.end()) {
                ost << ", color=red, penwidth=5";
            }
            ost << "];" << std::endl;
        }
        ost << "}" << std::endl;
    }

    void RearrangeByBreadthFirst(int start_vertex)
    {
        std::list<Edge> old_edge_list;
        std::vector<bool> visited_vertex_array(number_of_vertices_ + 1);

        for (int i = 1; i <= number_of_vertices_; ++i) {
            visited_vertex_array[i] = false;
        }

        for (uint i = 0; i < edge_array_.size(); ++i) {
            old_edge_list.push_back(edge_array_[i]);
        }
        edge_array_.clear();

        std::list<int> vertex_list;

        vertex_list.push_back(start_vertex);
        visited_vertex_array[start_vertex] = true;
        while (vertex_list.size() > 0) {
            int v = vertex_list.front();
            vertex_list.pop_front();

            std::vector<int> adjacent_array;

            std::list<Edge>::iterator itor = old_edge_list.begin();
            while (itor != old_edge_list.end()) {
                if (itor->src == v) {
                    adjacent_array.push_back(itor->dest);
                    itor = old_edge_list.erase(itor);
                } else if (itor->dest == v) {
                    adjacent_array.push_back(itor->src);
                    itor = old_edge_list.erase(itor);
                } else {
                    ++itor;
                }
            }

            for (uint i = 0; i < adjacent_array.size(); ++i) {
                int w = adjacent_array[i];
                if (!visited_vertex_array[w]) {
                    visited_vertex_array[w] = true;
                    vertex_list.push_back(w);
                }
                int v0 = v;
                if (v0 > w) {
                    std::swap(v0, w);
                }
                edge_array_.push_back(Edge(v0, w));
            }
        }
        //if (old_edge_list.size() > 0) {
        //    std::cerr << "old_edge_list > 0" << std::endl;
        //    exit(1);
        //}
        //for (uint i = 1; i < visited_vertex_array.size(); ++i) {
        //    if (!visited_vertex_array[i]) {
        //        std::cerr << "visited_vertex_array[" << i << "] == false" << std::endl;
        //        exit(1);
        //    }
        //}
    }

    void AddDummyVertex()
    {
        ++number_of_vertices_;

        std::list<Edge> new_edge_list;
        for (int i = static_cast<int>(edge_array_.size()) - 1; i >= 0; --i) {
            new_edge_list.push_back(edge_array_[i]);
        }

        for (int v = number_of_vertices_ - 1; v >= 1; --v) {
            std::list<Edge>::iterator itor = new_edge_list.begin();
            while (itor != new_edge_list.end()) {
                if (itor->src == v || itor->dest == v) {
                    new_edge_list.insert(itor, Edge(v, number_of_vertices_));
                    break;
                }
                ++itor;
            }
            if (itor == new_edge_list.end()) {
                new_edge_list.insert(itor, Edge(v, number_of_vertices_));
            }
        }

        edge_array_.clear();

        std::list<Edge>::reverse_iterator itor2 = new_edge_list.rbegin();
        while (itor2 != new_edge_list.rend()) {
            edge_array_.push_back(*itor2);
            ++itor2;
        }
    }

    void FloydWarshall()
    {
        for (int i = 0; i <= number_of_vertices_; ++i) {
            dist_matrix_.push_back(std::vector<int>());
            std::vector<int>& ar = dist_matrix_.back();
            for (int j = 0; j <= number_of_vertices_; ++j) {
                if (i == j) {
                    ar.push_back(0);
                } else {
                    ar.push_back(99999999);
                }
            }
        }

        for (unsigned int i = 0; i < edge_array_.size(); ++i) {
            Edge edge = edge_array_[i];
            if (edge.src != edge.dest) {
                dist_matrix_[edge.src][edge.dest] = edge.weight;
                dist_matrix_[edge.dest][edge.src] = edge.weight;
            }
        }

        for (int j = 1; j <= number_of_vertices_; ++j) {
            for (int i = 1; i <= number_of_vertices_; ++i) {
                for (int k = 1; k <= number_of_vertices_; ++k) {
                    if (dist_matrix_[i][k] > dist_matrix_[i][j] + dist_matrix_[j][k]) {
                        dist_matrix_[i][k] = dist_matrix_[i][j] + dist_matrix_[j][k];
                    }
                }
            }
        }
    }

    void PrintDistMatrix() const
    {
        for (int j = 1; j <= number_of_vertices_; ++j) {
            for (int i = 1; i <= number_of_vertices_; ++i) {
                std::cout << dist_matrix_[j][i] << " ";
            }
            std::cout << std::endl;
        }
    }

    void RemoveVertices(const std::vector<int>& remove_array,
        const std::vector<int>& new_var_array)
    {
        std::vector<Edge> new_edge_array;
        for (unsigned int i = 0; i < edge_array_.size(); ++i) {
            if (std::find(remove_array.begin(), remove_array.end(), edge_array_[i].src)
                == remove_array.end()
                && std::find(remove_array.begin(), remove_array.end(), edge_array_[i].dest)
                == remove_array.end()) {
                new_edge_array.push_back(edge_array_[i]);
            }
        }
        edge_array_ = new_edge_array;
        number_of_edges_ = edge_array_.size();
        number_of_vertices_ -= remove_array.size();

        for (unsigned int i = 0; i < edge_array_.size(); ++i) {
            edge_array_[i].src = new_var_array[edge_array_[i].src];
            edge_array_[i].dest = new_var_array[edge_array_[i].dest];
        }
    }

    const std::vector<std::vector<int> >& GetDistMatrix() const
    {
        return dist_matrix_;
    }

private:
    void LoadAdjacencyListUndirected(std::istream& ist)
    {
        std::string s;
        int max_vertex = 0;
        while (std::getline(ist, s)) {
            ++number_of_vertices_;
            std::istringstream iss(s);
            int x;
            while (iss >> x) {
                if (x != number_of_vertices_) { // ignore any self loop
                    Edge edge(number_of_vertices_, x);
                    if (number_of_vertices_ > x) {
                        std::swap(edge.src, edge.dest);
                    }

                    if (max_vertex < x) {
                        max_vertex = x;
                    }

                    uint e;
                    for (e = 0; e < edge_array_.size(); ++e) {
                        if (edge_array_[e].src == edge.src && edge_array_[e].dest == edge.dest) {
                            break;
                        }
                    }
                    if (e >= edge_array_.size()) { // Both src and dest are not found in edge_array_.
                        edge_array_.push_back(edge);
                    }
                }
            }
        }
        // The number of vertices is the largest index that appears in the list.
        if (number_of_vertices_ < max_vertex) {
            number_of_vertices_ = max_vertex;
        }

    }

    void LoadAdjacencyListDirected(std::istream& ist)
    {
        std::string s;
        while (std::getline(ist, s)) {
            ++number_of_vertices_;
            std::istringstream iss(s);
            int x;
            while (iss >> x) {
                edge_array_.push_back(Edge(number_of_vertices_, x));
            }
        }
    }
};

} // the end of the namespace

#endif // GRAPH_HPP
