//
// MateMTPath.hpp
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

#ifndef MATEMTPATH_HPP
#define MATEMTPATH_HPP

#include <vector>
#include <string>

#include "MatePathMatching.hpp"
#include "PseudoZDD.hpp"

namespace frontier_dd {

class MateMTPath;

//*************************************************************************************************
// TerminalManager: ヒントペアの集合を管理するクラス
class TerminalManager {
private:
    std::vector<std::pair<int, int> > pair_array_;
public:
    void Parse(const std::string& filename);

    // v がヒント頂点であるか調べる
    // (v がいずれかのペア (v1, v2) の v1 または v2 と等しいときに true を返す)
    bool Exists(int v) const;

    // (v1, v2) がヒントペアであるか調べる
    bool ExistsPair(int v1, int v2) const;
};

//*************************************************************************************************
// StateMTPath: 複数終端対パスのための State
class StateMTPath : public StatePathMatching {
protected:
    TerminalManager terminal_manager_;

public:
    StateMTPath(Graph* graph);
    virtual ~StateMTPath();

    const TerminalManager& GetTerminalManager() const
    {
        return terminal_manager_;
    }

    void ParseTerminal(const std::string& filename);
};

//*************************************************************************************************
// MateMTPath
class MateMTPath : public MatePathMatching {
public:
    MateMTPath(State* state) : MatePathMatching(state) { }
    virtual ~MateMTPath() { }

    virtual int CheckTerminalPre(State* state, int child_num);
    virtual int CheckTerminalPost(State* state);
};

} // the end of the namespace

#endif // MATEMTPATH_HPP
