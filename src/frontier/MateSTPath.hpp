//
// MateSTPath.hpp
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

#ifndef MATESTPATH_HPP
#define MATESTPATH_HPP

#include "StateFrontier.hpp"
#include "PseudoZDD.hpp"

namespace frontier_dd {

class MateSTPath;

//*************************************************************************************************
// StateSTPath: s-t パスのための State
class StateSTPath : public StateFrontier<mate_t> {
protected:
    int start_vertex_; // 始点 s
    int end_vertex_; // 終点 t
    bool is_hamilton_; // ハミルトンパスに限定するか（true の場合ハミルトンパス）
    bool is_cycle_; // s-t パスではなくてサイクルにするか（true の場合サイクル）
                    // true の場合、start_vertex_ と end_vertex_ は無視される

    //MateSTPath* global_mate_; // 処理速度効率化のため、MateSTPath オブジェクトを使いまわしする。
                                // アルゴリズムを通して、MateSTPath オブジェクトは一度だけ
                                // 作成される。そのオブジェクトを指すポインタ。

public:
    StateSTPath(Graph* graph) : StateFrontier<mate_t>(graph)
    {
        start_vertex_ = 1;
        end_vertex_ = graph->GetNumberOfVertices();
        is_hamilton_ = false;
        is_cycle_ = false;
    }

    virtual ~StateSTPath() { }

    int GetStartVertex() const
    {
        return start_vertex_;
    }

    int GetEndVertex() const
    {
        return end_vertex_;
    }

    void SetStartAndEndVertex(int start_vertex, int end_vertex)
    {
        start_vertex_ = start_vertex;
        end_vertex_ = end_vertex;
    }

    bool IsHamilton() const
    {
        return is_hamilton_;
    }

    void SetHamilton(bool is_hamilton)
    {
        is_hamilton_ = is_hamilton;
    }

    bool IsCycle() const
    {
        return is_cycle_;
    }

    void SetCycle(bool is_cycle)
    {
        is_cycle_ = is_cycle;
    }

    virtual Mate* MakeEmptyMate();
    //virtual void PackMate(ZDDNode* node, Mate* mate);
    virtual void UnpackMate(ZDDNode* node, Mate* mate, int child_num);
};

//*************************************************************************************************
// MateSTPath
class MateSTPath : public MateFrontier<mate_t> {
public:
    MateSTPath(State* state) : MateFrontier<mate_t>(state) { }

    virtual void UpdateMate(State* state, int child_num);
    virtual int CheckTerminalPre(State* state, int child_num);
    virtual int CheckTerminalPost(State* state);
};

} // the end of the namespace

#endif // MATESTPATH_HPP
