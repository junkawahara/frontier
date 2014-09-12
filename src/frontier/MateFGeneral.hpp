//
// MateFGeneral.hpp
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

#ifndef MATEFGENERAL_HPP
#define MATEFGENERAL_HPP

#include "StateFrontierAux.hpp"
#include "PseudoZDD.hpp"
#include "CompManager.hpp"

namespace frontier_dd {

class MateFGeneral;

static inline bool IsIn(int number, const std::vector<mate_t>& vec)
{
	return std::find(vec.begin(), vec.end(), number) != vec.end();
}

//*************************************************************************************************
// StateFGeneral:
class StateFGeneral : public StateFrontierAux<FrontierDegComp> {
protected:
    std::vector<std::vector<mate_t> > D_;
    std::vector<ShortPair> P_;
    std::vector<ShortPair> S_;
    std::vector<mate_t> C_;
    std::vector<mate_t> Q_;
    std::vector<mate_t> T_;

	bool is_using_cc_;
	bool is_using_cycle_;
	bool is_using_noe_;

    bool ignore_isolated_;

public:
    StateFGeneral(Graph* graph, const std::vector<std::vector<mate_t> >& D,
                  const std::vector<ShortPair>& P,
                  const std::vector<ShortPair>& S,
                  const std::vector<mate_t>& C,
                  const std::vector<mate_t>& Q,
                  const std::vector<mate_t>& T)
        : StateFrontierAux<FrontierDegComp>(graph), D_(D), P_(P), S_(S), C_(C), Q_(Q), T_(T),
          is_using_cc_(false), is_using_cycle_(false), is_using_noe_(false), ignore_isolated_(false)
    {
		int n = graph->GetNumberOfVertices();
		int m = graph->GetNumberOfEdges();

		for (int i = 1; i <= n; ++i) {
			if (!IsIn(i, C)) {
				is_using_cc_ = true;
				break;
			}
		}
		for (int i = 1; i <= m; ++i) {
			if (!IsIn(i, T)) {
				is_using_noe_ = true;
			}
		}

		if (!IsIn(0, Q)) {
			is_using_cycle_ = true;
		}

        byte initial_conf[sizeof(int) + 6];
        
        int pos = sizeof(int);
		if (is_using_cc_) {
			*reinterpret_cast<short*>(initial_conf + pos) = 0; // cc
			pos += sizeof(short);
		}
		if (is_using_cycle_) {
			*reinterpret_cast<short*>(initial_conf + pos) = 0; // cycle
			pos += sizeof(short);
		}
		if (is_using_noe_) {
			*reinterpret_cast<short*>(initial_conf + pos) = 0; // noe
			pos += sizeof(short);
		}
		*reinterpret_cast<int*>(initial_conf) = pos - sizeof(int);

        StateFrontierAux<FrontierDegComp>::Initialize(initial_conf, pos - sizeof(int));
    }

    virtual ~StateFGeneral() { }

	bool IsUsingCC()
	{
		return is_using_cc_;
	}

	bool IsUsingCycle()
	{
		return is_using_cycle_;
	}

	bool IsUsingNoe()
	{
		return is_using_noe_;
	}

    bool IsIgnoreIsolated()
    {
        return ignore_isolated_;
    } 

    void SetIgnoreIsolated(bool ignore_isolated)
    {
        ignore_isolated_ = ignore_isolated;
    }

    bool IsInPorS(int number)
    {
        for (unsigned int i = 0; i < P_.size(); ++i) {
            if (P_[i].first == number) {
                return true;
            }
            if (P_[i].second == number) {
                return true;
            }
        }
        for (unsigned int i = 0; i < S_.size(); ++i) {
            if (S_[i].first == number) {
                return true;
            }
            if (S_[i].second == number) {
                return true;
            }
        }
        return false;
    }

    virtual int GetNextAuxSize();
    virtual void Load(Mate* mate, byte* data);
    virtual void Store(Mate* mate, byte* data);

    virtual Mate* MakeEmptyMate();
    //virtual void PackMate(ZDDNode* node, Mate* mate);
    virtual void UnpackMate(ZDDNode* node, Mate* mate, int child_num);

    friend class MateFGeneral;
};

//*************************************************************************************************
// MateFGeneral
class MateFGeneral : public MateFrontier<FrontierDegComp> {
public:
    std::vector<mate_t>** vset;
    short vset_count;
    short cc;
    bool cycle;
    short noe;

	std::vector<mate_t>** swap_vset;

private:
    CompManager<FrontierDegComp> comp_manager_;

public:
    MateFGeneral(State* state) : MateFrontier<FrontierDegComp>(state),
                                 vset_count(0), cc(0), cycle(false), noe(0),
                                 comp_manager_(frontier_array, state->GetNumberOfVertices())
    {
        int n = state->GetNumberOfVertices();

        vset = new std::vector<mate_t>*[n + 1];
		vset[0] = NULL;
        for (int i = 1; i <= n; ++i) {
            vset[i] = new std::vector<mate_t>(); // need delete somewhere
        }

		swap_vset = new std::vector<mate_t>*[n + 1];
		swap_vset[0] = NULL;
		for (int i = 1; i <= n; ++i) {
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

template<>
std::string MateFrontier<FrontierDegComp>::GetPreviousString(State* state);
template<>
std::string MateFrontier<FrontierDegComp>::GetNextString(State* state);


} // the end of the namespace

#endif // MATEFGENERAL_HPP
