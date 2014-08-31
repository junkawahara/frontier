//
// MateSTPathDist.hpp
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

#ifndef MATESTPATHDIST_HPP
#define MATESTPATHDIST_HPP

#include "StateFrontierAux.hpp"
#include "PseudoZDD.hpp"

namespace frontier_dd {

class MateSTPathDist;

struct VarTypeSTPathDist {
    int size;
    double sum;
};

//*************************************************************************************************
// StateSTPathDist: s-t パスのための State
class StateSTPathDist : public StateFrontierAux<mate_t> {
protected:
    int start_vertex_; // 始点 s
    int end_vertex_; // 終点 t
    bool is_hamilton_; // ハミルトンパスに限定するか（true の場合ハミルトンパス）
    bool is_cycle_; // s-t パスではなくてサイクルにするか（true の場合サイクル）
                    // true の場合、start_vertex_ と end_vertex_ は無視される
    double max_distance_;

    //MateSTPathDist* global_mate_; // 処理速度効率化のため、MateSTPathDist オブジェクトを使いまわしする。
                                // アルゴリズムを通して、MateSTPathDist オブジェクトは一度だけ
                                // 作成される。そのオブジェクトを指すポインタ。

public:
    StateSTPathDist(Graph* graph) : StateFrontierAux<mate_t>(graph), max_distance_(99999999)
    {
        byte initial_conf[sizeof(int) + 8];
        *reinterpret_cast<int*>(initial_conf) = 8;
        *reinterpret_cast<double*>(initial_conf + sizeof(int)) = 0.0;
        StateFrontierAux<mate_t>::Initialize(initial_conf, 8);
    }

    virtual ~StateSTPathDist() { }

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

    double GetMaxDistance() const
    {
        return max_distance_;
    }

    void SetMaxDistance(double max_distance)
    {
        max_distance_ = max_distance;
    }

    virtual int GetNextAuxSize();
    virtual void Load(Mate* mate, byte* data);
    virtual void Store(Mate* mate, byte* data);

    virtual Mate* MakeEmptyMate();
    //virtual void PackMate(ZDDNode* node, Mate* mate);
    virtual void UnpackMate(ZDDNode* node, Mate* mate, int child_num);
};

//*************************************************************************************************
// MateSTPathDist
class MateSTPathDist : public MateFrontier<mate_t> {
public:
    double sum_;

public:
    MateSTPathDist(State* state) : MateFrontier<mate_t>(state), sum_(0.0) { }

    virtual void UpdateMate(State* state, int child_num);
    virtual int CheckTerminalPre(State* state, int child_num);
    virtual int CheckTerminalPost(State* state);
};

} // the end of the namespace

#endif // MATESTPATHDIST_HPP
