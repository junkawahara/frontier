//
// MateSTPath.hpp
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

#ifndef MATESTPATH_HPP
#define MATESTPATH_HPP

#include "../frontier_lib/StateFrontier.hpp"
#include "../frontier_lib/PseudoZDD.hpp"
#include "../frontier_lib/MatePrinter.hpp"

namespace frontier_lib {

//*************************************************************************************************
// StateSTPath: s-t パスのための State
class StateSTPath : public StateFrontier<MateF<mate_t> > {
protected:
    typedef MateF<mate_t> MateSTPath;

    int start_vertex_; // 始点 s
    int end_vertex_; // 終点 t
    bool is_hamilton_; // ハミルトンパスに限定するか（true の場合ハミルトンパス）
    bool is_cycle_; // s-t パスではなくてサイクルにするか（true の場合サイクル）
                    // true の場合、start_vertex_ と end_vertex_ は無視される
    bool st_entering_frontier_; // s or t がフロンティアに入ったかどうかを表す

public:
    StateSTPath(Graph* graph) : StateFrontier<MateSTPath>(graph)
    {
        start_vertex_ = 1;
        end_vertex_ = graph->GetNumberOfVertices();
        is_hamilton_ = false;
        is_cycle_ = false;
        st_entering_frontier_ = false;
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

    bool STEnteringFrontier()
    {
        return st_entering_frontier_;
    }

    virtual void StartNextEdge()
    {
        StateFrontier<MateSTPath>::StartNextEdge();

        if (!st_entering_frontier_) {
            if (frontier_manager_.FindInEnteringFrontier(start_vertex_)) {
                st_entering_frontier_ = true;
            }
            if (frontier_manager_.FindInEnteringFrontier(end_vertex_)) {
                st_entering_frontier_ = true;
            }
        }
    }

    //virtual void PackMate(ZDDNode* node, Mate* mate);
    virtual void UnpackMate(ZDDNode* node, Mate* mate, int child_num);

    virtual std::string GetString(Mate* mate, bool next) const
    {
        return MatePrinter<mate_t>::GetStringP("val",
            static_cast<MateSTPath*>(mate)->frontier, frontier_manager_, next);
    }

protected:
    virtual void UpdateMate(MateSTPath* mate, int child_num);
    virtual int CheckTerminalPre(MateSTPath* mate, int child_num);
    virtual int CheckTerminalPost(MateSTPath* mate);
};


} // the end of the namespace

#endif // MATESTPATH_HPP
