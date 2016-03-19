//
// StateRcut.hpp
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

#ifndef STATERCUT_HPP
#define STATERCUT_HPP

#include <string>
#include <iostream>

#include "StateSForest.hpp"
#include "../frontier_lib/RootManager.hpp"

namespace frontier_lib {

//*************************************************************************************************
// MateConfRcut
struct MateConfRcut {
    int cut_weight;

    std::string GetString()
    {
        std::ostringstream oss;

        oss << "cut weight = " << cut_weight;

        return oss.str();
    }
};

//*************************************************************************************************
// StateRcut: 根を指定したカットのための State
class StateRcut : public StateFrontierComp<MateFD<FrontierComp, MateConfRcut> > {
    typedef MateFD<FrontierComp, MateConfRcut> MateRcut;
protected:
    RootManager* root_mgr_;
    std::pair<int, int> elimit_;

public:
    StateRcut(Graph* graph, std::pair<int, int> edge_weight_limit) : StateFrontierComp<MateRcut>(graph),
                                                                     elimit_(edge_weight_limit) { }
    virtual ~StateRcut() { }

    const RootManager* GetRootManager() const
    {
        return root_mgr_;
    }

    void SetRootManager(RootManager* root_mgr)
    {
        root_mgr_ = root_mgr;
    }

    std::pair<short, short> GetELimit() const
    {
        return elimit_;
    }

    virtual Mate* Initialize(ZDDNode* root_node)
    {
        MateRcut* mate = static_cast<MateRcut*>(StateFrontier<MateRcut>::Initialize(root_node));
        MateConfRcut conf;
        conf.cut_weight = 0;
        mate->Initialize(root_node, &conf);
        return mate;
    }

    virtual std::string GetString(Mate* mate, bool next) const
    {
        return MatePrinter<FrontierComp>::GetStringComp("comp",
                   static_cast<MateRcut*>(mate)->frontier, frontier_manager_, next)
            + std::string(", ") + 
                   static_cast<MateRcut*>(mate)->data.GetString();
    }

protected:
    virtual void UnpackMate(ZDDNode* node, Mate* mate, int child_num);
    virtual void UpdateMate(MateRcut* mate, int child_num);
    virtual int CheckTerminalPre(MateRcut* mate, int child_num);
    virtual int CheckTerminalPost(MateRcut* mate);
};

} // the end of the namespace

#endif // STATERCUT_HPP
