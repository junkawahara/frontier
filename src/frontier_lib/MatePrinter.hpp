//
// MatePrinter.hpp
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

#ifndef MATEPRINTER_HPP
#define MATEPRINTER_HPP

#include <vector>
#include <set>
#include <algorithm>
#include <string>
#include <iostream>
#include <cassert>
#include <cstdio>

#include "Global.hpp"
#include "Graph.hpp"
#include "ZDDNode.hpp"
#include "RBuffer.hpp"
#include "FrontierManager.hpp"
#include "FrontierComp.hpp"
#include "Packer.hpp"

namespace frontier_lib {

//*************************************************************************************************
// MatePrinter
template <typename T>
class MatePrinter {
public:
    static std::string GetString(const std::string& val_name,
                                 const T& frontier, const FrontierManager& frontier_manager, bool next)
    {
        std::ostringstream oss;
        std::vector<mate_t> vec;

        oss << val_name << "[";
        for (int i = 0; (!next && i < frontier_manager.GetPreviousFrontierSize()) ||
                 (next && i < frontier_manager.GetNextFrontierSize()); ++i) {
            mate_t u = (!next ? frontier_manager.GetPreviousFrontierValue(i) :
                        frontier_manager.GetNextFrontierValue(i));

            vec.push_back(frontier(u));
            oss << u;

            if ((!next && i < frontier_manager.GetPreviousFrontierSize() - 1) ||
                 (next && i < frontier_manager.GetNextFrontierSize() - 1)) {
                oss << ", ";
            }
        }
        oss << "] = [";

        for (unsigned int i = 0; i < vec.size(); ++i) {
            oss << vec[i];
            if (i < vec.size() - 1) {
                oss << ", ";
            }
        }
        oss << "]";

        return oss.str();
    }

    static std::string GetStringP(const std::string& val_name,
                                         T* frontier, const FrontierManager& frontier_manager, bool next)
    {
        FrontierPointerPrinter<T> fpp(frontier);
        return MatePrinter<FrontierPointerPrinter<T> >::GetString(val_name, fpp, frontier_manager, next);
    }

    static std::string GetStringComp(const std::string& val_name,
                                             T* frontier, const FrontierManager& frontier_manager, bool next)
    {
        FrontierCompPrinter<T> fcp(frontier);
        return MatePrinter<FrontierCompPrinter<T> >::GetString(val_name, fcp, frontier_manager, next);
    }

    static std::string GetStringWeight(const std::string& val_name,
                                               T* frontier, const FrontierManager& frontier_manager, bool next)
    {
        FrontierWeightPrinter<T> fwp(frontier);
        return MatePrinter<FrontierWeightPrinter<T> >::GetString(val_name, fwp, frontier_manager, next);
    }

    template <typename M>
    class FrontierPointerPrinter {
    private:
        M* frontier;

    public:
        FrontierPointerPrinter(M* f) {
            frontier = f;
        }

        M operator()(M v) const {
            return frontier[v];
        }
    };

    template <typename M>
    class FrontierCompPrinter {
    private:
        M* frontier;

    public:
        FrontierCompPrinter(M* f) {
            frontier = f;
        }

        mate_t operator()(mate_t v) const {
            return frontier[v].comp;
        }
    };

    template <typename M>
    class FrontierWeightPrinter {
    private:
        M* frontier;

    public:
        FrontierWeightPrinter(M* f) {
            frontier = f;
        }

        mate_t operator()(mate_t v) const {
            return frontier[v].weight;
        }
    };
};


} // the end of the namespace

#endif // MATEPRINTER_HPP
