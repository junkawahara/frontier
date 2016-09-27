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


namespace frontier_lib {

//*************************************************************************************************
// Mate: mate を表すクラス
class Mate {
public:
    virtual ~Mate() { }
    virtual void SetOffset() = 0;
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
    intx s_offset_;

public:
    MateS() : sdd(2), use_subsetting_(false), s_offset_(0) { } // 2: root node

    virtual ~MateS() { }

    virtual void SetOffset()
    {
        s_offset_ = sdd_buffer_.GetHeadIndex();
    }

    bool IsUseSubsetting()
    {
        return use_subsetting_;
    }

    void SetUseSubsetting(bool use_subsetting, ZDDNode*)
    {
        use_subsetting_ = use_subsetting;

        if (use_subsetting) {
            assert(sdd == 2);
            sdd_buffer_.WriteAndSeekHead(sdd);
        }
    }

    virtual bool Equals(const ZDDNode& node1, const ZDDNode& node2,
                const FrontierManager&) const
    {
        if (!use_subsetting_) {
            return true;
        }

        return sdd_buffer_.GetValue(s_offset_ + node1.node_number)
            == sdd_buffer_.GetValue(s_offset_ + node2.node_number);
    }

    virtual intx GetHashValue(const ZDDNode& node, const FrontierManager&) const
    {
        if (!use_subsetting_) {
            return 0;
        }

        return sdd_buffer_.GetValue(s_offset_ + node.node_number);
    }

    virtual void PackMate(ZDDNode*, const FrontierManager&)
    {
        if (!use_subsetting_) {
            return;
        }

        sdd_buffer_.WriteAndSeekHead(sdd);
    }

    virtual void UnpackMate(ZDDNode* , int child_num, const FrontierManager&)
    {
        if (!use_subsetting_) {
            return;
        }

        sdd = sdd_buffer_.GetValueFromTail();

        if (child_num == 1) {
            sdd_buffer_.SeekTail(1);
        }
    }

    virtual void Revert(const FrontierManager&)
    {
        if (!use_subsetting_) {
            return;
        }

        sdd_buffer_.BackHead(1);
    }
};

}

#include "State.hpp"

namespace frontier_lib {

template <typename FT>
class MateF : public MateS {
public:
    FT* frontier;
private:
    RBuffer<FT> frontier_buffer_;
    intx f_offset_;

public:
    MateF(State* state) : f_offset_(0)
    {
        frontier = new FT[state->GetNumberOfVertices() + 1];
    }

    virtual ~MateF()
    {
        delete[] frontier;
    }

    virtual void SetOffset()
    {
        MateS::SetOffset();
        f_offset_ = frontier_buffer_.GetHeadIndex();
    }

    virtual bool Equals(const ZDDNode& node1, const ZDDNode& node2,
                const FrontierManager& frontier_manager) const
    {
        if (!MateS::Equals(node1, node2, frontier_manager)) {
            return false;
        }
        // フロンティアに含まれる各頂点についてmate値が同じかどうか判定
        int frontier_size = frontier_manager.GetNextFrontierSize();
        for (int i = 0; i < frontier_size; ++i) {
            //FT f1 = frontier_buffer_.GetValue(f_offset_ + node1.node_number * frontier_size + i);
            //FT f2 = frontier_buffer_.GetValue(f_offset_ + node2.node_number * frontier_size + i);
            //std::cout << "compare: " << f1 << ", " << f2 << std::endl;
            if (frontier_buffer_.GetValue(f_offset_ + node1.node_number * frontier_size + i)
                != frontier_buffer_.GetValue(f_offset_ + node2.node_number * frontier_size + i)) {
                return false;
            }
        }
        return true;
    }

    virtual intx GetHashValue(const ZDDNode& node, const FrontierManager& frontier_manager) const
    {
        uintx hash_value;

        hash_value = MateS::GetHashValue(node, frontier_manager);

        int frontier_size = frontier_manager.GetNextFrontierSize();
        //if (sizeof(FT) == sizeof(uintx)) {
        //    for (int i = 0; i < frontier_size; ++i) {
        //        hash_value = hash_value * 15284356289ll
        //            + reinterpret_cast<uintx>(frontier_buffer_.GetValue(f_offset_ + node.node_number * frontier_size + i));
        //    }
        //} else {
            for (int i = 0; i < frontier_size; ++i) {
                FT val = frontier_buffer_.GetValue(f_offset_ + node.node_number * frontier_size + i);
                hash_value = hash_value * 3161391051631ll + Translate(val);
            }
        //}
        return hash_value;
    }

