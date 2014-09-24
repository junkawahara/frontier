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

#include "PseudoZDD.hpp"
#include "BigInteger.hpp"
#include "RootManager.hpp"

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
    string gen_parameter_filename = ""; // for FGeneral
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
    bool is_input_hex = false;
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
        GENERAL, // generalized frontier-based method
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
        if (arg == "--input") {
            if (i + 1 < argc) {
                input_filename = argv[i + 1];
                ++i;
            } else {
                cerr << "Error: need filename after --input." << endl;
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
        } else if (arg == "--input-hex") {
            is_input_hex = true;
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

    istream* input_is;
    ifstream* input_ifs = NULL;

    if (input_filename != "" && input_filename != "-") {
        input_ifs = new ifstream;
        input_is = input_ifs;
        input_ifs->open(input_filename.c_str()); // FIX ME! need error check
    } else {
        input_is = &cin;
    }

    PseudoZDD* zdd = new PseudoZDD();
    zdd->ImportZDD(*input_is, is_input_hex);

    if (input_ifs != NULL) {
        input_ifs->close();
        delete input_ifs;
        input_is = input_ifs = NULL;
    }

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

    //if (is_random_graphviz) {
    //    vector<int> ar;
    //    zdd->SampleUniformlyRandomly(&ar);
    //    graph->PrintForGraphviz(cout, ar);
    //}

    delete zdd; // 後処理

    //muntrace(); // for debug

    return 0;
}
