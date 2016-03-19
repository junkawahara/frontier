//
// MateCombination.hpp
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

#ifndef MATESTATECOMBINATION_HPP
#define MATESTATECOMBINATION_HPP

#include <string>
#include <iostream>
#include <sstream>

#include "../frontier_lib/StateFrontier.hpp"
#include "../frontier_lib/PseudoZDD.hpp"
#include "../frontier_lib/MatePrinter.hpp"

namespace frontier_lib {


//*************************************************************************************************
// StateCombination: s-t パスのための State
class StateCombination : public StateFrontier<MateFD<mate_t, mate_t> > {
protected:
    typedef MateFD<mate_t, mate_t> MateCombination;

    std::pair<int, int> number_of_edges_range_;

public:
    StateCombination(Graph* graph, std::pair<int, int> number_of_edges_range)
        : StateFrontier<MateCombination>(graph),
          number_of_edges_range_(number_of_edges_range) { }

    virtual ~StateCombination() { }

    virtual std::string GetString(Mate* mate, bool /*next*/) const
    {
        std::ostringstream iss;
        iss << "noe = " << static_cast<MateCombination*>(mate)->data;
        if (static_cast<MateS*>(mate)->IsUseSubsetting()) {
            iss << ", sdd = " << static_cast<MateS*>(mate)->sdd;
        }
        return iss.str();
    }

    virtual Mate* Initialize(ZDDNode* root_node)
    {
        MateCombination* mate = static_cast<MateCombination*>(
                   StateFrontier<MateCombination>::Initialize(root_node));
        mate_t initial_value = 0;
        mate->Initialize(root_node, &initial_value);
        return mate;
    }

protected:
    virtual void UpdateMate(MateCombination* mate, int child_num);
    virtual int CheckTerminalPre(MateCombination* mate, int child_num);
    virtual int CheckTerminalPost(MateCombination* mate);
};


} // the end of the namespace

#endif // MATESTATECOMBINATION_HPP
