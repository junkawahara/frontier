//
// HyperGraph.cpp
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

#include <cstdlib>
#include <sstream>

#include "Global.hpp"
#include "HyperGraph.hpp"

namespace frontier_dd {

using namespace std;

void HyperGraph::LoadAdjacencyList(std::istream& , bool , bool )
{
    cerr << "HyperGraph::LoadAdjacencyList is not implemented." << endl;
    exit(1);
}

void HyperGraph::LoadAdjacencyMatrix(std::istream& , bool , bool )
{
    cerr << "HyperGraph::LoadAdjacencyMatrix is not implemented." << endl;
    exit(1);
}

void HyperGraph::LoadIncidenceMatrix(std::istream& ist, bool , bool )
{
    bool is_first = true;
    string s;
    while (std::getline(ist, s)) {
        if (s == "") {
            break;
        }
        HyperEdge edge;
        istringstream iss(s);
        int x;
        edge.var_array.push_back(false); // dummy vertex (index 0)
        while (iss >> x) {
            if (x != 0) {
                edge.var_array.push_back(true);
            } else {
                edge.var_array.push_back(false);
            }
        }
        if (is_first) {
            is_first = false;
            number_of_vertices_ = edge.var_array.size() - 1;
        } else {
            if (static_cast<int>(edge.var_array.size() - 1) != number_of_vertices_) {
                cerr << "input error!" << endl;
                exit(1);
            }
        }
        edge_array_.push_back(edge);
    }
    number_of_edges_ = static_cast<int>(edge_array_.size());
}

void HyperGraph::LoadEdgeList(std::istream& , bool , bool )
{
    cerr << "HyperGraph::LoadEdgeList is not implemented." << endl;
    exit(1);
}

void HyperGraph::SetWeightToEach(istream& ist)
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

void HyperGraph::PrintAdjacencyList(ostream&) const
{
    cerr << "HyperGraph::PrintAdjacencyList is not implemented." << endl;
    exit(1);
}

void HyperGraph::PrintAdjacencyMatrix(ostream&) const
{
    cerr << "HyperGraph::PrintAdjacencyMatrix is not implemented." << endl;
    exit(1);
}

void HyperGraph::PrintIncidenceMatrix(ostream& ost) const
{
    for (uint i = 0; i < edge_array_.size(); ++i) {
        for (uint j = 0; j < edge_array_[i].var_array.size(); ++j) {
            ost << (edge_array_[i].var_array[j] ? "1" : "0") << " ";
        }
        ost << endl;
    }
}

void HyperGraph::PrintEdgeList(ostream&) const
{
    cerr << "HyperGraph::PrintEdgeList is not implemented." << endl;
    exit(1);
}

void HyperGraph::PrintForGraphviz(ostream&) const
{
    cerr << "HyperGraph::PrintForGraphviz is not implemented." << endl;
    exit(1);
}

void HyperGraph::PrintForGraphviz(std::ostream&, const std::vector<int>& ) const
{
    cerr << "HyperGraph::PrintForGraphviz is not implemented." << endl;
    exit(1);
}

} // the end of the namespace
