//
// frontier.cpp
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <cstdio>
#include <ctime>
#include <climits>
#include <string>
#include <fstream>
#include <iomanip>
#include <stdexcept>

//#include <mcheck.h>

#ifdef HAVE_LIBGMPXX
#include <gmpxx.h>
#endif

#include "BigInteger.hpp"
//#include "MateSForest.hpp"
//#include "MateSTree.hpp"
//#include "MateRForest.hpp"
#include "MateSTPath.hpp"
#include "MateSTPathDist.hpp"
#include "MatePartition.hpp"
//#include "MatePathMatching.hpp"
//#include "MateMTPath.hpp"
//#include "MateDSTPath.hpp"
//#include "MateSTEDPath.hpp"
//#include "MateComponent.hpp"
//#include "MateKcut.hpp"
//#include "MateRcut.hpp"
//#include "MateSetPartition.hpp"
//#include "MateSetCover.hpp"
//#include "MateSetPacking.hpp"
#include "MateHyperSForest.hpp"
#include "MateHyperReli.hpp"
#include "RootManager.hpp"
//#include "Automaton.hpp"

using namespace std;
using namespace frontier_dd;

#ifdef HAVE_LIBGMPXX
static double GetDouble(MpInt num)
{
    return num.get_d();
}
#endif

static double GetDouble(uintx num)
{
    return static_cast<double>(num);
}

static double GetDouble(BigInteger num)
{
    return static_cast<double>(num);
}

template <typename T>
static void PrintNumberOfSolutions(PseudoZDD* zdd, bool is_compute_solution, bool)
{
    T num;
    num = zdd->ComputeNumberOfSolutions<T>();
    if (is_compute_solution) {
        cerr << "# of solutions = " << num;
        double dnum = GetDouble(num);
        if (dnum >= 1.0e+06) {
            cerr << " (" << dnum << ")";
        }
        cerr << endl;
    }
}

template <>
void PrintNumberOfSolutions<double>(PseudoZDD* zdd, bool is_compute_solution, bool)
{
    double num;
    num = zdd->ComputeNumberOfSolutions<double>();
    if (is_compute_solution) {
        cerr << setprecision(15) << "# of solutions = " << num << endl;
    }
}

