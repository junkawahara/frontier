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

#include "StateFrontierFixed.hpp"
#include "MateSTPath.hpp"

namespace frontier_dd {

//*************************************************************************************************
// MateConfSTPathDist
struct MateConfSTPathDist {
    double sum;
};

class MateSTPathDist;

//*************************************************************************************************
// StateSTPathDist: 距離制限付き s-t パスのための State
class StateSTPathDist : public StateFrontierFixed<MateConfSTPathDist> {
protected:
    int start_vertex_;
    int end_vertex_;
    bool is_hamilton_;
    bool is_cycle_;

    double max_distance_;

    MateSTPathDist* global_mate_;

public:
    StateSTPathDist(Graph* graph);
    virtual ~StateSTPathDist();

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

    virtual void PackMate(ZDDNode* node, Mate* mate);
    virtual Mate* UnpackMate(ZDDNode* node, int child_num);
};

//*************************************************************************************************
// MateSTPathDist
class MateSTPathDist : public MateSTPath {
protected:
    MateConfSTPathDist conf_;

public:
    MateSTPathDist(State* state) : MateSTPath(state) { }
    virtual ~MateSTPathDist() { }

    mate_t* GetMateArray()
    {
        return mate_;
    }

    MateConfSTPathDist* GetConf()
    {
        return &conf_;
    }

    virtual void Update(State* state, int child_num);
    virtual int CheckTerminalPre(State* state, int child_num);
    virtual int CheckTerminalPost(State* state);
};

} // the end of the namespace

#endif // MATESTPATHDIST_HPP
