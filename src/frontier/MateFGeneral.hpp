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

#include <sstream>
#include <fstream>

#include "StateFrontierAux.hpp"
#include "PseudoZDD.hpp"
#include "CompManager.hpp"

namespace frontier_dd {

class MateFGeneral;

//*************************************************************************************************
// FGeneralParser:
class FGeneralParser {
private:
    int default_n_;
    int default_m_;

public:
    std::vector<std::vector<mate_t> > D;
    std::vector<ShortPair> P;
    std::vector<ShortPair> S;
    std::vector<mate_t> C;
    std::vector<mate_t> Q;
    std::vector<mate_t> T;

    void Parse(std::istream& ist, int default_n, int default_m)
    {
        default_n_ = default_n;
        default_m_ = default_m;

        std::string str("");
        std::string line;
        while (std::getline(ist, line)) {
            str += line;
        }

        int start = -1;
        int end;
        for (uint i = 0; i < str.length(); ++i) {
            char c = str[i];

            if (('A' <= c && c <= 'Z') || i >= str.length() - 1) {
                if (!('A' <= c && c <= 'Z') && i >= str.length() - 1) {
                    ++i;
                }
                if (start == -1) {
                    start = i;
                } else {
                    end = i;
                    std::string line = str.substr(start, end - start);
                    switch (str[start]) {
                    case 'D':
                        ParseD(line);
                        break;
                    case 'P':
                        ParseP(line);
                        break;
                    case 'S':
                        ParseS(line);
                        break;
                    case 'C':
                        ParseC(line);
                        break;
                    case 'Q':
                        ParseQ(line);
                        break;
                    case 'T':
                        ParseT(line);
                        break;
                    }
                    start = i;
                }
            }
        }
    }

private:
    void ParseD(std::string str)
    {
        uint pos = 0;
        while (pos < str.length() && str[pos] != ':') {
            ++pos;
        }
        std::istringstream iss(str.substr(pos + 1));
        bool is_first = true;
        int x;
        char y;

        D.push_back(std::vector<mate_t>()); // dummy

        while (true) {
            int c = iss.peek();
            if (c == -1) {
                break;
            } else if ('0' <= c && c <= '9') {
                iss >> x;
                if (is_first) {
                    is_first = false;
                    D.push_back(std::vector<mate_t>());
                }
                D.back().push_back(x);
            } else if (c == ';') {
                if (is_first) { // The previous element is empty.
                    D.push_back(std::vector<mate_t>());
                }
                is_first = true;
                iss >> y;
            } else if (c == 'a') {
                if (iss.get() != 'a' || iss.get() != 'l' || iss.get() != 'l') {
                    std::cerr << "format error" << std::endl;
                    exit(1);
                } else {
                    if (is_first) {
                        is_first = false;
                        D.push_back(std::vector<mate_t>());
                    }
                    for (int j = 0; j <= default_n_; ++j) {
                        D.back().push_back(j);
                    }
                }
            } else if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
                iss.get();
                continue;
            } else {
                break;
            }
        }
    }

    void ParseP(std::string str)
    {
        uint pos = 0;
        while (pos < str.length() && str[pos] != ':') {
            ++pos;
        }
        ParsePairSequence(&P, str.substr(pos + 1));
        //std::cout << "parseP: " << str << std::endl;
    }

    void ParseS(std::string str)
    {
        uint pos = 0;
        while (pos < str.length() && str[pos] != ':') {
            ++pos;
        }
        ParsePairSequence(&S, str.substr(pos + 1));
        //std::cout << "parseS: " << str << std::endl;
    }

    void ParsePairSequence(std::vector<ShortPair>* vec, std::string str)
    {
        std::istringstream iss(str);
        bool is_first = true;
        int x;
        int first_num;

        while (true) {
            int c = iss.peek();
            if (c == -1) {
                break;
            } else if ('0' <= c && c <= '9') {
                iss >> x;
                if (is_first) {
                    is_first = false;
                    first_num = x;
                } else {
                    is_first = true;
                    vec->push_back(ShortPair(first_num, x));
                }
            } else if (c == '{' || c == '}' || c == ',' ||
                       c == ' ' || c == '\t' || c == '\r' || c == '\n') {
                iss.get();
                continue;
            } else {
                break;
            }
        }
    }

    void ParseC(std::string str)
    {
        uint pos = 0;
        while (pos < str.length() && str[pos] != ':') {
            ++pos;
        }
        ParseNumSequence(&C, str.substr(pos + 1), true);

        //std::cout << "parseC: " << str << std::endl;
    }

