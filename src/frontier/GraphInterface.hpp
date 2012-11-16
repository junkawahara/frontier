//
// GraphInterface.hpp
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

#ifndef GRAPHINTERFACE_HPP
#define GRAPHINTERFACE_HPP

#include <iostream>

namespace frontier_dd {

//*************************************************************************************************
// GraphInterface: Graph と HyperGraph の共通部分のインターフェース
class GraphInterface {
protected:
    int number_of_vertices_;
    int number_of_edges_;

public:
    GraphInterface() : number_of_vertices_(0), number_of_edges_(0) { }

    int GetNumberOfVertices() const
    {
        return number_of_vertices_;
    }

    int GetNumberOfEdges() const
    {
        return number_of_edges_;
    }

    virtual void LoadAdjacencyList(std::istream& ist, bool is_simple = false,
        bool is_directed = false) = 0;
    virtual void LoadAdjacencyMatrix(std::istream& ist, bool is_simple = false,
        bool is_directed = false) = 0;
    virtual void LoadIncidenceMatrix(std::istream& ist, bool is_simple = false,
        bool is_directed = false) = 0;
    virtual void LoadEdgeList(std::istream& ist, bool is_simple = false,
        bool is_directed = false) = 0;

    virtual void PrintAdjacencyList(std::ostream& ost) const = 0;
    virtual void PrintAdjacencyMatrix(std::ostream& ost) const = 0;
    virtual void PrintIncidenceMatrix(std::ostream& ost) const = 0;
    virtual void PrintEdgeList(std::ostream& ost) const = 0;
    virtual void PrintForGraphviz(std::ostream& ost) const = 0;
    virtual void PrintForGraphviz(std::ostream& ost,
        const std::vector<int>& bold_edge_list) const = 0;
};

} // the end of the namespace

#endif // GRAPHINTERFACE_HPP
