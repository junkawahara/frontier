//
// Graph.cpp
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

#include <sstream>
#include <algorithm>
#include <list>

#include "Global.hpp"
#include "Graph.hpp"

namespace frontier_dd {

using namespace std;

//*************************************************************************************************
// Edge

Edge::Edge(int src, int dest)
{
    this->src = src;
    this->dest = dest;
    this->weight = 1.0;
}

Edge::Edge(int src, int dest, double weight)
{
    this->src = src;
    this->dest = dest;
    this->weight = weight;
}

//*************************************************************************************************
// Graph

void Graph::LoadAdjacencyList(istream& ist, bool is_simple, bool is_directed)
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

void Graph::LoadAdjacencyMatrix(istream&, bool is_simple, bool is_directed)
{
    is_simple_ = is_simple;
    is_directed_ = is_directed;

    cerr << "Loading an adjacency matrix is not implemented." << endl;
    exit(1);
}

void Graph::LoadIncidenceMatrix(istream&, bool is_simple, bool is_directed)
{
    is_simple_ = is_simple;
    is_directed_ = is_directed;

    cerr << "Loading an incidence matrix is not implemented." << endl;
    exit(1);
}

void Graph::LoadEdgeList(istream& ist, bool is_simple, bool is_directed)
{
    is_simple_ = is_simple;
    is_directed_ = is_directed;

    string s;
    std::getline(ist, s);
    istringstream issx(s);
    issx >> number_of_vertices_;

    while (std::getline(ist, s)) {
        istringstream iss(s);
        int src, dest;
        iss >> src >> dest;

        double weight;
        if (!(iss >> weight)) {
            weight = 1.0;
        }
        edge_array_.push_back(Edge(src, dest, weight));
    }
    number_of_edges_ = static_cast<int>(edge_array_.size());
}

void Graph::SetWeightToEach(istream& ist)
{
    double c;
    bool is_end = false;

    for (int i = 0; i < number_of_vertices_; ++i)
    {
        if (is_end) {
            edge_array_[i].weight = 1.0;
        } else {
            if (ist >> c) {
                edge_array_[i].weight = c;
            } else {
                is_end = true;
                edge_array_[i].weight = 1.0;
            }
        }
    }
}

void Graph::PrintAdjacencyList(ostream& ost) const
{
    for (int i = 1; i <= number_of_vertices_; ++i) {
        vector<int> vertex_array;
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
        ost << endl;
    }
}

void Graph::PrintAdjacencyMatrix(std::ostream& ) const
{
    cerr << "Printing an adjacency matrix is not implemented." << endl;
    exit(1);
}

void Graph::PrintIncidenceMatrix(std::ostream& ) const
{
    cerr << "Printing an incidence matrix is not implemented." << endl;
    exit(1);
}

void Graph::PrintEdgeList(ostream& ost) const
{
    ost << number_of_vertices_ << endl;
    for (uint i = 0; i < edge_array_.size(); ++i) {
        ost << edge_array_[i].src << " " << edge_array_[i].dest
            << " " << edge_array_[i].weight << endl;
    }
}

void Graph::PrintForGraphviz(ostream& ost) const
{
    PrintForGraphviz(ost, vector<int>());
}

void Graph::PrintForGraphviz(ostream& ost, const vector<int>& bold_edge_list) const
{
    ost << "graph G {" << endl;
    for (int i = 1; i <= number_of_vertices_; ++i) {
        ost << "\t" << i << ";" << endl;
    }
    for (uint i = 0; i < edge_array_.size(); ++i) {
        ost << "\t" << edge_array_[i].src << " -- " << edge_array_[i].dest
            << " [label=" << (i + 1);
        if (find(bold_edge_list.begin(), bold_edge_list.end(), i + 1) != bold_edge_list.end()) {
            ost << ", color=red, penwidth=5";
        }
        ost << "];" << endl;
    }
    ost << "}" << endl;
}

void Graph::RearrangeByBreadthFirst(int start_vertex)
{
    list<Edge> old_edge_list;
    vector<bool> visited_vertex_array(number_of_vertices_ + 1);

    for (int i = 1; i <= number_of_vertices_; ++i) {
        visited_vertex_array[i] = false;
    }

    for (uint i = 0; i < edge_array_.size(); ++i) {
        old_edge_list.push_back(edge_array_[i]);
    }
    edge_array_.clear();

    list<int> vertex_list;

    vertex_list.push_back(start_vertex);
    visited_vertex_array[start_vertex] = true;
    while (vertex_list.size() > 0) {
        int v = vertex_list.front();
        vertex_list.pop_front();

        vector<int> adjacent_array;

        list<Edge>::iterator itor = old_edge_list.begin();
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
                swap(v0, w);
            }
            edge_array_.push_back(Edge(v0, w));
        }
    }
    if (old_edge_list.size() > 0) {
        cerr << "old_edge_list > 0" << endl;
        exit(1);
    }
    for (uint i = 1; i < visited_vertex_array.size(); ++i) {
        if (!visited_vertex_array[i]) {
            cerr << "visited_vertex_array[" << i << "] == false" << endl;
            exit(1);
        }
    }
}

void Graph::AddDummyVertex()
{
    ++number_of_vertices_;

    list<Edge> new_edge_list;
    for (int i = static_cast<int>(edge_array_.size()) - 1; i >= 0; --i) {
        new_edge_list.push_back(edge_array_[i]);
    }

    for (int v = number_of_vertices_ - 1; v >= 1; --v) {
        list<Edge>::iterator itor = new_edge_list.begin();
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

    list<Edge>::reverse_iterator itor2 = new_edge_list.rbegin();
    while (itor2 != new_edge_list.rend()) {
        edge_array_.push_back(*itor2);
        ++itor2;
    }
}

void Graph::FloydWarshall()
{
    for (int i = 0; i <= number_of_vertices_; ++i) {
        dist_matrix_.push_back(vector<double>());
        vector<double>& ar = dist_matrix_.back();
        for (int j = 0; j <= number_of_vertices_; ++j) {
            if (i == j) {
                ar.push_back(0);
            } else {
                ar.push_back(99999999.0);
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

void Graph::PrintDistMatrix() const
{
    for (int j = 1; j <= number_of_vertices_; ++j) {
        for (int i = 1; i <= number_of_vertices_; ++i) {
            cout << dist_matrix_[j][i] << " ";
        }
        cout << endl;
    }
}

void Graph::RemoveVertices(const vector<int>& remove_array, const vector<int>& new_var_array)
{
    vector<Edge> new_edge_array;
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

/* private */ void Graph::LoadAdjacencyListUndirected(istream& ist)
{
    string s;
    while (std::getline(ist, s)) {
        ++number_of_vertices_;
        istringstream iss(s);
        int x;
        while (iss >> x) {
            Edge edge(number_of_vertices_, x);
            if (number_of_vertices_ > x) {
                std::swap(edge.src, edge.dest);
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

/* private */ void Graph::LoadAdjacencyListDirected(istream& ist)
{
    string s;
    while (std::getline(ist, s)) {
        ++number_of_vertices_;
        istringstream iss(s);
        int x;
        while (iss >> x) {
            edge_array_.push_back(Edge(number_of_vertices_, x));
        }
    }
}

} // the end of the namespace
