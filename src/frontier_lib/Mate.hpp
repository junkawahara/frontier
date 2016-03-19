//
// Mate.hpp
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

#ifndef MATE_HPP
#define MATE_HPP

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
// Mate: mate を表すクラス
class Mate {
public:
    virtual ~Mate() { }
    virtual bool Equals(const ZDDNode& node1, const ZDDNode& node2,
                        const FrontierManager& frontier_manager) const = 0;
    virtual intx GetHashValue(const ZDDNode& node, const FrontierManager& frontier_manager) const = 0;
    virtual void PackMate(ZDDNode* node, const FrontierManager& frontier_manager) = 0;
    virtual void UnpackMate(ZDDNode* , int child_num, const FrontierManager& frontier_manager) = 0;
    virtual void Revert(const FrontierManager& frontier_manager) = 0;
};

class MateS : public Mate {
public:
    intx sdd;
private:
    bool use_subsetting_;
    RBuffer<intx> sdd_buffer_;

public:
    MateS() : sdd(2), use_subsetting_(false) { } // 2: root node

    virtual ~MateS() { }

    bool IsUseSubsetting()
    {
        return use_subsetting_;
    }

    void SetUseSubsetting(bool use_subsetting, ZDDNode* root_node)
    {
        use_subsetting_ = use_subsetting;

        if (use_subsetting) {
            intx* p = sdd_buffer_.GetWritePointerAndSeekHead(sizeof(intx));
            assert(sdd == 2);
            *p = sdd;
            root_node->p.pos_sdd = 0;
        }
    }

    virtual bool Equals(const ZDDNode& node1, const ZDDNode& node2,
                const FrontierManager&) const
    {
        if (!use_subsetting_) {
            return true;
        }

        // フロンティアに含まれる各頂点についてmate値が同じかどうか判定
        intx v1 = *sdd_buffer_.GetPointer(node1.p.pos_sdd);
        intx v2 = *sdd_buffer_.GetPointer(node2.p.pos_sdd);

        return v1 == v2;
    }

    virtual intx GetHashValue(const ZDDNode& node, const FrontierManager&) const
    {
        if (!use_subsetting_) {
            return 0;
        }

        return *sdd_buffer_.GetPointer(node.p.pos_sdd);
    }

    virtual void PackMate(ZDDNode* node, const FrontierManager&)
    {
        if (!use_subsetting_) {
            return;
        }

        intx* p = sdd_buffer_.GetWritePointerAndSeekHead(sizeof(intx));
        *p = sdd;
        node->p.pos_sdd = sdd_buffer_.GetHeadIndex() - sizeof(intx);
    }

    virtual void UnpackMate(ZDDNode* , int child_num, const FrontierManager&)
    {
        if (!use_subsetting_) {
            return;
        }

        sdd = *sdd_buffer_.GetReadPointer(0);

        if (child_num == 1) {
            sdd_buffer_.SeekTail(sizeof(intx));
        }
    }

    virtual void Revert(const FrontierManager&)
    {
        if (!use_subsetting_) {
            return;
        }

        sdd_buffer_.BackHead(sizeof(intx));
    }
};

template <typename FT>
class MateF : public MateS {
public:
    FT* frontier;
private:
    RBuffer<FT> frontier_buffer_;

public:
    MateF(State* state)
    {
        frontier = new FT[state->GetNumberOfVertices() + 1];
    }

    virtual ~MateF()
    {
        delete[] frontier;
    }

    virtual bool Equals(const ZDDNode& node1, const ZDDNode& node2,
                const FrontierManager& frontier_manager) const
    {
        if (!MateS::Equals(node1, node2, frontier_manager)) {
            return false;
        }
        // フロンティアに含まれる各頂点についてmate値が同じかどうか判定
        const FT* p1 = frontier_buffer_.GetPointer(node1.p.pos_frontier);
        const FT* p2 = frontier_buffer_.GetPointer(node2.p.pos_frontier);

        return memcmp(p1, p2, frontier_manager.GetNextFrontierSize() * sizeof(FT)) == 0;        
    }

    virtual intx GetHashValue(const ZDDNode& node, const FrontierManager& frontier_manager) const
    {
        uintx hash_value;

        hash_value = MateS::GetHashValue(node, frontier_manager);

        const byte* p = reinterpret_cast<const byte*>(frontier_buffer_.GetPointer(node.p.pos_frontier));
        for (uint i = 0; i < frontier_manager.GetNextFrontierSize() * sizeof(FT); ++i) {
            hash_value = hash_value * 15284356289ll + p[i];
        }
        return hash_value;
    }

