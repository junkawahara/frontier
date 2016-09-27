//
// FrontierComp.hpp
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

#ifndef FRONTIERCOMP_HPP
#define FRONTIERCOMP_HPP

#include "Global.hpp"

namespace frontier_lib {

struct FrontierComp {
    mate_t comp;
};

inline bool operator==(const FrontierComp& f1, const FrontierComp& f2)
{
    return f1.comp == f2.comp;
}

inline bool operator!=(const FrontierComp& f1, const FrontierComp& f2)
{
    return f1.comp != f2.comp;
}

struct MateConfVertexWeight {
    int weight;
    mate_t comp;
};

inline bool operator==(const MateConfVertexWeight& f1, const MateConfVertexWeight& f2)
{
    return f1.weight == f2.weight && f1.comp == f2.comp;
}

inline bool operator!=(const MateConfVertexWeight& f1, const MateConfVertexWeight& f2)
{
    return !(f1 == f2);
}

struct MateConfSingleCComponent {
    mate_t comp;
    byte touched; // 1 if the vertex is touched; 0 if not
};

inline bool operator==(const MateConfSingleCComponent& f1, const MateConfSingleCComponent& f2)
{
    return f1.comp == f2.comp && f1.touched == f2.touched;
}

inline bool operator!=(const MateConfSingleCComponent& f1, const MateConfSingleCComponent& f2)
{
    return !(f1 == f2);
}

} // the end of the namespace

#endif // FRONTIERCOMP_HPP
