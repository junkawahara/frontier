//
// RootManager.hpp
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

#ifndef ROOTMANAGER_HPP
#define ROOTMANAGER_HPP

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>

namespace frontier_lib {

//*************************************************************************************************
// RootManager: 根を管理するクラス

class RootManager {
private:
    std::vector<int> source_array_;
public:
    void Add(int v)
    {
        source_array_.push_back(v);
    }

    int Get(int index) const
    {
        return source_array_[index];
    }

    int GetSize() const
    {
        return static_cast<int>(source_array_.size());
    }

    bool Exists(int v) const
    {
        return std::find(source_array_.begin(), source_array_.end(), v) != source_array_.end();
    }

    void Parse(std::string filename)
    {
        std::ifstream ifs(filename.c_str());
        Parse(ifs);
    }

    void Parse(std::istream& ifs)
    {
        int c;
        while (ifs >> c) {
            source_array_.push_back(c);
        }
    }
};

} // the end of the namespace

#endif // ROOTMANAGER_HPP