    virtual void PackMate(ZDDNode* node, const FrontierManager& frontier_manager)
    {
        MateS::PackMate(node, frontier_manager);

        int frontier_size = frontier_manager.GetNextFrontierSize();
        FT* p = frontier_buffer_.GetWritePointerAndSeekHead(frontier_size);
        for (int i = 0; i < frontier_size; ++i) {
            p[i] = frontier[frontier_manager.GetNextFrontierValue(i)];
        }
        node->p.pos_frontier = frontier_buffer_.GetHeadIndex() - frontier_size;
    }

    virtual void UnpackMate(ZDDNode* node, int child_num, const FrontierManager& frontier_manager)
    {
        MateS::UnpackMate(node, child_num, frontier_manager);

        FT* p = frontier_buffer_.GetReadPointer(0);
        for (int i = 0; i < frontier_manager.GetPreviousFrontierSize(); ++i) {
            frontier[frontier_manager.GetPreviousFrontierValue(i)] = p[i];
        }

        if (child_num == 1) {
            frontier_buffer_.SeekTail(frontier_manager.GetPreviousFrontierSize());
        }
    }

    virtual void Revert(const FrontierManager& frontier_manager)
    {
        MateS::Revert(frontier_manager);

        frontier_buffer_.BackHead(frontier_manager.GetNextFrontierSize());
    }
};

template <typename FT, typename DT>
class MateFD : public MateF<FT> {
public:
    DT data;
private:
    FixedPacker<DT> fixed_packer_;

public:
    MateFD(State* state) : MateF<FT>(state) { }

    virtual ~MateFD() { }

    virtual void Initialize(ZDDNode* root_node, DT* initial_data)
    {
        fixed_packer_.Pack(root_node, initial_data);
    }

    virtual bool Equals(const ZDDNode& node1, const ZDDNode& node2,
                const FrontierManager& frontier_manager) const
    {
        return MateF<FT>::Equals(node1, node2, frontier_manager) && fixed_packer_.Equals(node1, node2);
    }

    virtual intx GetHashValue(const ZDDNode& node, const FrontierManager& frontier_manager) const
    {
        return MateF<FT>::GetHashValue(node, frontier_manager) * 15284356289ll + fixed_packer_.GetHashValue(node);
    }

    virtual void PackMate(ZDDNode* node, const FrontierManager& frontier_manager)
    {
        MateF<FT>::PackMate(node, frontier_manager);
        fixed_packer_.Pack(node, &data);
    }

    virtual void UnpackMate(ZDDNode* node, int child_num, const FrontierManager& frontier_manager)
    {
        MateF<FT>::UnpackMate(node, child_num, frontier_manager);
        fixed_packer_.Unpack(node, &data);

        if (child_num == 1) {
            fixed_packer_.SeekTail();
        }
    }

    virtual void Revert(const FrontierManager& frontier_manager)
    {
        MateF<FT>::Revert(frontier_manager);
        fixed_packer_.Revert();
    }
};


template <typename FT, typename DT, typename VT>
class MateFDV1 : public MateFD<FT, DT> {
public:
    std::vector<VT>* varray1;
private:
    std::vector<std::vector<VT>*> v_buffer_array1_;

public:
    MateFDV1(State* state) : MateFD<FT, DT>(state) { }

    virtual ~MateFDV1() { }

    virtual void Initialize(ZDDNode* root_node, DT* initial_data)
    {
        MateFD<FT, DT>::Initialize(root_node, initial_data);
        root_node->p.pos_v = 0;
        v_buffer_array1_.push_back(new std::vector<VT>());
    }

    virtual void Initialize(ZDDNode* root_node, DT* initial_data, std::vector<VT>* initial_varray1)
    {
        MateFD<FT, DT>::Initialize(root_node, initial_data);
        root_node->p.pos_v = 0;
        v_buffer_array1_.push_back(initial_varray1);
    }

    virtual bool Equals(const ZDDNode& node1, const ZDDNode& node2,
                const FrontierManager& frontier_manager) const
    {
        if (!MateFD<FT, DT>::Equals(node1, node2, frontier_manager)) {
            return false;
        }
        if (v_buffer_array1_[node1.p.pos_v]->size() != v_buffer_array1_[node2.p.pos_v]->size()) {
            return false;
        }
        for (uint i = 0; i < v_buffer_array1_[node1.p.pos_v]->size(); ++i) {
            if ((*v_buffer_array1_[node1.p.pos_v])[i] != (*v_buffer_array1_[node2.p.pos_v])[i]) {
                return false;
            }
        }
        return true;
    }

