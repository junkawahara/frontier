//
// State.hpp
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

#ifndef STATE_HPP
#define STATE_HPP

#include <vector>
#include <set>
#include <algorithm>
#include <string>
#include <iostream>
#include <cassert>
#include <cstdio>

#include "Global.hpp"
#include "Graph.hpp"
#include "ZDDNode.hpp"
#include "RBuffer.hpp"
#include "FrontierManager.hpp"

namespace frontier_lib {

class Mate;
class MateS;
class PseudoZDD;

//*************************************************************************************************
// State: アルゴリズム動作時の「状態」を表すクラス
class State {
protected:
    const Graph* const graph_;
    const int number_of_vertices_; // # of vertices in graph_
    const int number_of_edges_; // # of edges in graph_
    int current_edge_;
    PseudoZDD* subsetting_dd_;

    bool is_print_progress_;
    int print_counter_;

public:
    State(Graph* graph, PseudoZDD* subsetting_dd = NULL) : graph_(graph),
                          number_of_vertices_((graph != NULL) ? graph->GetNumberOfVertices() : 0),
                          number_of_edges_((graph != NULL) ? graph->GetNumberOfEdges() : 0),
                          current_edge_(-1), subsetting_dd_(subsetting_dd), is_print_progress_(false),
                          print_counter_(0) { }

    virtual ~State() { }

    int GetNumberOfVertices() const
    {
        return number_of_vertices_;
    }

    int GetNumberOfEdges() const
    {
        return number_of_edges_;
    }

    int GetCurrentEdgeNumber() const
    {
        return current_edge_;
    }

    const Edge& GetCurrentEdge() const
    {
        return graph_->GetEdge(current_edge_);
    }

    bool IsLastEdge() const
    {
        return current_edge_ >= number_of_edges_ - 1;
    }

    virtual void StartNextEdge()
    {
        ++current_edge_;
    }

    virtual bool IsUsingVertexWeight() const
    {
        return graph_->IsUsingVertexWeight();
    }

    bool IsPrintProgress() const
    {
        return is_print_progress_;
    }

    void SetPrintProgress(bool is_print_progress)
    {
        is_print_progress_ = is_print_progress;
    }

    void PrintNodeNum(intx node_num)
    {
        if (is_print_progress_) {
            while (print_counter_ > 0) {
                std::cerr << "\b";
                --print_counter_;
            }

            char buff[256];
            print_counter_ = sprintf(buff, "edge %d: " PERCENT_D, current_edge_, node_num);
            std::cerr << buff;
        }
    }

    void ErasePrintedNodeNum()
    {
        if (is_print_progress_) {
            while (print_counter_ > 0) {
                std::cerr << "\b";
                --print_counter_;
            }
        }
    }

    void FlushPrint()
    {
        if (is_print_progress_){
            while (print_counter_ > 0) {
                std::cerr << "\b";
                --print_counter_;
            }
        }
    }

    int GetNumberOfChildren() const
    {
        return 2; // Each non-terminal node of BDD/ZDD has 2 children.
    }

    void SetSubsettingDD(PseudoZDD* subsetting_dd)
    {
        subsetting_dd_ = subsetting_dd;
    }

    int DoSubsetting(int child_num, MateS* mate);

    virtual ZDDNode* MakeNewNode(ZDDNode* node, Mate* mate,
                                 int child_num, PseudoZDD* zdd) = 0;
    virtual Mate* Initialize(ZDDNode* root_node) = 0;
    virtual bool Equals(const ZDDNode& node1, const ZDDNode& node2, Mate* mate) const = 0;
    virtual intx GetHashValue(const ZDDNode& node, Mate* mate) const = 0;
    virtual void PackMate(ZDDNode* node, Mate* mate) = 0;
    virtual void UnpackMate(ZDDNode* node, Mate* mate, int child_num) = 0;
    virtual void Revert(Mate* mate) = 0;

    virtual void OutputMultiterminal(Mate* /*mate*/) const { }

    virtual std::string GetString(Mate* /*mate*/, bool /*next*/) const { return std::string(""); }

protected:
    // for StateHyper
    State(int number_of_vertices, int number_of_edges) : graph_(NULL), number_of_vertices_(number_of_vertices),
                                                         number_of_edges_(number_of_edges),
                                                         current_edge_(-1), is_print_progress_(false),
                                                         print_counter_(0) { }
};


} // the end of the namespace

#include "State_impl.hpp"

#endif // STATE_HPP
