//
// StateFrontier.cpp
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

#include "StateFrontier.hpp"
#include "PseudoZDD.hpp"

namespace frontier_dd {

using namespace std;

template<>
std::string MateFrontier<mate_t>::GetPreviousString(State* state)
{
    StateFrontier<mate_t>* st = (StateFrontier<mate_t>*)state;
    std::ostringstream oss;
    std::vector<mate_t> vec;

    oss << "val[";
    for (int i = 0; i < st->GetPreviousFrontierSize(); ++i) {
        mate_t u = st->GetPreviousFrontierValue(i);

        vec.push_back(frontier_array[u]);
        oss << u;

        if (i < st->GetPreviousFrontierSize() - 1) {
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

template<>
std::string MateFrontier<mate_t>::GetNextString(State* state)
{
    StateFrontier<mate_t>* st = (StateFrontier<mate_t>*)state;
    std::ostringstream oss;
    std::vector<mate_t> vec;

    oss << "val[";
    for (int i = 0; i < st->GetNextFrontierSize(); ++i) {
        mate_t u = st->GetNextFrontierValue(i);

        vec.push_back(frontier_array[u]);
        oss << u;

        if (i < st->GetNextFrontierSize() - 1) {
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

template<>
std::string MateFrontier<FrontierComp>::GetPreviousString(State* state)
{
    StateFrontier<FrontierComp>* st = (StateFrontier<FrontierComp>*)state;
    std::ostringstream oss;
    std::vector<mate_t> vec;

    oss << "comp[";
    for (int i = 0; i < st->GetPreviousFrontierSize(); ++i) {
        mate_t u = st->GetPreviousFrontierValue(i);

        vec.push_back(frontier_array[u].comp);
        oss << u;

        if (i < st->GetPreviousFrontierSize() - 1) {
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

template<>
std::string MateFrontier<FrontierComp>::GetNextString(State* state)
{
    StateFrontier<FrontierComp>* st = (StateFrontier<FrontierComp>*)state;
    std::ostringstream oss;
    std::vector<mate_t> vec;

    oss << "comp[";
    for (int i = 0; i < st->GetNextFrontierSize(); ++i) {
        mate_t u = st->GetNextFrontierValue(i);

        vec.push_back(frontier_array[u].comp);
        oss << u;

        if (i < st->GetNextFrontierSize() - 1) {
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


} // the end of the namespace