int main(int argc, char** argv)
{
    int enum_kind = 0;
    int loading_kind = 0;
    int print_kind = 0;
    string input_filename = "";
    bool is_print_graph = false; // print the input graph
    string print_graph_filename = "";
    bool is_print_graphviz = false; // print the input graph for graphviz
    string print_graphviz_filename = "";
    bool is_breadth_first = false; // rename vertex numbers by the breadth-first search if true
    int start_vertex = 1; // start vertex number for (directed / undirected) s-t path
    int end_vertex = -1;  // end vertex number for (directed / undirected) s-t path
    bool is_hamilton = false; // use all vertices for an s-t path or a cycle
    bool is_cycle = false;    // if this option is true and enum_kind == STPATH or DSTPATH,
                             // enumerate cycles
    bool is_any_path = false; // enumerate paths from start_vertex to any vertex
    string terminal_filename = "";
    bool is_set_weight = false;
    string weight_filename = "";
    bool is_use_upper = false;
    double upper_bound = 99999999.0;
    bool is_le = false; // for enumeration of components
    bool is_me = false; // for enumeration of components
    bool is_reduce = false; // reduce the ZDD
    bool is_print_pzdd = true; // print the (pseudo) ZDD
    bool is_print_zdd_graphviz = false; // print the (pseudo) ZDD for graphviz
    bool is_print_zero = false; // print zero-terminal if true
    string print_zdd_graphviz_filename = "";
    bool is_print_zdd_sbdd = false; // print the (pseudo) ZDD for SapporoBDD
    string print_zdd_sbdd_filename = "";
    bool is_compute_solution = true; // compute and print the number of solutions of the
                                   // constructed ZDD
    bool is_enum = false; // enumerate and output all solutions
    string enum_filename = "";
    bool is_sample = false; // sample solutions randomly
    string sample_filename = "";
    int sample_num = 100; // the number of solutions to sample
    bool is_random_graphviz = false;
    bool is_hex = false; // hexadecimal ZDD node ID
    //bool is_use_automaton = false; // construct the automaton from the ZDD
    //bool is_print_am = false; // print the constructed automaton
    int precision_kind = 0;
    bool is_print_progress = false;

    RootManager root_mgr;

    enum {
        SFOREST, // spanning forest
        STREE,   // spanning tree
        STPATH,  // s-t path or cycle
        PATHMATCHING, // path-matching
        MTPATH, // multi-terminal path
        DSTPATH, // directed s-t path or cycle
        STEDPATH, // s-t edge-disjoint path
        RFOREST, // rooted (spanning) forest
        PARTITION,    // partition
        KCUT,    // k-cut
        RCUT,    // rooted k-cut
        SETPT,   // set partition on a hypergraph
        SETC,    // set cover
        SETPK,   // set packing
        HFOREST, // hyper forest 
        HRELI,   // hyper reli
    };

    enum { // loading_kind
        ADJ_LIST,
        ADJ_MATRIX,
        INC_MATRIX,
        EDGE_LIST,
    };

    enum { // precision_kind
        AUTO,
        INTX,
        DOUBLE,
        BIGINT,
        GMP
    };

    //mtrace(); // for debug

    srand(static_cast<unsigned int>(time(NULL)));

    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];
        if (arg == "-t") {
            if (i + 1 < argc) {
                string kind = argv[i + 1];
                if (kind == "sforest") {
                    enum_kind = SFOREST;
                } else if (kind == "stree") {
                    enum_kind = STREE;
                } else if (kind == "stpath") {
                    enum_kind = STPATH;
                } else if (kind == "stpathdist") { // obsolete
                    enum_kind = STPATH;
                } else if (kind == "pathmatching" || kind == "pm") {
                    enum_kind = PATHMATCHING;
                } else if (kind == "mtpath" || kind == "numberlink") {
                    enum_kind = MTPATH;
                } else if (kind == "dstpath") {
                    enum_kind = DSTPATH;
                } else if (kind == "stedpath") {
                    enum_kind = STEDPATH;
                } else if (kind == "rforest") {
                    enum_kind = RFOREST;
                } else if (kind == "partition" || kind == "part") {
                    enum_kind = PARTITION;
                } else if (kind == "kcut") {
                    enum_kind = KCUT;
                } else if (kind == "rcut") {
                    enum_kind = RCUT;
                } else if (kind == "setpt") {
                    enum_kind = SETPT;
                } else if (kind == "setc") {
                    enum_kind = SETC;
                } else if (kind == "setpk") {
                    enum_kind = SETPK;
                } else if (kind == "hforest") {
                    enum_kind = HFOREST;
                } else if (kind == "hreli") {
                    enum_kind = HRELI;
                } else {
                    cerr << "Error: unknown enum_type: " << kind << "." << endl;
                    exit(1);
                }
            }
            ++i;
        } else if (arg == "--input") {
            if (i + 1 < argc) {
                input_filename = argv[i + 1];
                ++i;
            } else {
                cerr << "Error: need filename after --input." << endl;
                exit(1);
            }
        } else if (arg == "--print-graph" || arg == "--print-graph-al") {
            is_print_graph = true;
            print_kind = ADJ_LIST;
            if (i + 1 < argc) {
                print_graph_filename = argv[i + 1];
                ++i;
            }
        } else if (arg == "--print-graph-am") {
            is_print_graph = true;
            print_kind = ADJ_MATRIX;
            if (i + 1 < argc) {
                print_graph_filename = argv[i + 1];
                ++i;
            }
        } else if (arg == "--print-graph-im") {
            is_print_graph = true;
            print_kind = INC_MATRIX;
            if (i + 1 < argc) {
                print_graph_filename = argv[i + 1];
                ++i;
            }
        } else if (arg == "--print-graph-el") {
            is_print_graph = true;
            print_kind = EDGE_LIST;
            if (i + 1 < argc) {
                print_graph_filename = argv[i + 1];
                ++i;
            }
        } else if (arg == "--print-graphviz") {
            is_print_graphviz = true;
            if (i + 1 < argc) {
                print_graphviz_filename = argv[i + 1];
                ++i;
            }
        } else if (arg == "--input-al") {
            loading_kind = ADJ_LIST;
        } else if (arg == "--input-am") {
            loading_kind = ADJ_MATRIX;
        } else if (arg == "--input-im") {
            loading_kind = INC_MATRIX;
        } else if (arg == "--input-el" || arg == "-c") {
            loading_kind = EDGE_LIST;
        //} else if (arg == "-w" || arg == "--weight") {
        //    is_set_weight = true;
        //    if (i + 1 < argc) {
        //        weight_filename = argv[i + 1];
        //        ++i;
        //    }
        } else if (arg == "-b") {
            is_breadth_first = true;
        } else if (arg == "-s") {
            if (i + 1 < argc) {
                start_vertex = atoi(argv[i + 1]);
            }
            ++i;
        } else if (arg == "-e") {
            if (i + 1 < argc) {
                end_vertex = atoi(argv[i + 1]);
            }
            ++i;
        } else if (arg == "-h" || arg == "--hamilton") {
            is_hamilton = true;
        } else if (arg == "--cycle") {
            is_cycle = true;
        } else if (arg == "--any") {
            is_any_path = true;
        } else if (arg == "--terminal") {
            if (i + 1 < argc) {
                terminal_filename = argv[i + 1];
                ++i;
            }
        } else if (arg == "-k" || arg == "--upper") {
            is_use_upper = true;
            upper_bound = atof(argv[i + 1]);
            ++i;
        } else if (arg == "--le") {
            is_le = true;
        } else if (arg == "--me") {
            is_me = true;
        } else if (arg == "-f") {
            // -f オプションの後は "-f 1 3 7 9" のように整数が並ぶので、それをパースする。
            while (i + 1 < argc && argv[i + 1][0] != '-') {
                root_mgr.Add(atoi(argv[i + 1]));
                ++i;
            }
            if (root_mgr.GetSize() == 0) {
                cerr << "Error: need integer(s) after -f." << endl;
                exit(1);
            }
        } else if (arg == "--root") {
            if (i + 1 < argc) {
                root_mgr.Parse(string(argv[i + 1]));
                ++i;
            }
            if (root_mgr.GetSize() == 0) {
                cerr << "Error: cannot read roots from file." << endl;
                exit(1);
            }
        } else if (arg == "-r") {
            is_reduce = true;
        } else if (arg == "-n" || arg == "--no-print-zdd") {
            is_print_pzdd = false;
        } else if (arg == "--print-zdd-graphviz") {
            is_print_zdd_graphviz = true;
            if (i + 1 < argc) {
                print_zdd_graphviz_filename = argv[i + 1];
                ++i;
            }
            // parse if the next argument is an integer
            if (i + 1 < argc && argv[i + 1][0] == '0') {
                is_print_zero = true;
                ++i;
            }
        } else if (arg == "--print-zdd-sbdd") {
            is_reduce = true;
            is_print_zdd_sbdd = true;
            if (i + 1 < argc) {
                print_zdd_sbdd_filename = argv[i + 1];
                ++i;
            }
        } else if (arg == "--no-solution") {
            is_compute_solution = false;
        } else if (arg == "--si") {
            precision_kind = INTX;
        } else if (arg == "--sd") {
            precision_kind = DOUBLE;
        } else if (arg == "--sb") {
            precision_kind = BIGINT;
        } else if (arg == "--sa") {
            cerr << "The option of --sa (using apfloat) is obsolete." << endl;
            exit(1);
        } else if (arg == "--sm") {
#ifdef HAVE_LIBGMPXX
            precision_kind = GMP;
#else
            cerr << "Please run ./configure and make after installing gmp and gmpxx." << endl;
            exit(1);
#endif
        } else if (arg == "--enum") {
            is_enum = true;
            if (i + 1 < argc) {
                enum_filename = argv[i + 1];
                ++i;
            }
        } else if (arg == "--sample") {
            is_sample = true;
            if (i + 1 < argc) {
                sample_filename = argv[i + 1];
                ++i;
            }
            // parse if the next argument is an integer
            if (i + 1 < argc && '0' <= argv[i + 1][0] && argv[i + 1][0] <= '9') {
                sample_num = atoi(argv[i + 1]);
                ++i;
            }
        } else if (arg == "--random-graphviz") {
            is_random_graphviz = true;
        } else if (arg == "--hex") {
            is_hex = true;
        //} else if (arg == "--am") {
        //    is_use_automaton = true;
        //} else if (arg == "--print-am") {
        //    is_print_am = true;
        } else if (arg == "-v") {
            is_print_progress = true;
        } else if (arg == "--version") {
#ifdef HAVE_CONFIG_H
            cout << PACKAGE_NAME << " version " << PACKAGE_VERSION;
#else
            cout << "frontier version unknown";
#endif

#ifdef HAVE_LIBGMPXX
            cout << " with GNU MP library";
#endif
            cout << endl;
            exit(0);
        } else if (arg == "--help") {
            cout << "Under construction..." << endl;
            return 0;
        } else {
            cerr << "Error: unknown option: " << arg << "." << endl;
            exit(1);
        }
    }

    Graph* graph = NULL;
    HyperGraph* hgraph = NULL;
    GraphInterface* igraph = NULL;

    switch (enum_kind) {
    case SFOREST:
    case STREE:
    case STPATH:
    case PATHMATCHING:
    case MTPATH:
    case DSTPATH:
    case STEDPATH:
    case RFOREST:
    case PARTITION:
    case KCUT:
    case RCUT:
        graph = new Graph();
        igraph = graph;
        break;
    case SETPT:
    case SETC:
    case SETPK:
    case HFOREST:
    case HRELI:
        hgraph = new HyperGraph();
        igraph = hgraph;
        if (loading_kind == 0) {
            loading_kind = EDGE_LIST;
        }
        break;
    }

    istream* input_is;
    ifstream* input_ifs = NULL;

    if (input_filename != "" && input_filename != "-") {
        input_ifs = new ifstream;
        input_is = input_ifs;
        input_ifs->open(input_filename.c_str()); // FIX ME! need error check
    } else {
        input_is = &cin;
    }

    bool is_directed = (enum_kind == DSTPATH);
    switch (loading_kind) {
    case ADJ_LIST:
        igraph->LoadAdjacencyList(*input_is, is_directed);
        break;
    case ADJ_MATRIX:
        igraph->LoadAdjacencyMatrix(*input_is, is_directed);
        break;
    case INC_MATRIX:
        igraph->LoadIncidenceMatrix(*input_is, is_directed);
        break;
    case EDGE_LIST:
        igraph->LoadEdgeList(*input_is, is_directed);
        break;
    }

    if (input_ifs != NULL) {
        input_ifs->close();
        delete input_ifs;
        input_is = input_ifs = NULL;
    }

    cerr << "# of vertices: " << igraph->GetNumberOfVertices()
        << ", # of edges: " << igraph->GetNumberOfEdges() << endl;

    if (is_set_weight) {
        ifstream ifs(weight_filename.c_str());
        igraph->SetWeightToEach(ifs);
    }

    if (is_breadth_first) {
        if (graph != NULL) {
            graph->RearrangeByBreadthFirst(start_vertex);
        }
    }

    if (enum_kind == STPATH || enum_kind == DSTPATH
        || enum_kind == STEDPATH) {
        if (end_vertex < 0) { // 終端に指定が無ければ
            end_vertex = graph->GetNumberOfVertices(); // 終端を最後にする
        }
        if (is_any_path) {
            graph->AddDummyVertex(); // ダミーの頂点を加える
            end_vertex = graph->GetNumberOfVertices();
        }
    }

    if (is_print_graph) {
        ostream* ost;
        ofstream ofs;
        if (print_graph_filename == "") {
            cerr << "Please input a filename for print_graph." << endl;
            exit(1);
        } else if (print_graph_filename == "-") {
            ost = &cout;
        } else {
            ofs.open(print_graph_filename.c_str());
            ost = &ofs;
        }
        switch (print_kind) {
        case ADJ_LIST:
            igraph->PrintAdjacencyList(*ost);
            break;
        case ADJ_MATRIX:
            igraph->PrintAdjacencyMatrix(*ost);
            break;
        case INC_MATRIX:
            igraph->PrintIncidenceMatrix(*ost);
            break;
        case EDGE_LIST:
            igraph->PrintEdgeList(*ost);
            break;
        }
        ofs.close();
        return 0;
    }

    if (is_print_graphviz) {
        if (print_graphviz_filename == "") {
            cerr << "Please input a filename for print_graphviz." << endl;
            exit(1);
        } else if (print_graphviz_filename == "-") {
            igraph->PrintForGraphviz(cout);
            return 0;
        } else {
            ofstream ofs(print_graphviz_filename.c_str());
            igraph->PrintForGraphviz(ofs);
            return 0;
        }
    }

    State* state = NULL;

    switch (enum_kind) {
    case SFOREST:
        cerr << "not implemented yet." << endl;
        exit(1);
        //state = new StateSForest(graph);
        break;
    case STREE:
        cerr << "not implemented yet." << endl;
        exit(1);
        //state = new StateSTree(graph);
        break;
    case STPATH:
    case DSTPATH:
        if (enum_kind == STPATH) {
            if (is_use_upper) {
                state = new StateSTPathDist(graph);
            } else {
                state = new StateSTPath(graph);
            }
        } else {
            cerr << "not implemented yet." << endl;
            exit(1);
            //state = new StateDSTPath(graph);
        }
        if (is_use_upper) {
            static_cast<StateSTPathDist*>(state)->SetStartAndEndVertex(start_vertex, end_vertex);
            static_cast<StateSTPathDist*>(state)->SetHamilton(is_hamilton);
            static_cast<StateSTPathDist*>(state)->SetCycle(is_cycle);
            static_cast<StateSTPathDist*>(state)->SetMaxDistance(upper_bound);
        } else {
            static_cast<StateSTPath*>(state)->SetStartAndEndVertex(start_vertex, end_vertex);
            static_cast<StateSTPath*>(state)->SetHamilton(is_hamilton);
            static_cast<StateSTPath*>(state)->SetCycle(is_cycle);
        }
        break;
    case PATHMATCHING:
        cerr << "not implemented yet." << endl;
        exit(1);
        //state = new StatePathMatching(graph);
        //static_cast<StatePathMatching*>(state)->SetHamilton(is_hamilton);
        break;
    case MTPATH:
        cerr << "not implemented yet." << endl;
        exit(1);
        //state = new StateMTPath(graph);
        //static_cast<StateMTPath*>(state)->SetHamilton(is_hamilton);
        //static_cast<StateMTPath*>(state)->ParseTerminal(terminal_filename);
        break;
    case STEDPATH:
        cerr << "not implemented yet." << endl;
        exit(1);
        //state = new StateSTEDPath(graph);
        //static_cast<StateSTEDPath*>(state)->SetStartAndEndVertex(start_vertex, end_vertex);
        //static_cast<StateSTEDPath*>(state)->SetHamilton(is_hamilton);
        //static_cast<StateSTEDPath*>(state)->SetCycle(is_cycle);
        break;
    case RFOREST:
        cerr << "not implemented yet." << endl;
        exit(1);
        //state = new StateRForest(graph);
        //static_cast<StateRForest*>(state)->SetRootManager(&root_mgr);
        break;
    case PARTITION:
        state = new StatePartition(graph, (is_use_upper ? static_cast<short>(upper_bound) :
            SHRT_MAX), is_le, is_me);
        break;
    case KCUT:
        cerr << "not implemented yet." << endl;
        exit(1);

        //state = new StateKcut(graph, upper_bound);
        break;
    case RCUT:
        cerr << "not implemented yet." << endl;
        exit(1);

        //state = new StateRcut(graph, upper_bound);
        //static_cast<StateRcut*>(state)->SetRootManager(&root_mgr);
        break;
    case SETPT:
        cerr << "not implemented yet." << endl;
        exit(1);

        //state = new StateSetPartition(hgraph);
        break;
    case SETC:
        cerr << "not implemented yet." << endl;
        exit(1);

        //state = new StateSetCover(hgraph);
        break;
    case SETPK:
        cerr << "not implemented yet." << endl;
        exit(1);

        //state = new StateSetPacking(hgraph);
        break;
    case HFOREST:
        state = new StateHyperSForest(hgraph);
        break;
    case HRELI:
        state = new StateHyperReli(hgraph);
        static_cast<StateHyperReli*>(state)->SetRootManager(&root_mgr);
        break;
    }
    state->SetPrintProgress(is_print_progress);

    PseudoZDD* zdd = FrontierAlgorithm::Construct(state); // アルゴリズム開始

    state->FlushPrint();

    intx number_of_nodes = zdd->GetNumberOfNodes();

    if (is_reduce) {
        zdd->ReduceAsZDD(); // ZDDの既約化
    }

    if (is_print_pzdd) {
        zdd->OutputZDD(stdout, is_hex); // ZDDを標準出力に出力する
    }

    if (is_print_zdd_graphviz) {
        if (print_zdd_graphviz_filename == "") {
            cerr << "Please input a filename for print_zdd_graphviz." << endl;
            exit(1);
        } else if (print_zdd_graphviz_filename == "-") {
            zdd->OutputZDDForGraphviz(cout, is_print_zero);
        } else {
            ofstream ofs(print_zdd_graphviz_filename.c_str());
            zdd->OutputZDDForGraphviz(ofs, is_print_zero);
        }
    }

    if (is_print_zdd_sbdd) {
        if (print_zdd_sbdd_filename == "") {
            cerr << "Please input a filename for print_zdd_sbdd." << endl;
            exit(1);
        } else if (print_zdd_sbdd_filename == "-") {
            zdd->OutputZDDForSapporoBDD(cout);
        } else {
            ofstream ofs(print_zdd_sbdd_filename.c_str());
            zdd->OutputZDDForSapporoBDD(ofs);
        }
    }

    cerr << "# of nodes of ZDD = " << number_of_nodes << endl;

    if (is_reduce) {
        cerr << "# of nodes of reduced ZDD = " << zdd->GetNumberOfNodes() << endl;
    }

    if (is_compute_solution || is_sample) {
        switch (precision_kind) {
        case INTX:
            PrintNumberOfSolutions<uintx>(zdd, is_compute_solution, false);
            break;
        case DOUBLE:
            PrintNumberOfSolutions<double>(zdd, is_compute_solution, false);
            break;
        case BIGINT:
            PrintNumberOfSolutions<BigInteger>(zdd, is_compute_solution, false);
            break;
        case GMP:
            PrintNumberOfSolutions<MpInt>(zdd, is_compute_solution, false);
            break;
        default:
#ifdef HAVE_LIBGMPXX
            PrintNumberOfSolutions<MpInt>(zdd, is_compute_solution, false);
#else
            PrintNumberOfSolutions<BigInteger>(zdd, is_compute_solution, false);
#endif
            break;
        }
    }

    if (is_enum) {
        if (enum_filename == "") {
            cerr << "Please input a filename for enum." << endl;
            exit(1);
        } else if (enum_filename == "-") {
            zdd->OutputAllSolutions(stdout); // 全解を標準出力に出力する
        } else {
            FILE* fout = fopen(enum_filename.c_str(), "w");
            if (fout == NULL) {
                cerr << "file open error!" << endl;
                exit(1);
            }
            zdd->OutputAllSolutions(fout); // 全解を標準出力に出力する
            fclose(fout);
        }
    }

    if (is_sample) {
        if (sample_filename == "") {
            cerr << "Please input a filename for sample." << endl;
            exit(1);
        } else if (sample_filename == "-") {
            zdd->OutputSamplingSolutions(stdout, sample_num); // random sampling
        } else {
            FILE* fout = fopen(sample_filename.c_str(), "w");
            if (fout == NULL) {
                cerr << "file open error!" << endl;
                exit(1);
            }
            zdd->OutputSamplingSolutions(fout, sample_num); // random sampling
            fclose(fout);
        }
    }

    if (is_random_graphviz) {
        vector<int> ar;
        zdd->SampleUniformlyRandomly(&ar);
        graph->PrintForGraphviz(cout, ar);
    }

    //if (is_use_automaton) { // オートマトン化に関する処理
        //AutomatonManager am_mgr(zdd);
        //Automaton* am = am_mgr.MakeAutomaton3(); // ZDDをオートマトンに変換
        //cerr << "# of states = " << am_mgr.GetNumberOfStates() << endl;

    //    if (is_print_am) {
    //        //am->PrintForGraphviz(); // Graphviz 用出力
    //    }
    //}

    delete zdd; // 後処理

    if (state != NULL) {
        delete state;
    }

    if (hgraph != NULL) {
        delete hgraph;
    }
    if (graph != NULL) {
        delete graph;
    }
    //muntrace(); // for debug

    return 0;
}
