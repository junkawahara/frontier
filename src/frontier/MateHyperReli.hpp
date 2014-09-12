//
// MateHyperReli.hpp
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

#ifndef MATEHYPERRELI_HPP
#define MATEHYPERRELI_HPP

#include "StateFrontierHyperAux.hpp"
#include "PseudoZDD.hpp"
#include "RootManager.hpp"
#include "CompManager.hpp"

namespace frontier_dd {

class MateHyperReli;

static inline bool IsIn(int number, const std::vector<mate_t>& vec)
{
	return std::find(vec.begin(), vec.end(), number) != vec.end();
}

//*************************************************************************************************
// StateHyperReli: 信頼性多項式のための State
class StateHyperReli : public StateFrontierHyperAux<FrontierComp> {
protected:
    RootManager* root_mgr_;

public:
    StateHyperReli(HyperGraph* hgraph) : StateFrontierHyperAux<FrontierComp>(hgraph)
    {
        byte initial_conf[sizeof(int)];
		*reinterpret_cast<int*>(initial_conf) = 0;

        StateFrontierHyperAux<FrontierComp>::Initialize(initial_conf, 0);
    }

    virtual ~StateHyperReli() { }

    const RootManager* GetRootManager() const
    {
        return root_mgr_;
    }

    void SetRootManager(RootManager* root_mgr)
    {
        root_mgr_ = root_mgr;
    }

    bool IsInP(int number)
    {
        return root_mgr_->Exists(number);
    }

    virtual int GetNextAuxSize();
    virtual void Load(Mate* mate, byte* data);
    virtual void Store(Mate* mate, byte* data);

    virtual Mate* MakeEmptyMate();
    //virtual void PackMate(ZDDNode* node, Mate* mate);
    virtual void UnpackMate(ZDDNode* node, Mate* mate, int child_num);
};

//*************************************************************************************************
// MateHyperReli
class MateHyperReli : public MateFrontierHyper<FrontierComp> {
public:
    std::vector<mate_t>** vset;
    short vset_count;

	std::vector<mate_t>** swap_vset;

private:
    CompManagerHyper<FrontierComp> comp_manager_;

public:
    MateHyperReli(State* state) : MateFrontierHyper<FrontierComp>(state),
                                  vset_count(0),
                                  comp_manager_(frontier_array, state->GetNumberOfVertices())
    {
        int n = state->GetNumberOfVertices();

        vset = new std::vector<mate_t>*[2 * (n + 1)];
		vset[0] = NULL;
        for (int i = 1; i <= 2 * n; ++i) {
            vset[i] = new std::vector<mate_t>(); // need delete somewhere
        }

		swap_vset = new std::vector<mate_t>*[2 * (n + 1)];
		swap_vset[0] = NULL;
		for (int i = 1; i <= 2 * n; ++i) {
			swap_vset[i] = new std::vector<mate_t>(); // need delete somewhere
		}
    }

    static int FindComponentNumber(std::vector<mate_t>** vv, int vset_c, int number)
    {
        for (int i = 1; i <= vset_c; ++i) {
            for (unsigned int j = 0; j < vv[i]->size(); ++j) {
                if ((*vv[i])[j] == number) {
                    return i;
                }
            }
        }
        return -1;
    }

    virtual void UpdateMate(State* state, int child_num);
    virtual int CheckTerminalPre(State* state, int child_num);
    virtual int CheckTerminalPost(State* state);
    virtual void Rename(State* state);

    virtual std::string GetPreviousString(State* state);
    virtual std::string GetNextString(State* state);
};

//template<>
//std::string MateFrontierHyper<FrontierComp>::GetPreviousString(State* state);
//template<>
//std::string MateFrontierHyper<FrontierComp>::GetNextString(State* state);


} // the end of the namespace

#endif // MATEHYPERRELI_HPP