    template<typename T>
    inline uintx Translate(T val) const
    {
        uintx hash_value = 0;
        for (uint j = 0; j < sizeof(T); ++j) {
            hash_value = hash_value * 15284356289ll + *(reinterpret_cast<byte*>(&val) + j);
        }
        return hash_value;
    }

    inline uintx Translate(mate_t val) const
    {
        return static_cast<uintx>(val);
    }

    inline uintx Translate(uint val) const
    {
        return static_cast<uintx>(val);
    }

    inline uintx Translate(uintx val) const
    {
        return static_cast<uintx>(val);
    }

    virtual void PackMate(ZDDNode* node, const FrontierManager& frontier_manager)
    {
        MateS::PackMate(node, frontier_manager);

        for (int i = 0; i < frontier_manager.GetNextFrontierSize(); ++i) {
            frontier_buffer_.WriteAndSeekHead(frontier[frontier_manager.GetNextFrontierValue(i)]);
        }
    }

    virtual void UnpackMate(ZDDNode* node, int child_num, const FrontierManager& frontier_manager)
    {
        MateS::UnpackMate(node, child_num, frontier_manager);

        for (int i = 0; i < frontier_manager.GetPreviousFrontierSize(); ++i) {
            frontier[frontier_manager.GetPreviousFrontierValue(i)] = frontier_buffer_.GetValueFromTail(i);
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
    RBuffer<DT> dt_buffer_;
    intx d_offset_;

public:
    MateFD(State* state) : MateF<FT>(state), d_offset_(0) { }

    virtual ~MateFD() { }

    virtual void SetOffset()
    {
        MateF<FT>::SetOffset();
        d_offset_ = dt_buffer_.GetHeadIndex();
    }

    virtual void Initialize(ZDDNode* /*root_node*/, DT initial_data)
    {
        dt_buffer_.WriteAndSeekHead(initial_data);
    }

    virtual bool Equals(const ZDDNode& node1, const ZDDNode& node2,
                const FrontierManager& frontier_manager) const
    {
        if (!MateF<FT>::Equals(node1, node2, frontier_manager)) {
            return false;
        }
        return dt_buffer_.GetValue(d_offset_ + node1.node_number)
            == dt_buffer_.GetValue(d_offset_ + node2.node_number);
    }

    virtual intx GetHashValue(const ZDDNode& node, const FrontierManager& frontier_manager) const
    {
        uintx hash_value = MateF<FT>::GetHashValue(node, frontier_manager);
        DT dt = dt_buffer_.GetValue(d_offset_ + node.node_number);
        for (uint i = 0; i < sizeof(DT); ++i) {
            hash_value = hash_value * 15284356289ll + *(reinterpret_cast<byte*>(&dt) + i);
        }
        return hash_value;
    }

    virtual void PackMate(ZDDNode* node, const FrontierManager& frontier_manager)
    {
        MateF<FT>::PackMate(node, frontier_manager);
        dt_buffer_.WriteAndSeekHead(data);
    }

    virtual void UnpackMate(ZDDNode* node, int child_num, const FrontierManager& frontier_manager)
    {
        MateF<FT>::UnpackMate(node, child_num, frontier_manager);
        data = dt_buffer_.GetValueFromTail();

        if (child_num == 1) {
            dt_buffer_.SeekTail(1);
        }
    }

    virtual void Revert(const FrontierManager& frontier_manager)
    {
        MateF<FT>::Revert(frontier_manager);
        dt_buffer_.BackHead(1);
    }
};

template <typename FT, typename DT, typename VT>
class MateFDV1 : public MateFD<FT, DT> {
public:
    VT* varray1;
    int varray1_size;
    static const int MAX_VARRAY1_SIZE = 65536;
private:
    //std::vector<std::vector<VT>*> v_buffer_array1_;
    RBuffer<VT> vt1_buffer_;
    RBuffer<uintx> pos_buffer_;
    intx vt1_pos_offset_;
    intx vt1_buf_offset_;
    intx prev_size_;
    //intx v1_prev_offset_;

public:
    MateFDV1(State* state) : MateFD<FT, DT>(state), vt1_pos_offset_(0), prev_size_(0)/*, v1_prev_offset_(-9999)*/ {
        //varray1 = new std::vector<VT>();
        varray1 = new VT[MAX_VARRAY1_SIZE];
    }

    virtual ~MateFDV1() { }

    virtual void SetOffset()
    {
        MateFD<FT, DT>::SetOffset();
        //v1_prev_offset_ = vt1_pos_offset_;
        //vt1_pos_offset_ = static_cast<intx>(v_buffer_array1_.size());//dt_buffer_.GetTailIndex();
        vt1_pos_offset_ = static_cast<intx>(pos_buffer_.GetHeadIndex());
        vt1_buf_offset_ = static_cast<intx>(vt1_buffer_.GetHeadIndex());
        prev_size_ = 0;
    }

    virtual void Initialize(ZDDNode* root_node, DT initial_data)
    {
        MateFD<FT, DT>::Initialize(root_node, initial_data);
        //root_node->node_number = 0;
        //v_buffer_array1_.push_back(new std::vector<VT>());
        pos_buffer_.WriteAndSeekHead(0); // The initial size is 0.
    }

    virtual void Initialize(ZDDNode* root_node, DT initial_data, const std::vector<VT>& initial_varray1)
    {
        MateFD<FT, DT>::Initialize(root_node, initial_data);
        //root_node->node_number = 0;
        for (size_t i = 0; i < initial_varray1.size(); ++i) {
            vt1_buffer_.WriteAndSeekHead(initial_varray1[i]);
        }
        pos_buffer_.WriteAndSeekHead(initial_varray1.size());
        //v_buffer_array1_.push_back(initial_varray1);
    }

    virtual bool Equals(const ZDDNode& node1, const ZDDNode& node2,
                const FrontierManager& frontier_manager) const
    {
        if (!MateFD<FT, DT>::Equals(node1, node2, frontier_manager)) {
            return false;
        }
        uintx size1 = GetSize(vt1_pos_offset_ + node1.node_number);
        uintx size2 = GetSize(vt1_pos_offset_ + node2.node_number);
        if (size1 != size2) {
            return false;
        }
        uintx pos1 = vt1_buf_offset_ + GetPos(vt1_pos_offset_ + node1.node_number);
        uintx pos2 = vt1_buf_offset_ + GetPos(vt1_pos_offset_ + node2.node_number);
        for (uint i = 0; i < size1; ++i) {
            if (vt1_buffer_.GetValue(pos1 + i) != vt1_buffer_.GetValue(pos2 + i)) {
                return false;
            }
        }
        return true;
    }

    virtual intx GetHashValue(const ZDDNode& node, const FrontierManager& frontier_manager) const
    {
        intx hash_value = MateFD<FT, DT>::GetHashValue(node, frontier_manager);
        uintx size = GetSize(vt1_pos_offset_ + node.node_number);
        uintx pos = vt1_buf_offset_ + GetPos(vt1_pos_offset_ + node.node_number);
        for (uint i = 0; i < size; ++i) {
            hash_value = hash_value * 15284356289ll + vt1_buffer_.GetValue(pos + i);
        }
        return hash_value;
    }

    virtual void PackMate(ZDDNode* node, const FrontierManager& frontier_manager)
    {
        MateFD<FT, DT>::PackMate(node, frontier_manager);
        //node->node_number = static_cast<intx>(v_buffer_array1_.size());
        //v_buffer_array1_.push_back(varray1);
        for (int i = 0; i < varray1_size; ++i) {
            vt1_buffer_.WriteAndSeekHead(varray1[i]);
        }
        if (vt1_pos_offset_ < pos_buffer_.GetHeadIndex()) {
            pos_buffer_.WriteAndSeekHead(pos_buffer_.Peek() + varray1_size);
        } else {
            pos_buffer_.WriteAndSeekHead(varray1_size);
        }
    }

    virtual void UnpackMate(ZDDNode* node, int child_num, const FrontierManager& frontier_manager)
    {
        MateFD<FT, DT>::UnpackMate(node, child_num, frontier_manager);
        //varray1 = new std::vector<VT>(*v_buffer_array1_[v1_prev_offset_ + node->node_number]);
        uintx size = pos_buffer_.GetValueFromTail(0);
        for (uintx i = 0; i < size - prev_size_; ++i) {
            varray1[i] = vt1_buffer_.GetValueFromTail(i);
        }
        varray1_size = size - prev_size_;
        if (child_num == 1) {
            if (size - prev_size_ > 0) {
                vt1_buffer_.SeekTail(size - prev_size_);
            }
            pos_buffer_.SeekTail(1);
            prev_size_ = size;
        }
    }

    virtual void Revert(const FrontierManager& frontier_manager)
    {
        MateFD<FT, DT>::Revert(frontier_manager);

        //delete v_buffer_array1_.back();
        //v_buffer_array1_.resize(v_buffer_array1_.size() - 1);
        uintx size = pos_buffer_.Peek();
        if (vt1_pos_offset_ < pos_buffer_.GetHeadIndex()) {
            uintx c = pos_buffer_.Peek(1);
            assert(size >= c);
            size -= c;
        }
        vt1_buffer_.BackHead(size);
        pos_buffer_.BackHead(1);
    }

private:
    uintx GetPos(intx index) const
    {
        if (index > vt1_pos_offset_) {
            return pos_buffer_.GetValue(index - 1);
        } else {
            return 0;
        }
    }

    uintx GetSize(intx index) const
    {
        if (index > vt1_pos_offset_) {
            return pos_buffer_.GetValue(index) - pos_buffer_.GetValue(index - 1);
        } else {
            return pos_buffer_.GetValue(index);
        }
    }
};

template <typename FT, typename DT, typename VT1, typename VT2>
class MateFDV2 : public MateFDV1<FT, DT, VT1> {
public:
    std::vector<VT2>* varray2;
private:
    std::vector<std::vector<VT2>*> v_buffer_array2_;
    intx v2_offset_;
    intx v2_prev_offset_;

public:
    MateFDV2(State* state) : MateFDV1<FT, DT, VT1>(state), v2_offset_(0), v2_prev_offset_(-9999) { }

    virtual ~MateFDV2() { }

    virtual void SetOffset()
    {
        MateFDV1<FT, DT, VT1>::SetOffset();
        v2_prev_offset_ = v2_offset_;
        v2_offset_ = static_cast<intx>(v_buffer_array2_.size());//dt_buffer_.GetTailIndex();
    }

    virtual void Initialize(ZDDNode* root_node, DT initial_data)
    {
        MateFDV1<FT, DT, VT1>::Initialize(root_node, initial_data);
        v_buffer_array2_.push_back(new std::vector<VT2>());
    }

    virtual void Initialize(ZDDNode* root_node, DT initial_data, std::vector<VT1>* initial_varray1,
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
        if (v_buffer_array2_[v2_offset_ + node1.node_number]->size() != v_buffer_array2_[v2_offset_ + node2.node_number]->size()) {
            return false;
        }
        for (uint i = 0; i < v_buffer_array2_[v2_offset_ + node1.node_number]->size(); ++i) {
            if ((*v_buffer_array2_[v2_offset_ + node1.node_number])[i] != (*v_buffer_array2_[v2_offset_ + node2.node_number])[i]) {
                return false;
            }
        }
        return true;
    }

    virtual intx GetHashValue(const ZDDNode& node, const FrontierManager& frontier_manager) const
    {
        intx hash_value = MateFDV1<FT, DT, VT1>::GetHashValue(node, frontier_manager);
        for (uint i = 0; i < v_buffer_array2_[v2_offset_ + node.node_number]->size(); ++i) {
            hash_value = hash_value * 15284356289ll + (*v_buffer_array2_[v2_offset_ + node.node_number])[i];
        }
        return hash_value;
    }

    virtual void PackMate(ZDDNode* node, const FrontierManager& frontier_manager)
    {
        MateFDV1<FT, DT, VT1>::PackMate(node, frontier_manager);
        //node->node_number = static_cast<intx>(v_buffer_array2_.size());
        v_buffer_array2_.push_back(varray2);
    }

    virtual void UnpackMate(ZDDNode* node, int child_num, const FrontierManager& frontier_manager)
    {
        MateFDV1<FT, DT, VT1>::UnpackMate(node, child_num, frontier_manager);
        varray2 = new std::vector<VT2>(*v_buffer_array2_[v2_prev_offset_ + node->node_number]);
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
