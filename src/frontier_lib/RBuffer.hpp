//
// RBuffer.hpp
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

#ifndef RBUFFER_HPP
#define RBUFFER_HPP

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


namespace frontier_lib {

//*************************************************************************************************
// RBuffer<T>: T型のデータを格納するキュー
template <typename T>
class RBuffer {
private:
    std::vector<T*> buffer_array_;
    //std::vector<intx> tail_pos_array_;
    int deleted_index_;
    intx head_;
    intx tail_;

    static const intx BLOCK_SIZE_ = (1 << 24);

public:
    RBuffer() : deleted_index_(0), head_(0), tail_(0)
    {
        buffer_array_.push_back(new T[BLOCK_SIZE_]);
        //tail_pos_array_.push_back(-1);
    }

    ~RBuffer()
    {
        for (uint i = deleted_index_; i < buffer_array_.size(); ++i) {
            delete[] buffer_array_[i];
            buffer_array_[i] = NULL;
        }

        for (uint i = 0; i < buffer_array_.size(); ++i) { // for debug
            assert(buffer_array_[i] == NULL);
        }
    }

    intx GetHeadIndex() const
    {
        return head_;
    }

    //intx GetTailIndex() const
    //{
    //    return tail_;
    //}

    T GetValue(intx index) const
    {
        return buffer_array_[index / BLOCK_SIZE_][index % BLOCK_SIZE_];
    }

    // return the (tail + offset)-th value
    T GetValueFromTail(uintx offset = 0) const
    {
        return buffer_array_[(tail_ + offset) / BLOCK_SIZE_][(tail_ + offset) % BLOCK_SIZE_];
    }

    // return the (head - offset)-th value
    T Peek(uintx offset = 0) const
    {
        return buffer_array_[(head_ - 1 - offset) / BLOCK_SIZE_][(head_ - 1 - offset) % BLOCK_SIZE_];
    }

    //T* GetPointer(intx index)
    //{
    //    return &buffer_array_[index / BLOCK_SIZE_][index % BLOCK_SIZE_];
    //}

    //const T* GetPointer(intx index) const
    //{
    //    return &buffer_array_[index / BLOCK_SIZE_][index % BLOCK_SIZE_];
    //}

    //T* GetReadPointer(uintx offset)
    //{
    //    return &buffer_array_[(tail_ + offset) / BLOCK_SIZE_][(tail_ + offset) % BLOCK_SIZE_];
    //}

    //const T* GetReadPointer(uintx offset) const
    //{
    //    return &buffer_array_[(tail_ + offset) / BLOCK_SIZE_][(tail_ + offset) % BLOCK_SIZE_];
    //}

    void WriteAndSeekHead(T value)
    {
        while (static_cast<int>(buffer_array_.size()) <= head_ / BLOCK_SIZE_) {
            buffer_array_.push_back(new T[BLOCK_SIZE_]);
        }
        buffer_array_[head_ / BLOCK_SIZE_][head_ % BLOCK_SIZE_] = value;
        ++head_;
    }

    /*T* GetWritePointerAndSeekHead(intx seek_size)
    {
        assert(seek_size < BLOCK_SIZE_);

        if (head_ % BLOCK_SIZE_ + seek_size >= BLOCK_SIZE_) {
            assert(head_ / BLOCK_SIZE_ < static_cast<intx>(tail_pos_array_.size()));
            tail_pos_array_[head_ / BLOCK_SIZE_] = head_ % BLOCK_SIZE_;
            tail_pos_array_.push_back(-1);

            if (tail_ == head_) {
                tail_ = (head_ / BLOCK_SIZE_ + 1) * BLOCK_SIZE_;
            }
            head_ = (head_ / BLOCK_SIZE_ + 1) * BLOCK_SIZE_ + seek_size;
            while (static_cast<int>(buffer_array_.size()) <= (head_ - seek_size) / BLOCK_SIZE_) {
                buffer_array_.push_back(new T[BLOCK_SIZE_]);
            }

            return &buffer_array_[(head_ - seek_size) / BLOCK_SIZE_][0];
        } else {
            head_ += seek_size;
            return &buffer_array_[(head_ - seek_size) / BLOCK_SIZE_][(head_ - seek_size)
                % BLOCK_SIZE_];
        }
        }*/

    void BackHead(intx offset)
    {
        head_ -= offset;
    }

    void SeekTail(intx offset)
    {
        assert(offset >= 0);
        assert(tail_ + offset <= head_);

        tail_ += offset;

        for (; deleted_index_ < tail_ / BLOCK_SIZE_; ++deleted_index_) {
            buffer_array_.push_back(buffer_array_[deleted_index_]);
            buffer_array_[deleted_index_] = NULL;
        }

        /*if (tail_pos_array_[tail_ / BLOCK_SIZE_] == tail_ % BLOCK_SIZE_) {
            for (; deleted_index_ < tail_ / BLOCK_SIZE_ + 1; ++deleted_index_) {
                buffer_array_.push_back(buffer_array_[deleted_index_]);
                buffer_array_[deleted_index_] = NULL;
            }
            tail_ = (tail_ / BLOCK_SIZE_ + 1) * BLOCK_SIZE_;
            }*/
    }

    
};

//*************************************************************************************************
// DBuffer: RBuffer のデバッグ用データ構造
template <typename T>
class DBuffer {
private:
    T* buffer_;
    intx head_;
    intx tail_;

    static const int BUFFER_SIZE_ = (1 << 30);

public:
    DBuffer() : head_(0), tail_(0)
    {
        buffer_ = static_cast<T*>(malloc(BUFFER_SIZE_));
        if (buffer_ == NULL) {
            std::cerr << "Error: malloc for buffer_ failed!" << std::endl;
            exit(1);
        }
    }

    ~DBuffer()
    {
        free(buffer_);
    }

    intx GetHeadIndex() const
    {
        return head_;
    }

    T* GetPointer(intx index)
    {
        return &buffer_[index];
    }

    const T* GetPointer(intx index) const
    {
        return &buffer_[index];
    }

    T* GetReadPointer(uintx offset)
    {
        return &buffer_[(tail_ + offset)];
    }

    const T* GetReadPointer(uintx offset) const
    {
        return &buffer_[(tail_ + offset)];
    }

    T* GetWritePointerAndSeekHead(intx size)
    {
        head_ += size;
        return &buffer_[head_ - size];
    }

    void BackHead(intx index)
    {
        head_ -= index;
    }

    void SeekTail(uintx offset)
    {
        tail_ += static_cast<intx>(offset);
    }
};

} // the end of the namespace

#endif // RBUFFER_HPP
