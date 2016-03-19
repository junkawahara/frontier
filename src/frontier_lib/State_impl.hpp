//
// State_impl.hpp
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

#ifndef STATE_IMPL_HPP
#define STATE_IMPL_HPP

#include "PseudoZDD.hpp"
#include "Mate.hpp"

namespace frontier_lib {

inline int State::DoSubsetting(int child_num, MateS* mate)
{
    assert(mate->sdd != 0);

    if (mate->sdd == 1) {
        if (child_num == 1) { // zero suppressed rule
            return 0;
        } else {
            return 1;
        }
    }

    int edge = current_edge_ + 1;
    int level = subsetting_dd_->GetLevel(mate->sdd);

    //std::cerr << "#" << level << ", " << edge << std::endl;

    assert(edge <= level);

    intx c;
    if (edge == level) {
        c = subsetting_dd_->GetChildNodeId(mate->sdd, child_num);
        if (c == 0) {
            return 0;
        }
        mate->sdd = c;
    } else { // edge < level
        if (child_num == 1) { // zero suppressed rule
            return 0;
        }
    }

    return 1;
}


} // the end of the namespace

#endif // STATE_IMPL_HPP
