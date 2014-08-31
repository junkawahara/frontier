//
// StateFrontierAux.hpp
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

#ifndef STATEFRONTIERAUX_HPP
#define STATEFRONTIERAUX_HPP

#include <cstring>

#include "StateFrontier.hpp"
#include "PseudoZDD.hpp"
#include "ZDDNode.hpp"
#include "Packer.hpp"

namespace frontier_dd {

//*************************************************************************************************
// MateConfNull: configuration には（フロンティア情報以外は）何も加えないことを表すクラス
struct MateConfNull {
    // nothing
};

//*************************************************************************************************
// StateFrontierAux<T>: StateFrontier<T> に付加的な configuration を管理する機能を加えたクラス
template <typename T>
class StateFrontierAux : public StateFrontier<T> {
protected:
    RBuffer<byte> rbuffer_;

private:
    int current_buffer_size_;
    int next_buffer_size_;

public:
    StateFrontierAux(Graph* graph) : StateFrontier<T>(graph) { }

    void Initialize(byte* initial_conf, int initial_size)
    {
        current_buffer_size_ = next_buffer_size_ = initial_size + sizeof(int);
        byte* p = rbuffer_.GetWritePointerAndSeekHead(current_buffer_size_);
        memcpy(p, initial_conf, current_buffer_size_);
    }

    virtual void StartNextEdge()
    {
        StateFrontier<T>::StartNextEdge();

        current_buffer_size_ = next_buffer_size_;
        next_buffer_size_ = this->GetNextAuxSize() + sizeof(int);
    }

    virtual bool Equals(const ZDDNode& node1, const ZDDNode& node2) const
    {
        if (!StateFrontier<T>::Equals(node1, node2)) {
            return false;
        } else {
            const byte* p1 = rbuffer_.GetPointer(node1.p.pos_fixed);
            const byte* p2 = rbuffer_.GetPointer(node2.p.pos_fixed);

            const int n1 = *reinterpret_cast<const int*>(p1);
            const int n2 = *reinterpret_cast<const int*>(p2);

            //assert(n1 <= next_buffer_size_ - sizeof(int));
            //assert(n2 <= next_buffer_size_ - sizeof(int));

            if (n1 != n2) {
                return false;
            }

            p1 += sizeof(int);
            p2 += sizeof(int);
            for (uint i = 0; i < n1; ++i) {
                if (p1[i] != p2[i]) {
                    return false;
                }
            }
            return true;
        }
    }

    virtual intx GetHashValue(const ZDDNode& node) const
    {
        uintx hash_value = 0;

        const byte* p = rbuffer_.GetPointer(node.p.pos_fixed);
        const int n = *reinterpret_cast<const int*>(p);

        //assert(n <= next_buffer_size_ - sizeof(int));
        hash_value = n;

        p += sizeof(int);
        for (uint i = 0; i < n; ++i) {
            hash_value = hash_value * 15284356289ll + p[i];
        }

        hash_value += hash_value * 15284356289ll + StateFrontier<T>::GetHashValue(node);

        return hash_value;
    }

    virtual void PackMate(ZDDNode* node, Mate* mate)
    {
        PackAux(node, mate);
        StateFrontier<T>::PackMate(node, mate);
    }

    virtual void UnpackMate(ZDDNode* node, Mate* mate, int child_num)
    {
        UnpackAux(node, mate);
        if (child_num == 1) {
            SeekTailAux();
        }
        StateFrontier<T>::UnpackMate(node, mate, child_num);
    }

    virtual void Revert()
    {
        rbuffer_.BackHead(next_buffer_size_);
        StateFrontier<T>::Revert();
    }

protected:
    void PackAux(ZDDNode* node, Mate* mate)
    {
        byte* p = rbuffer_.GetWritePointerAndSeekHead(next_buffer_size_);
        this->Store(mate, p);
        node->p.pos_fixed = rbuffer_.GetHeadIndex() - next_buffer_size_;
    }

    void UnpackAux(ZDDNode* node, Mate* mate)
    {
        byte* p = rbuffer_.GetReadPointer(0);
        this->Load(mate, p);
    }

    void SeekTailAux()
    {
        rbuffer_.SeekTail(current_buffer_size_);
    }

    virtual int GetNextAuxSize() = 0;
    virtual void Load(Mate* mate, byte* data) = 0;
    virtual void Store(Mate* mate, byte* data) = 0;

};



/*template <>
class StateFrontierAux<MateConfNull> : public StateFrontier {
public:
    StateFrontierAux(Graph* graph) : StateFrontier(graph) { }

protected:
    void PackAll(ZDDNode* node, mate_t* mate, MateConfNull* )
    {
        StateFrontier::Pack(node, mate);
    }

    void UnpackAll(ZDDNode* node, mate_t* mate, MateConfNull* )
    {
        StateFrontier::Unpack(node, mate);
    }

    void SeekTailAll()
    {
        StateFrontier::SeekTail();
    }

private:
    // seal functions in Base class (StateFrontier class)
    void Pack(ZDDNode* node, mate_t* mate);
    void UnpackAndSeek(ZDDNode* node, mate_t* mate);
    void Unpack(ZDDNode* node, mate_t* mate);
    void SeekTail();
};*/

} // the end of the namespace

#endif // STATEFRONTIERAUX_HPP
