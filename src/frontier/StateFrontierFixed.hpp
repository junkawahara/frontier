//
// StateFrontierFixed.hpp
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

#ifndef STATEFRONTIERFIXED_HPP
#define STATEFRONTIERFIXED_HPP

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
// StateFrontierFixed<T>: StateFrontier に付加的な configuration を管理する機能を加えたクラス
// T は付加的な情報を表す構造体型。
template <typename T>
class StateFrontierFixed : public StateFrontier {
protected:
    FixedPacker<T> fixed_packer_;

public:
    StateFrontierFixed(Graph* graph) : StateFrontier(graph) { }

    void Initialize(const T& initial_conf)
    {
        fixed_packer_.Initialize(initial_conf);
    }

    virtual bool Equals(const ZDDNode& node1, const ZDDNode& node2) const
    {
        return fixed_packer_.Equals(node1, node2) && StateFrontier::Equals(node1, node2);
    }

    virtual intx GetHashValue(const ZDDNode& node) const
    {
        return fixed_packer_.GetHashValue(node) * 15284356289ll
                + StateFrontier::GetHashValue(node);
    }

    virtual void Undo()
    {
        fixed_packer_.Undo();
        StateFrontier::Undo();
    }

protected:
    void PackAll(ZDDNode* node, mate_t* mate, T* mate_conf)
    {
        fixed_packer_.Pack(node, mate_conf);
        StateFrontier::Pack(node, mate);
    }

    void UnpackAll(ZDDNode* node, mate_t* mate, T* mate_conf)
    {
        fixed_packer_.Unpack(node, mate_conf);
        StateFrontier::Unpack(node, mate);
    }

    void UnpackFixed(ZDDNode* node, T* mate_conf)
    {
        fixed_packer_.Unpack(node, mate_conf);
    }

    void SeekTailAll()
    {
        StateFrontier::SeekTail();
        fixed_packer_.SeekTail();
    }

    void SeekTailFrontier()
    {
        StateFrontier::SeekTail();
    }

    void SeekTailFixed()
    {
        fixed_packer_.SeekTail();
    }

private:
    // seal functions in the base class (StateFrontier class)
    void Pack(ZDDNode* node, mate_t* mate);
    void UnpackAndSeek(ZDDNode* node, mate_t* mate);
    void Unpack(ZDDNode* node, mate_t* mate);
    void SeekTail();
};

template <>
class StateFrontierFixed<MateConfNull> : public StateFrontier {
public:
    StateFrontierFixed(Graph* graph) : StateFrontier(graph) { }

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
};

} // the end of the namespace

#endif // STATEFRONTIERFIXED_HPP
