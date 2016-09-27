//
// MateDSTPath.hpp
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

#ifndef MATEDSTPATH_HPP
#define MATEDSTPATH_HPP

#include "../frontier_lib/StateFrontier.hpp"
#include "../frontier_lib/PseudoZDD.hpp"
#include "../frontier_lib/MatePrinter.hpp"
#include "StateSTPath.hpp"

namespace frontier_lib {

//*************************************************************************************************
// StateDSTPath: s-t パスのための State
class StateDSTPath : public StateSTPath {
protected:
    typedef MateSTPath MateDSTPath;

public:
    StateDSTPath(Graph* graph) : StateSTPath(graph) { }

    virtual ~StateDSTPath() { }

    //virtual void PackMate(ZDDNode* node, Mate* mate);
    virtual void UnpackMate(ZDDNode* node, Mate* mate, int child_num);

    //virtual std::string GetString(Mate* mate, bool next) const
    //{
    //    return MatePrinter<mate_t>::GetStringP("val",
    //        static_cast<MateDSTPath*>(mate)->frontier, frontier_manager_, next);
    //}

protected:
    virtual void UpdateMate(MateDSTPath* mate, int child_num);
    virtual int CheckTerminalPre(MateDSTPath* mate, int child_num);
    virtual int CheckTerminalPost(MateDSTPath* mate);
};


} // the end of the namespace

#endif // MATEDSTPATH_HPP