    void ParseQ(std::string str)
    {
        uint pos = 0;
        while (pos < str.length() && str[pos] != ':') {
            ++pos;
        }
        ParseNumSequence(&Q, str.substr(pos + 1), true);

        //std::cout << "parseQ: " << str << std::endl;
    }

    void ParseT(std::string str)
    {
        uint pos = 0;
        while (pos < str.length() && str[pos] != ':') {
            ++pos;
        }
        ParseNumSequence(&T, str.substr(pos + 1), false);

        //std::cout << "parseT: " << str << std::endl;
    }

    // is_vertex: true -> default_n is used. false -> default_m is used
    void ParseNumSequence(std::vector<mate_t>* vec, std::string str, bool is_vertex)
    {
        std::istringstream iss(str);
        int x;
        int c = iss.peek();
        while (c == ' ') {
            iss.get();
            c = iss.peek();
        }
        if (iss.peek() == 'a') {
            if (iss.get() != 'a' || iss.get() != 'l' || iss.get() != 'l') {
                std::cerr << "format error" << std::endl;
                exit(1);
            } else {
                int n = (is_vertex ? default_n_ : default_m_);
                for (int i = 0; i <= n; ++i) {
                    vec->push_back(i);
                }
            }
        } else {
            while (iss >> x) {
                vec->push_back(x);
            }
        }
    }
};


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
        Initialize(graph);
    }

    StateFGeneral(Graph* graph, std::string filename)
        : StateFrontierAux<FrontierDegComp>(graph),
          is_using_cc_(false), is_using_cycle_(false), is_using_noe_(false), ignore_isolated_(false)
    {
		int n = graph->GetNumberOfVertices();
		int m = graph->GetNumberOfEdges();

        std::ifstream ifs;
        ifs.open(filename.c_str());

        FGeneralParser parser;
        parser.Parse(ifs, n, m);
        ifs.close();

        D_ = parser.D;
        P_ = parser.P;
        S_ = parser.S;
        C_ = parser.C;
        Q_ = parser.Q;
        T_ = parser.T;

		Initialize(graph);
    }

    virtual ~StateFGeneral() { }

    void Initialize(Graph* graph)
    {
		int n = graph->GetNumberOfVertices();
		int m = graph->GetNumberOfEdges();

        for (int i = 1; i <= n; ++i) {
			if (!IsIn(i, C_)) {
				is_using_cc_ = true;
				break;
			}
		}
		for (int i = 1; i <= m; ++i) {
			if (!IsIn(i, T_)) {
				is_using_noe_ = true;
			}
		}

		if (!IsIn(0, Q_)) {
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

    void PrintParameter(std::ostream& ost)
    {
        ost << "D: ";
        for (unsigned int i = 1; i < D_.size(); ++i) {
            if (i >= 2) {
                ost << "   ";
            }
            ost << i << ": ";
            for (unsigned int j = 0; j < D_[i].size(); ++j) {
                ost << D_[i][j] << " ";
            }
            ost << std::endl;
        }
        ost << std::endl;
        ost << "P: ";
        for (unsigned int i = 0; i < P_.size(); ++i) {
            ost << "{" << P_[i].first << ", " << P_[i].second << "} ";
        }
        ost << std::endl;
        ost << "S: ";
        for (unsigned int i = 0; i < S_.size(); ++i) {
            ost << "{" << S_[i].first << ", " << S_[i].second << "} ";
        }
        ost << std::endl;
        ost << "C: ";
        for (unsigned int i = 0; i < C_.size(); ++i) {
            ost << C_[i] << " ";
        }
        ost << std::endl;
        ost << "Q: ";
        for (unsigned int i = 0; i < Q_.size(); ++i) {
            ost << Q_[i] << " ";
        }
        ost << std::endl;

        ost << "T: ";
        for (unsigned int i = 0; i < T_.size(); ++i) {
            ost << T_[i] << " ";
        }
        ost << std::endl;
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

        vset = new std::vector<mate_t>*[2 * n + 1];
		vset[0] = NULL;
        for (int i = 1; i <= 2 * n; ++i) {
            vset[i] = new std::vector<mate_t>(); // need delete somewhere
        }

		swap_vset = new std::vector<mate_t>*[2 * n + 1];
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

template<>
std::string MateFrontier<FrontierDegComp>::GetPreviousString(State* state);
template<>
std::string MateFrontier<FrontierDegComp>::GetNextString(State* state);


} // the end of the namespace

#endif // MATEFGENERAL_HPP
