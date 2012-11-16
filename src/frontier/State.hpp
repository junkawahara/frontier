//
// State.hpp
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

#ifndef STATE_HPP
#define STATE_HPP

#include <vector>
#include <set>
#include <algorithm>
#include <iostream>
#include <cassert>
#include <cstdio>

#include "Global.hpp"
#include "Graph.hpp"
#include "ZDDNode.hpp"

namespace frontier_dd {

class Mate;

//*************************************************************************************************
// State: アルゴリズム動作時の「状態」を表すクラス
class State {
protected:
    Graph* graph_;
    int number_of_vertices_; // # of vertices in graph_
    int number_of_edges_; // # of edges in graph_
    int current_edge_;

    bool is_print_progress_;
    int print_counter_;

public:
    State(Graph* graph) : graph_(graph), current_edge_(-1), is_print_progress_(0),
        print_counter_(0)
    {
        if (graph != NULL) {
            number_of_vertices_ = graph->GetNumberOfVertices();
            number_of_edges_ = graph->GetNumberOfEdges();
        } else {
            number_of_vertices_ = 0;
            number_of_edges_ = 0;
        }
    }

    virtual ~State() { }

    virtual int GetNumberOfVertices()
    {
        return number_of_vertices_;
    }

    virtual int GetNumberOfEdges()
    {
        return number_of_edges_;
    }

    int GetCurrentEdgeNumber()
    {
        return current_edge_;
    }

    Edge GetCurrentEdge()
    {
        return graph_->GetEdgeArray()[current_edge_];
    }

    const std::vector<Edge>& GetEdgeArray() const
    {
        return graph_->GetEdgeArray();
    }

    bool IsLastEdge()
    {
        return current_edge_ >= number_of_edges_ - 1;
    }

    virtual void Update()
    {
        ++current_edge_;
    }

    void SetPrintProgress(bool is_print_progress)
    {
        is_print_progress_ = is_print_progress;
    }

    void PrintNodeNum(intx node_num)
    {
        if (is_print_progress_){
            while (print_counter_ > 0) {
                std::cerr << "\b";
                --print_counter_;
            }

            char buff[256];
            print_counter_ = sprintf(buff, "edge %d: " PERCENT_D, current_edge_, node_num);
            std::cerr << buff;
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

    int GetNumberOfChildren()
    {
        return 2; // Each non-terminal node of BDD/ZDD has 2 children.
    }

    virtual bool Equals(const ZDDNode& node1, const ZDDNode& node2) const = 0;
    virtual intx GetHashValue(const ZDDNode& node) const = 0;
    virtual void PackMate(ZDDNode* node, Mate* mate) = 0;
    virtual Mate* UnpackMate(ZDDNode* node, int child_num) = 0;
    virtual void Undo() = 0;
};

//*************************************************************************************************
// Mate: mate を表すクラス
class Mate {
public:
    virtual ~Mate() { }

    virtual void Update(State* state, int child_num) = 0;
    virtual int CheckTerminalPre(State* state, int child_num) = 0;
    virtual int CheckTerminalPost(State* state) = 0;
};

//*************************************************************************************************
// RBuffer<T>: T型のデータを格納するキュー
template <typename T>
class RBuffer {
private:
    T* buffer_;
    intx head_;
    intx tail_;

    intx capacity_;

public:
    RBuffer() : head_(0), tail_(0), capacity_(1 << 24)
    {
        buffer_ = static_cast<T*>(malloc(capacity_ * sizeof(T)));
        if (buffer_ == NULL) {
            std::cerr << "Error: malloc for buffer_ failed!" << std::endl;
            exit(1);
        }
    }

    ~RBuffer()
    {
        free(buffer_);
    }

    intx GetHeadIndex() const
    {
        return head_;
    }

    intx GetTailIndex() const // for debug
    {
        return tail_;
    }

    T* GetPointer(intx index)
    {
        return &buffer_[index];
    }

    const T* GetPointer(intx index) const
    {
        return &buffer_[index];
    }

    T* GetReadPointer(uintx offset)
    {
        return &buffer_[(tail_ + offset)];
    }

    const T* GetReadPointer(uintx offset) const
    {
        return &buffer_[(tail_ + offset)];
    }

    T* GetWritePointerAndSeekHead(intx size)
    {
        head_ += size;
        if (head_ >= capacity_) {
            capacity_ *= 2;
            buffer_ = static_cast<T*>(realloc(buffer_, capacity_ * sizeof(T)));
            if (buffer_ == NULL) {
                std::cerr << "Error: out of memory!" << std::endl;
                exit(1);
            }
        }
        return &buffer_[head_ - size];
    }

    void BackHead(intx index)
    {
        head_ -= index;
    }

    void SeekTail(uintx offset)
    {
        tail_ += static_cast<intx>(offset);
    }
};

//*************************************************************************************************
// DBuffer: RBuffer のデバッグ用データ構造
template <typename T>
class DBuffer {
private:
    T* buffer_;
    intx head_;
    intx tail_;

    static const int BUFFER_SIZE_ = (1 << 30);

public:
    DBuffer() : head_(0), tail_(0)
    {
        buffer_ = static_cast<T*>(malloc(BUFFER_SIZE_));
        if (buffer_ == NULL) {
            std::cerr << "Error: malloc for buffer_ failed!" << std::endl;
            exit(1);
        }
    }

    ~DBuffer()
    {
        free(buffer_);
    }

    intx GetHeadIndex() const
    {
        return head_;
    }

    intx GetTailIndex() const // for debug
    {
        return tail_;
    }

    T* GetPointer(intx index)
    {
        return &buffer_[index];
    }

    const T* GetPointer(intx index) const
    {
        return &buffer_[index];
    }

    T* GetReadPointer(uintx offset)
    {
        return &buffer_[(tail_ + offset)];
    }

    const T* GetReadPointer(uintx offset) const
    {
        return &buffer_[(tail_ + offset)];
    }

    T* GetWritePointerAndSeekHead(intx size)
    {
        head_ += size;
        return &buffer_[head_ - size];
    }

    void BackHead(intx index)
    {
        head_ -= index;
    }

    void SeekTail(uintx offset)
    {
        tail_ += static_cast<intx>(offset);
    }
};

} // the end of the namespace

#endif // STATE_HPP
