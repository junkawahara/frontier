//
// StateMTPath.hpp
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

#ifndef STATEMTPATH_HPP
#define STATEMTPATH_HPP

#include <vector>
#include <fstream>
#include <string>

#include "StatePathMatching.hpp"
#include "../frontier_lib/PseudoZDD.hpp"

namespace frontier_lib {

//*************************************************************************************************
// TerminalManager: ヒントペアの集合を管理するクラス
class TerminalManager {
private:
    std::vector<std::pair<int, int> > pair_array_;
public:
    void Parse(const std::string& filename)
    {
        int x, y;
        std::ifstream ifs(filename.c_str());
        while (ifs >> x) {
            if (!(ifs >> y)) {
                std::cerr << "File format error in TerminalManager" << std::endl;
                exit(1);
            }
            if (x > y) {
                std::swap(x, y);
            }
            pair_array_.push_back(std::make_pair(x, y));
        }
    }

    // v がヒント頂点であるか調べる
    // (v がいずれかのペア (v1, v2) の v1 または v2 と等しいときに true を返す)
    bool Exists(int v) const
    {
        for (uint i = 0; i < pair_array_.size(); ++i)
        {
            if (pair_array_[i].first == v || pair_array_[i].second == v) {
                return true;
            }
        }
        return false;
    }

    // (v1, v2) がヒントペアであるか調べる
    bool ExistsPair(int v1, int v2) const
    {
        if (v1 > v2) {
            std::swap(v1, v2);
        }
        return find(pair_array_.begin(), pair_array_.end(), std::make_pair(v1, v2)) != pair_array_.end();
    }
};

//*************************************************************************************************
// StateMTPath: 複数終端対パスのための State
class StateMTPath : public StatePathMatching {
protected:
    typedef MateF<mate_t> MateMTPath;
    TerminalManager terminal_manager_;

public:
    StateMTPath(Graph* graph) : StatePathMatching(graph) { }
    virtual ~StateMTPath() { }

    const TerminalManager& GetTerminalManager() const
    {
        return terminal_manager_;
    }

    void ParseTerminal(const std::string& filename)
    {
        terminal_manager_.Parse(filename);
    }

protected:
    virtual int CheckTerminalPre(MateMTPath* mate, int child_num);
    virtual int CheckTerminalPost(MateMTPath* mate);
};

} // the end of the namespace

#endif // STATEMTPATH_HPP
