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

#include <vector>
#include <iostream>
#include <cstring> // for memcmp

#include "Global.hpp"
#include "HyperGraph.hpp"
#include "StateFrontier.hpp"
#include "ZDDNode.hpp"

namespace frontier_dd {

//*************************************************************************************************
// StateFrontierHyper: xxxxxx
template<typename T>
class StateFrontierHyper : public StateFrontier<T> {
protected:
    HyperGraph* graph_;

public:
    StateFrontierHyper(HyperGraph* hgraph) : StateFrontier<T>(NULL), graph_(hgraph)
    {
        if (hgraph != NULL) {
            State::number_of_vertices_ = hgraph->GetNumberOfVertices();
            State::number_of_edges_ = hgraph->GetNumberOfEdges();
        } else {
            State::number_of_vertices_ = 0;
            State::number_of_edges_ = 0;
        }
    }

    virtual void StartNextEdge();

    const HyperEdge& GetCurrentHyperEdge() const
    {
        return graph_->GetEdgeArray()[State::current_edge_];
    }

    bool IsExistUnprocessedVertex() const;

private:
    bool Find(int edge_number, int value) const;
};

template<typename T>
class MateFrontierHyper : public MateFrontier<T> {
public:
    MateFrontierHyper(State* state) : MateFrontier<T>(state) { }

    virtual std::string GetPreviousString(State* ) { return std::string(""); }
    virtual std::string GetNextString(State* ) { return std::string(""); }
};

template<>
std::string MateFrontierHyper<FrontierComp>::GetPreviousString(State* state);
template<>
std::string MateFrontierHyper<FrontierComp>::GetNextString(State* state);


template<typename T>
void StateFrontierHyper<T>::StartNextEdge()
{
    State::StartNextEdge();

    const std::vector<HyperEdge>& edge_array = graph_->GetEdgeArray();

    const HyperEdge& edge = edge_array[State::current_edge_];

    StateFrontier<T>::previous_frontier_array_ = StateFrontier<T>::next_frontier_array_;
    StateFrontier<T>::entering_frontier_array_.clear();

    for (uint i = 0; i < edge.var_array.size(); ++i) {
        int v = edge.var_array[i];
        if (std::find(StateFrontier<T>::next_frontier_array_.begin(),
                  StateFrontier<T>::next_frontier_array_.end(), v)
                        == StateFrontier<T>::next_frontier_array_.end()) {
            StateFrontier<T>::next_frontier_array_.push_back(v);
            StateFrontier<T>::entering_frontier_array_.push_back(v);
        }
    }

    StateFrontier<T>::leaving_frontier_array_.clear();

    for (uint i = 0; i < edge.var_array.size(); ++i) {
        int v = edge.var_array[i];

        if (!Find(State::current_edge_, v)) {
            StateFrontier<T>::leaving_frontier_array_.push_back(v);
            StateFrontier<T>::next_frontier_array_.erase(
                             std::remove(StateFrontier<T>::next_frontier_array_.begin(),
                             StateFrontier<T>::next_frontier_array_.end(), v),
                             StateFrontier<T>::next_frontier_array_.end());
        }
    }
}

template<typename T>
bool StateFrontierHyper<T>::IsExistUnprocessedVertex() const
{
    const std::vector<HyperEdge>& edge_array = graph_->GetEdgeArray();
    for (uint i = State::current_edge_ + 1; i < edge_array.size(); ++i) {

        for (uint j = 0; j < edge_array[i].var_array.size(); ++j) {
            int v = edge_array[i].var_array[j];
            // not found
            if (std::find(StateFrontier<T>::next_frontier_array_.begin(),
                          StateFrontier<T>::next_frontier_array_.end(),
                          v) == StateFrontier<T>::next_frontier_array_.end()) {
                return true;
            }
        }
    }
    return false;
}

template<typename T>
bool StateFrontierHyper<T>::Find(int edge_number, int value) const
{
    const std::vector<HyperEdge>& edge_array = graph_->GetEdgeArray();
    for (uint i = edge_number + 1; i < edge_array.size(); ++i) {

        for (uint j = 0; j < edge_array[i].var_array.size(); ++j) {
            int v = edge_array[i].var_array[j];
            if (value == v) {
                return true;
            }
        }

    }
    return false;
}


} // the end of the namespace

#endif // STATEFRONTIERHYPER_HPP
