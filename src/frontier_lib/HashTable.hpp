//
// HashTable.hpp
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

#ifndef HASH_TABLE_HPP
#define HASH_TABLE_HPP

#include <vector>
#include <algorithm>
#include <cstdarg>
#include <cstdio>
#include <climits>
#include <iostream>
#include <fstream>

#include "Global.hpp"
#include "State.hpp"
#include "ZDDNode.hpp"
#include "SolutionArray.hpp"
#include "Mate.hpp"
#include "PseudoZDD.hpp"

namespace frontier_lib {

//*************************************************************************************************
// HashTable: HashTableを表すクラス
// 内部ハッシュ法により実装している。
class HashTable {
private:
    mate_t* hash_flush_table_;
    intx* hash_table_;
    intx size_;
    mate_t flush_time_;

public:
    HashTable(intx size) : size_(size), flush_time_(1)
    {
        Initialize();
    }

    ~HashTable()
    {
        Finalize();
    }

    void Expand()
    {
        Finalize();
        size_ *= 2;
        Initialize();
    }

    void Set(uintx hash_value, intx value)
    {
        hash_value %= size_;
        for (uintx i = hash_value; i < static_cast<uintx>(size_); ++i) {
            if (hash_flush_table_[i] != flush_time_) {
                hash_table_[i] = value;
                hash_flush_table_[i] = flush_time_;
                return;
            }
        }
        for (uintx i = 0; i < hash_value; ++i) {
            if (hash_flush_table_[i] != flush_time_) {
                hash_table_[i] = value;
                hash_flush_table_[i] = flush_time_;
                return;
            }
        }
        std::cerr << "Error: the hash is full!" << std::endl;
        exit(1);
    }

    intx Get(uintx hash_value, State* state, const ZDDNode& node,
             const std::vector<ZDDNode>& node_array, Mate* mate) const
    {
        hash_value %= size_;

        while (hash_flush_table_[hash_value] == flush_time_) {
            intx index = hash_table_[hash_value];
            if (state->Equals(node, node_array[index], mate)) {
                return index;
            }
            hash_value = (hash_value + 1) % size_;
        }
        return static_cast<intx>(-1);
    }


    intx GetSize() const
    {
        return size_;
    }

    void Flush()
    {
        ++flush_time_;
    }

private:
    void Initialize()
    {
        hash_flush_table_ = (mate_t*)calloc(size_, sizeof(mate_t)); // use calloc for padding 0
        if (hash_flush_table_ == NULL) {
            std::cerr << "Error: calloc for hash_flush_table_ failed!" << std::endl;
            exit(1);
        }

        hash_table_ = (intx*)malloc(size_ * sizeof(intx));
        if (hash_table_ == NULL) {
            std::cerr << "Error: malloc for hash_table_ failed!" << std::endl;
            exit(1);
        }
    }

    void Finalize()
    {
        free(hash_table_);
        hash_table_ = NULL;
        free(hash_flush_table_);
        hash_flush_table_ = NULL;
    }
};

} // the end of the namespace

#endif // HASH_TABLE_HPP
