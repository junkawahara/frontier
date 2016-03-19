//
// StateKcut.hpp
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

#ifndef STATEKCUT_HPP
#define STATEKCUT_HPP

#include <string>
#include <iostream>

#include "StateSForest.hpp"

namespace frontier_lib {

struct MateConfKcut {
    int number_of_components;
    int cut_weight;

    std::string GetString() const
    {
        std::ostringstream oss;

        oss << "# of comp. = " << number_of_components << ", # of cuts = " << cut_weight;

        return oss.str();
    }
};

//*************************************************************************************************
// StateKcut: 高々 k 本の辺からなるカットのための State
class StateKcut : public StateFrontierComp<MateFD<FrontierComp, MateConfKcut> > {
protected:
    typedef MateFD<FrontierComp, MateConfKcut> MateKcut;
    std::pair<int, int> elimit_;
    std::pair<short, short> component_limit_;

public:
    StateKcut(Graph* graph, std::pair<int, int> edge_weight_limit,
              std::pair<short, short> component_limit) : StateFrontierComp<MateKcut>(graph),
                  elimit_(edge_weight_limit), component_limit_(component_limit) { }

    virtual ~StateKcut() { }

    std::pair<short, short> GetELimit() const
    {
        return elimit_;
    }

    std::pair<short, short> GetComponentLimit() const
    {
        return component_limit_;
    }

    virtual Mate* Initialize(ZDDNode* root_node)
    {
        MateKcut* mate = static_cast<MateKcut*>(StateFrontier<MateKcut>::Initialize(root_node));
        MateConfKcut conf;
        conf.number_of_components = 0;
        conf.cut_weight = 0;
        mate->Initialize(root_node, &conf);
        return mate;
    }

    virtual std::string GetString(Mate* mate, bool next) const
    {
        return MatePrinter<FrontierComp>::GetStringComp("comp",
                   static_cast<MateKcut*>(mate)->frontier, frontier_manager_, next)
            + std::string(", ") + 
                   static_cast<MateKcut*>(mate)->data.GetString();
    }

protected:
    virtual void UpdateMate(MateKcut* mate, int child_num);
    virtual int CheckTerminalPre(MateKcut* mate, int child_num);
    virtual int CheckTerminalPost(MateKcut* mate);
};

} // the end of the namespace

#endif // STATEKCUT_HPP
