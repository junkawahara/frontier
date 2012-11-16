//
// Packer.hpp
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

#ifndef PACKER_HPP
#define PACKER_HPP

#include "ZDDNode.hpp"

namespace frontier_dd {

//*************************************************************************************************
// FixedPacker<T>: Configuration を記憶するためのクラス。T は MateConf 型
template <typename T>
class FixedPacker {
private:
    RBuffer<byte> fixed_buffer_;

public:
    void Initialize(const T& initial_conf)
    {
        // for 0/1-terminal and root node
        byte* p = fixed_buffer_.GetWritePointerAndSeekHead(sizeof(T) * 3);
        memcpy(p + sizeof(T) * 2, &initial_conf, sizeof(T)); // copy root node
        fixed_buffer_.SeekTail(sizeof(T) * 2); // skip 0/1-terminal
    }

    bool Equals(const ZDDNode& node1, const ZDDNode& node2) const
    {
        const byte* p1 = fixed_buffer_.GetPointer(node1.p.id * sizeof(T));
        const byte* p2 = fixed_buffer_.GetPointer(node2.p.id * sizeof(T));
        for (uint i = 0; i < sizeof(T); ++i) {
            if (p1[i] != p2[i]) {
                return false;
            }
        }
        return true;
    }

    intx GetHashValue(const ZDDNode& node) const
    {
        uintx hash_value = 0;

        const byte* p = fixed_buffer_.GetPointer(node.p.id * sizeof(T));
        for (uint i = 0; i < sizeof(T); ++i) {
            hash_value = hash_value * 15284356289ll + p[i];
        }
        return hash_value;
    }

    void Pack(ZDDNode* , T* mate_conf)
    {
        byte* p = fixed_buffer_.GetWritePointerAndSeekHead(sizeof(T));
        memcpy(p, mate_conf, sizeof(T));
    }

    void Unpack(ZDDNode* , T* mate_conf)
    {
        byte* p = fixed_buffer_.GetReadPointer(0);
        memcpy(mate_conf, p, sizeof(T));
    }

    void SeekTail()
    {
        fixed_buffer_.SeekTail(sizeof(T));
    }

    void Undo()
    {
        fixed_buffer_.BackHead(sizeof(T));
    }
};

} // the end of the namespace

#endif // PACKER_HPP