    virtual intx GetHashValue(const ZDDNode& node, const FrontierManager& frontier_manager) const
    {
        intx hash_value = MateFD<FT, DT>::GetHashValue(node, frontier_manager);
        for (uint i = 0; i < v_buffer_array1_[node.p.pos_v]->size(); ++i) {
            hash_value = hash_value * 15284356289ll + (*v_buffer_array1_[node.p.pos_v])[i];
        }
        return hash_value;
    }

    virtual void PackMate(ZDDNode* node, const FrontierManager& frontier_manager)
    {
        MateFD<FT, DT>::PackMate(node, frontier_manager);
        node->p.pos_v = static_cast<intx>(v_buffer_array1_.size());
        v_buffer_array1_.push_back(varray1);        
    }

    virtual void UnpackMate(ZDDNode* node, int child_num, const FrontierManager& frontier_manager)
    {
        MateFD<FT, DT>::UnpackMate(node, child_num, frontier_manager);
        varray1 = new std::vector<VT>(*v_buffer_array1_[node->p.pos_v]);
    }

    virtual void Revert(const FrontierManager& frontier_manager)
    {
        MateFD<FT, DT>::Revert(frontier_manager);

        delete v_buffer_array1_.back();
        v_buffer_array1_.resize(v_buffer_array1_.size() - 1);
    }
};

template <typename FT, typename DT, typename VT1, typename VT2>
class MateFDV2 : public MateFDV1<FT, DT, VT1> {
public:
    std::vector<VT2>* varray2;
private:
    std::vector<std::vector<VT2>*> v_buffer_array2_;

public:
    MateFDV2(State* state) : MateFDV1<FT, DT, VT1>(state) { }

    virtual ~MateFDV2() { }

    virtual void Initialize(ZDDNode* root_node, DT* initial_data)
    {
        MateFDV1<FT, DT, VT1>::Initialize(root_node, initial_data);
        v_buffer_array2_.push_back(new std::vector<VT2>());
    }

    virtual void Initialize(ZDDNode* root_node, DT* initial_data, std::vector<VT1>* initial_varray1,
                            std::vector<VT2>* initial_varray2)
    {
        MateFDV1<FT, DT, VT1>::Initialize(root_node, initial_data, initial_varray1);
        v_buffer_array2_.push_back(initial_varray2);
    }

    virtual bool Equals(const ZDDNode& node1, const ZDDNode& node2,
                const FrontierManager& frontier_manager) const
    {
        if (!MateFDV1<FT, DT, VT1>::Equals(node1, node2, frontier_manager)) {
            return false;
        }
        if (v_buffer_array2_[node1.p.pos_v]->size() != v_buffer_array2_[node2.p.pos_v]->size()) {
            return false;
        }
        for (uint i = 0; i < v_buffer_array2_[node1.p.pos_v]->size(); ++i) {
            if ((*v_buffer_array2_[node1.p.pos_v])[i] != (*v_buffer_array2_[node2.p.pos_v])[i]) {
                return false;
            }
        }
        return true;
    }

    virtual intx GetHashValue(const ZDDNode& node, const FrontierManager& frontier_manager) const
    {
        intx hash_value = MateFDV1<FT, DT, VT1>::GetHashValue(node, frontier_manager);
        for (uint i = 0; i < v_buffer_array2_[node.p.pos_v]->size(); ++i) {
            hash_value = hash_value * 15284356289ll + (*v_buffer_array2_[node.p.pos_v])[i];
        }
        return hash_value;
    }

    virtual void PackMate(ZDDNode* node, const FrontierManager& frontier_manager)
    {
        MateFDV1<FT, DT, VT1>::PackMate(node, frontier_manager);
        node->p.pos_v = static_cast<intx>(v_buffer_array2_.size());
        v_buffer_array2_.push_back(varray2);        
    }

    virtual void UnpackMate(ZDDNode* node, int child_num, const FrontierManager& frontier_manager)
    {
        MateFDV1<FT, DT, VT1>::UnpackMate(node, child_num, frontier_manager);
        varray2 = new std::vector<VT2>(*v_buffer_array2_[node->p.pos_v]);
    }

    virtual void Revert(const FrontierManager& frontier_manager)
    {
        MateFDV1<FT, DT, VT1>::Revert(frontier_manager);

        delete v_buffer_array2_.back();
        v_buffer_array2_.resize(v_buffer_array2_.size() - 1);
    }
};


} // the end of the namespace

#endif // MATE_HPP
