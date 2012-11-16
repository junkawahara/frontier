//
// StateFrontier.hpp
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

#ifndef STATEFRONTIER_HPP
#define STATEFRONTIER_HPP

#include <vector>
#include <iostream>

#include "Global.hpp"
#include "State.hpp"
#include "ZDDNode.hpp"

namespace frontier_dd {

//*************************************************************************************************
// StateFrontier: State にフロンティア関連の機能を付加したクラス
class StateFrontier : public State {
protected:
    RBuffer<mate_t> mate_buffer_;

    // 現在の辺を処理する直前のフロンティアの頂点番号の配列
    std::vector<int> previous_frontier_array_;
    // 現在の辺を処理する際に新たにフロンティアに加えられる頂点番号の配列
    std::vector<int> entering_frontier_array_;
    // 現在の辺を処理した直後のフロンティアの頂点番号の配列
    std::vector<int> next_frontier_array_;
    // 現在の辺を処理する際にフロンティアから抜ける頂点番号の配列
    std::vector<int> leaving_frontier_array_;

public:
    StateFrontier(Graph* graph) : State(graph) { }

    virtual void Update();

    virtual bool Equals(const ZDDNode& node1, const ZDDNode& node2) const;
    virtual intx GetHashValue(const ZDDNode& node) const;

    virtual void Undo()
    {
        mate_buffer_.BackHead(GetNextFrontierSize());
    }

    void PrintFrontier(std::ostream& ost) const;
    bool IsExistUnprocessedVertex() const;

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

protected:
    void Pack(ZDDNode* node, mate_t* mate);
    void UnpackAndSeek(ZDDNode* node, mate_t* mate);
    void Unpack(ZDDNode* node, mate_t* mate);
    void SeekTail();

private:
    bool Find(int edge_number, int value) const;
};

} // the end of the namespace

#endif // STATEFRONTIER_HPP
