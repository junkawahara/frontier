//
// OptionParser.hpp
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

#ifndef OPTIONPARSER_HPP
#define OPTIONPARSER_HPP

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


#include "../frontier_lib/RootManager.hpp"
#include "../frontier_lib/BigInteger.hpp"
#include "../frontier_lib/HyperGraph.hpp"
#include "../frontier_lib/FrontierAlgorithm.hpp"
//#include "../frontier_lib/Automaton.hpp"

#include "StateCombination.hpp"
#include "StateSForest.hpp"
#include "StateSTree.hpp"
#include "StateRForest.hpp"
#include "StateSTPath.hpp"
#include "StateSTPathDist.hpp"
#include "StatePathMatching.hpp"
#include "StateMTPath.hpp"
//#include "StateDSTPath.hpp"
//#include "StateSTEDPath.hpp"
#include "StateKcut.hpp"
#include "StateRcut.hpp"
#include "StateSetPartition.hpp"
#include "StateSetCover.hpp"
#include "StateSetPacking.hpp"

namespace frontier_lib {

class NumPrinter {

public:

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
            std::cerr << "# of solutions = " << num;
            double dnum = GetDouble(num);
            if (dnum >= 1.0e+06) {
                std::cerr << " (" << dnum << ")";
            }
            std::cerr << std::endl;
        }
    }
};

template <>
inline void NumPrinter::PrintNumberOfSolutions<double>(PseudoZDD* zdd, bool is_compute_solution, bool)
{
    double num;
    num = zdd->ComputeNumberOfSolutions<double>();
    if (is_compute_solution) {
        std::cerr << std::setprecision(15) << "# of solutions = " << num << std::endl;
    }
}


class OptionParser {
public:
    Graph* graph;
    HyperGraph* hgraph;
    GraphInterface* igraph;

    State* state;

    int enum_kind;
    int loading_kind;
    int print_kind;
    std::string input_filename;
    bool is_print_graph; // print the input graph
    std::string print_graph_filename;
    bool is_print_graphviz; // print the input graph for graphviz
    std::string print_graphviz_filename;
    std::string gen_parameter_filename; // for FGeneral
    bool is_breadth_first; // rename vertex numbers by the breadth-first search if true
    int start_vertex; // start vertex number for (directed / undirected) s-t path
    int end_vertex;  // end vertex number for (directed / undirected) s-t path
    bool is_hamilton; // use all vertices for an s-t path or a cycle
    bool is_cycle;    // if this option is true and enum_kind == STPATH or DSTPATH,
                             // enumerate cycles
    bool is_any_path; // enumerate paths from start_vertex to any vertex
    std::string terminal_filename;
    bool is_set_weight;
    std::string weight_filename;
    bool is_set_vertex_weight;
    std::string vertex_weight_filename;
    std::pair<int, int> vertex_weight_limit;
    std::pair<int, int> edge_weight_limit;
    std::pair<short, short> component_limit;
    bool is_use_ratio;
    double ratio;
    bool is_use_elimit;
    //double upper_bound;
    //bool is_le; // for enumeration of components
    //bool is_me; // for enumeration of components
    int max_vertex_weight;
    bool is_ignore_isolated;
    bool is_print_parameter;
    bool is_reduce; // reduce the ZDD
    bool is_switch; // switch arcs
    bool is_print_pzdd; // print the (pseudo) ZDD
    bool is_print_zdd_graphviz; // print the (pseudo) ZDD for graphviz
    bool is_print_zero; // print zero-terminal if true
    std::string print_zdd_graphviz_filename;
    bool is_print_zdd_sbdd; // print the (pseudo) ZDD for SapporoBDD
    std::string print_zdd_sbdd_filename;
    bool is_compute_solution; // compute and print the number of solutions of the
                                   // constructed ZDD
    bool is_enum; // enumerate and output all solutions
    std::string enum_filename;
    bool is_sample; // sample solutions randomly
    std::string sample_filename;
    int sample_num; // the number of solutions to sample
    bool is_random_graphviz;
    bool is_hex; // hexadecimal ZDD node ID
    //bool is_use_automaton; // construct the automaton from the ZDD
    //bool is_print_am; // print the constructed automaton
    int precision_kind;
    bool is_print_progress;
    RootManager root_mgr;

    enum {
        COMBINATION, // combination
        SFOREST, // spanning forest
        STREE,   // spanning tree
        STPATH,  // s-t path or cycle
        PATHMATCHING, // path-matching
        MTPATH, // multi-terminal path
        DSTPATH, // directed s-t path or cycle
        STEDPATH, // s-t edge-disjoint path
        RFOREST, // rooted (spanning) forest
        PARTITION,    // partition
        VERTEXWEIGHT,
        KCUT,    // k-cut
        RCUT,    // rooted k-cut
        GENERAL, // generalized frontier-based method
        SETPT,   // set partition on a hypergraph
        SETC,    // set cover
        SETPK,   // set packing
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

    OptionParser()
    {
        graph = NULL;
        hgraph = NULL;
        igraph = NULL;
        state = NULL;

        enum_kind = 0;
        loading_kind = 0;
        print_kind = 0;
        input_filename = "";
        is_print_graph = false; 
        print_graph_filename = "";
        is_print_graphviz = false; 
        print_graphviz_filename = "";
        gen_parameter_filename = ""; 
        is_breadth_first = false; 
        start_vertex = 1; 
        end_vertex = -1;  
        is_hamilton = false; 
        is_cycle = false;    
        is_any_path = false; 
        terminal_filename = "";
        is_set_weight = false;
        weight_filename = "";
        is_set_vertex_weight = false;
        vertex_weight_filename = "";
        vertex_weight_limit = std::make_pair(0, INT_MAX);
        edge_weight_limit = std::make_pair(0, INT_MAX);
        component_limit = std::make_pair(0, SHRT_MAX);
        is_use_ratio = false;
        ratio = 99999999.0;
        is_use_elimit = false;
        //upper_bound = 99999999.0;
        //is_le = false; 
        //is_me = false; 
        //max_vertex_weight = -1;
        is_ignore_isolated = false;
        is_print_parameter = false;
        is_reduce = false; 
        is_switch = false; 
        is_print_pzdd = true; 
        is_print_zdd_graphviz = false; 
        is_print_zero = false; 
        print_zdd_graphviz_filename = "";
        is_print_zdd_sbdd = false; 
        print_zdd_sbdd_filename = "";
        is_compute_solution = true;         
        is_enum = false; 
        enum_filename = "";
        is_sample = false; 
        sample_filename = "";
        sample_num = 100; 
        is_random_graphviz = false;
        is_hex = false; 
        //is_use_automaton = false;
        //is_print_am = false;
        precision_kind = 0;
        is_print_progress = false;
    }

    ~OptionParser()
    {
        if (state != NULL) {
            delete state;
        }
        if (hgraph != NULL) {
            delete hgraph;
        }
        if (graph != NULL) {
            delete graph;
        }
    }

    void ParseOption(int argc, char** argv)
    {
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "-t") {
                if (i + 1 < argc) {
                    std::string kind = argv[i + 1];
                    if (kind == "combination" || kind == "comb") {
                        enum_kind = COMBINATION;
                    } else if (kind == "sforest") {
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
                    } else if (kind == "vertexweight" || kind == "vw") {
                        enum_kind = VERTEXWEIGHT;
                    } else if (kind == "kcut") {
                        enum_kind = KCUT;
                    } else if (kind == "rcut") {
                        enum_kind = RCUT;
                    } else if (kind == "general") {
                        enum_kind = GENERAL;
                    } else if (kind == "setpt") {
                        enum_kind = SETPT;
                    } else if (kind == "setc") {
                        enum_kind = SETC;
                    } else if (kind == "setpk") {
                        enum_kind = SETPK;
                    } else {
                        std::cerr << "Error: unknown enum_type: " << kind << "." << std::endl;
                        exit(1);
                    }
                }
                ++i;
            } else if (arg == "--input") {
                if (i + 1 < argc) {
                    input_filename = argv[i + 1];
                    ++i;
                } else {
                    std::cerr << "Error: need filename after --input." << std::endl;
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
            } else if (arg == "-w" || arg == "--weight" || arg == "--edge-weight") {
                is_set_weight = true;
                if (i + 1 < argc) {
                    weight_filename = argv[i + 1];
                    ++i;
                }
            } else if (arg == "--vertex-weight") {
                is_set_vertex_weight = true;
                if (i + 1 < argc) {
                    vertex_weight_filename = argv[i + 1];
                    ++i;
                }
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
            } else if (arg == "--vlimit") {
                if (i + 1 < argc) {
                    ParseRange(argv[i + 1], &vertex_weight_limit);
                    ++i;
                }
            } else if (arg == "--elimit") {
                is_use_elimit = true;
                if (i + 1 < argc) {
                    ParseRange(argv[i + 1], &edge_weight_limit);
                    ++i;
                }
            } else if (arg == "--ratio") {
                is_use_ratio = true;
                if (i + 1 < argc) {
                    ratio = atof(argv[i + 1]);
                    ++i;
                }
            } else if (arg == "-k") {
                std::cerr << "-k option is obsolete." << std::endl;
                exit(1);
            } else if (arg == "--upper") {
                std::cerr << "-k option is obsolete." << std::endl;
                exit(1);
            } else if (arg == "--le") {
                std::cerr << "--le option is obsolete." << std::endl;
                exit(1);
            } else if (arg == "--me") {
                std::cerr << "--me option is obsolete." << std::endl;
                exit(1);
            } else if (arg == "--comp") {
                if (i + 1 < argc) {
                    ParseRange(argv[i + 1], &component_limit);
                    ++i;
                }                
            } else if (arg == "--max-vw" || arg == "--max-vertex-weight") {
                std::cerr << "--max-vertex-weight option is obsolete. Use --vlimit instead." << std::endl;
                exit(1);
            } else if (arg == "--ignore-isolated") {
                is_ignore_isolated = true;
            } else if (arg == "--print-parameter") {
                is_print_parameter = true;
            } else if (arg == "-f") {
                // -f オプションの後は "-f 1 3 7 9" のように整数が並ぶので、それをパースする。
                while (i + 1 < argc && argv[i + 1][0] != '-') {
                    root_mgr.Add(atoi(argv[i + 1]));
                    ++i;
                }
                if (root_mgr.GetSize() == 0) {
                    std::cerr << "Error: need integer(s) after -f." << std::endl;
                    exit(1);
                }
            } else if (arg == "--root") {
                if (i + 1 < argc) {
                    root_mgr.Parse(std::string(argv[i + 1]));
                    ++i;
                }
                if (root_mgr.GetSize() == 0) {
                    std::cerr << "Error: cannot read roots from file." << std::endl;
                    exit(1);
                }
            } else if (arg == "-r") {
                is_reduce = true;
            } else if (arg == "--switch") {
                is_switch = true;
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
            } else if (arg == "--parameter") {
                if (i + 1 < argc) {
                    gen_parameter_filename = argv[i + 1];
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
                std::cerr << "The option of --sa (using apfloat) is obsolete." << std::endl;
                exit(1);
            } else if (arg == "--sm") {
#ifdef HAVE_LIBGMPXX
                precision_kind = GMP;
#else
                std::cerr << "Please run ./configure and make after installing gmp and gmpxx." << std::endl;
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
                std::cout << PACKAGE_NAME << " version " << PACKAGE_VERSION;
#else
                std::cout << "frontier version unknown";
#endif

#ifdef HAVE_LIBGMPXX
                std::cout << " with GNU MP library";
#endif
                std::cout << std::endl;
                exit(0);
            } else if (arg == "--help") {
                std::cout << "Under construction..." << std::endl;
                exit(0);
            } else {
                std::cerr << "Error: unknown option: " << arg << "." << std::endl;
                exit(1);
            }
        }
    }

    void ParseCommandLine(const std::string& line)
    {
        int n = static_cast<int>(line.size());
        char* line_str = new char[n + 1];
        char** ar = new char*[n + 1];
        strncpy(line_str, line.c_str(), n + 1); // + 1 for '\0'
        line_str[n] = '\0'; // just in case
        int count = 1;

        ar[0] = &line_str[n]; // ar[0] points to '\0'

        int pos = 0;
        while (pos < n) {
            while (line_str[pos] == ' ') {
                ++pos;
            }
            if (pos < n) {
                ar[count] = line_str + pos;
                ++count;
            }
            while (pos < n && line_str[pos] != ' ') {
                ++pos;
            }
            if (pos < n) {
                line_str[pos] = '\0';
                ++pos;
            }
        }
        for (int i = 1; i < count; ++i) {
            printf("%s", ar[i]);
            if (i < count - 1) {
                printf(", ");
            }
        }
        printf("\n");

        ParseOption(count, ar);

        delete[] ar;
        delete[] line_str;
    }

    void PrepareGraph()
    {
        switch (enum_kind) {
        case COMBINATION:
        case SFOREST:
        case STREE:
        case STPATH:
        case PATHMATCHING:
        case MTPATH:
        case DSTPATH:
        case STEDPATH:
        case RFOREST:
        case PARTITION:
        case VERTEXWEIGHT:
        case KCUT:
        case RCUT:
        case GENERAL:
            graph = new Graph();
            igraph = graph;
            break;
        case SETPT:
        case SETC:
        case SETPK:
            hgraph = new HyperGraph();
            igraph = hgraph;
            if (loading_kind == 0) {
                loading_kind = EDGE_LIST;
            }
            break;
        }

        std::istream* input_is;
        std::ifstream* input_ifs = NULL;

        if (input_filename != "" && input_filename != "-") {
            input_ifs = new std::ifstream;
            input_is = input_ifs;
            input_ifs->open(input_filename.c_str());
            if (!(*input_ifs)) {
                std::cerr << "cannot open " << input_filename << std::endl;
                exit(1);
            }
        } else {
            input_is = &std::cin;
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

        std::cerr << "# of vertices: " << igraph->GetNumberOfVertices()
             << ", # of edges: " << igraph->GetNumberOfEdges() << std::endl;

        if (is_set_weight) {
            std::ifstream ifs(weight_filename.c_str());
            if (!!ifs) {
                igraph->SetWeightToEach(ifs);
            } else {
                std::cerr << "cannot open " << weight_filename << std::endl;
                exit(1);
            }
        }
        if (is_set_vertex_weight) {
            std::ifstream ifs(vertex_weight_filename.c_str());
            if (!!ifs) {
                igraph->SetVertexWeight(ifs);
            } else {
                std::cerr << "cannot open " << vertex_weight_filename << std::endl;
                exit(1);
            }
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
            std::ostream* ost;
            std::ofstream ofs;
            if (print_graph_filename == "") {
                std::cerr << "Please input a filename for print_graph." << std::endl;
                exit(1);
            } else if (print_graph_filename == "-") {
                ost = &std::cout;
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
            exit(0);
        }

        if (is_print_graphviz) {
            if (print_graphviz_filename == "") {
                std::cerr << "Please input a filename for print_graphviz." << std::endl;
                exit(1);
            } else if (print_graphviz_filename == "-") {
                igraph->PrintForGraphviz(std::cout);
                exit(0);
            } else {
                std::ofstream ofs(print_graphviz_filename.c_str());
                igraph->PrintForGraphviz(ofs);
                exit(0);
            }
        }

        // check the existence of vertices
        if (start_vertex >= 0) {
            if (!igraph->IsIncluded(start_vertex)) {
                std::cerr << "The start vertex is not included in the graph." << std::endl;
                exit(1);
            }
        }
        if (end_vertex >= 0) {
            if (!igraph->IsIncluded(end_vertex)) {
                std::cerr << "The end vertex is not included in the graph." << std::endl;
                exit(1);
            }
        }
        if (root_mgr.GetSize() > 0) {
            for (int i = 0; i < root_mgr.GetSize(); ++i) {
                if (!igraph->IsIncluded(root_mgr.Get(i))) {
                    std::cerr << "Vertex " << root_mgr.Get(i) << " is not included in the graph." << std::endl;
                exit(1);
                }
            }
        }
    }

    void MakeState()
    {
        switch (enum_kind) {
        case COMBINATION:
            state = new StateCombination(graph, edge_weight_limit);
            break;
        case SFOREST:
            state = new StateSForest(graph);
            break;
        case STREE:
            state = new StateSTree(graph);
            break;
        case STPATH:
        case DSTPATH:
            if (enum_kind == STPATH) {
                if (is_use_elimit) {
                    state = new StateSTPathDist(graph);
                } else {
                    state = new StateSTPath(graph);
                }
            } else {
                std::cerr << "not implemented yet." << std::endl;
                exit(1);
                //state = new StateDSTPath(graph);
            }
            if (is_use_elimit) {
                static_cast<StateSTPathDist*>(state)->SetStartAndEndVertex(start_vertex, end_vertex);
                static_cast<StateSTPathDist*>(state)->SetHamilton(is_hamilton);
                static_cast<StateSTPathDist*>(state)->SetCycle(is_cycle);
                static_cast<StateSTPathDist*>(state)->SetELimit(edge_weight_limit);
            } else {
                static_cast<StateSTPath*>(state)->SetStartAndEndVertex(start_vertex, end_vertex);
                static_cast<StateSTPath*>(state)->SetHamilton(is_hamilton);
                static_cast<StateSTPath*>(state)->SetCycle(is_cycle);
            }
            break;
        case PATHMATCHING:
            state = new StatePathMatching(graph);
            static_cast<StatePathMatching*>(state)->SetHamilton(is_hamilton);
            break;
        case MTPATH:
            state = new StateMTPath(graph);
            static_cast<StateMTPath*>(state)->SetHamilton(is_hamilton);
            static_cast<StateMTPath*>(state)->ParseTerminal(terminal_filename);
            break;
        case STEDPATH:
            std::cerr << "not implemented yet." << std::endl;
            exit(1);
            //state = new StateSTEDPath(graph);
            //static_cast<StateSTEDPath*>(state)->SetStartAndEndVertex(start_vertex, end_vertex);
            //static_cast<StateSTEDPath*>(state)->SetHamilton(is_hamilton);
            //static_cast<StateSTEDPath*>(state)->SetCycle(is_cycle);
            break;
        case RFOREST:
            state = new StateRForest(graph);
            static_cast<StateRForest*>(state)->SetRootManager(&root_mgr);
            break;
        case PARTITION:
            std::cerr << "not implemented yet." << std::endl;
            exit(1);
            //state = new StatePartition(graph, component_limit);
            //if (max_vertex_weight >= 0) {
            //    static_cast<StatePartition*>(state)->SetMaxVertexWeight(max_vertex_weight);
            //}
            break;
        case VERTEXWEIGHT:
            std::cerr << "not implemented yet." << std::endl;
            exit(1);
            //if (is_use_ratio) {
            //    state = new StateVertexWeightBound(graph, vertex_weight_limit, ratio);
            //} else {
            //    state = new StateVertexWeight(graph, vertex_weight_limit);
            //}
            break;
        case KCUT:
            state = new StateKcut(graph, edge_weight_limit, component_limit);
            break;
        case RCUT:
            state = new StateRcut(graph, edge_weight_limit);
            static_cast<StateRcut*>(state)->SetRootManager(&root_mgr);
            break;
        case GENERAL:
            std::cerr << "not implemented yet." << std::endl;
            exit(1);
            //if (gen_parameter_filename == "") {
            //    std::cerr << "Please specify a filename for parameter." << std::endl;
            //    exit(1);
            //}
            //state = new StateFGeneral(graph, gen_parameter_filename);
            //if (is_ignore_isolated) {
            //    static_cast<StateFGeneral*>(state)->SetIgnoreIsolated(true);
            //}
            //if (is_print_parameter) {
            //    static_cast<StateFGeneral*>(state)->PrintParameter(std::cerr);
            //}
            break;
        case SETPT:
            state = new StateSetPartition(hgraph);
            break;
        case SETC:
            state = new StateSetCover(hgraph);
            break;
        case SETPK:
            state = new StateSetPacking(hgraph);
            break;
        }
        state->SetPrintProgress(is_print_progress);
    }

    void Output(PseudoZDD* zdd)
    {
        state->FlushPrint();

        intx number_of_nodes = zdd->GetNumberOfNodes();

        if (is_switch) {
            zdd->SwitchArcs();
        }

        if (is_reduce) {
            zdd->ReduceAsZDD(); // ZDDの既約化
        }

        if (is_print_pzdd) {
            zdd->OutputZDD(stdout, is_hex); // ZDDを標準出力に出力する
        }

        if (is_print_zdd_graphviz) {
            if (print_zdd_graphviz_filename == "") {
                std::cerr << "Please input a filename for print_zdd_graphviz." << std::endl;
                exit(1);
            } else if (print_zdd_graphviz_filename == "-") {
                zdd->OutputZDDForGraphviz(std::cout, is_print_zero);
            } else {
                std::ofstream ofs(print_zdd_graphviz_filename.c_str());
                zdd->OutputZDDForGraphviz(ofs, is_print_zero);
            }
        }

        if (is_print_zdd_sbdd) {
            if (print_zdd_sbdd_filename == "") {
                std::cerr << "Please input a filename for print_zdd_sbdd." << std::endl;
                exit(1);
            } else if (print_zdd_sbdd_filename == "-") {
                zdd->OutputZDDForSapporoBDD(std::cout);
            } else {
                std::ofstream ofs(print_zdd_sbdd_filename.c_str());
                zdd->OutputZDDForSapporoBDD(ofs);
            }
        }

        std::cerr << "# of nodes of ZDD = " << number_of_nodes << std::endl;

        if (is_reduce) {
            std::cerr << "# of nodes of reduced ZDD = " << zdd->GetNumberOfNodes() << std::endl;
        }

        if (is_compute_solution || is_sample) {
            switch (precision_kind) {
            case INTX:
                NumPrinter::PrintNumberOfSolutions<uintx>(zdd, is_compute_solution, false);
                break;
            case DOUBLE:
                NumPrinter::PrintNumberOfSolutions<double>(zdd, is_compute_solution, false);
                break;
            case BIGINT:
                NumPrinter::PrintNumberOfSolutions<BigInteger>(zdd, is_compute_solution, false);
                break;
            case GMP:
                NumPrinter::PrintNumberOfSolutions<MpInt>(zdd, is_compute_solution, false);
                break;
            default:
#ifdef HAVE_LIBGMPXX
                NumPrinter::PrintNumberOfSolutions<MpInt>(zdd, is_compute_solution, false);
#else
                NumPrinter::PrintNumberOfSolutions<BigInteger>(zdd, is_compute_solution, false);
#endif
                break;
            }
        }

        if (is_enum) {
            if (enum_filename == "") {
                std::cerr << "Please input a filename for enum." << std::endl;
                exit(1);
            } else if (enum_filename == "-") {
                zdd->OutputAllSolutions(stdout); // 全解を標準出力に出力する
            } else {
                FILE* fout = fopen(enum_filename.c_str(), "w");
                if (fout == NULL) {
                    std::cerr << "file open error!" << std::endl;
                    exit(1);
                }
                zdd->OutputAllSolutions(fout); // 全解を標準出力に出力する
                fclose(fout);
            }
        }

        if (is_sample) {
            if (sample_filename == "") {
                std::cerr << "Please input a filename for sample." << std::endl;
                exit(1);
            } else if (sample_filename == "-") {
                zdd->OutputSamplingSolutions(stdout, sample_num); // random sampling
            } else {
                FILE* fout = fopen(sample_filename.c_str(), "w");
                if (fout == NULL) {
                    std::cerr << "file open error!" << std::endl;
                    exit(1);
                }
                zdd->OutputSamplingSolutions(fout, sample_num); // random sampling
                fclose(fout);
            }
        }

        if (is_random_graphviz) {
            std::vector<int> ar;
            zdd->SampleUniformlyRandomly(&ar);
            graph->PrintForGraphviz(std::cout, ar);
        }

        //if (is_use_automaton) { // オートマトン化に関する処理
        //AutomatonManager am_mgr(zdd);
        //Automaton* am = am_mgr.MakeAutomaton3(); // ZDDをオートマトンに変換
        //std::cerr << "# of states = " << am_mgr.GetNumberOfStates() << std::endl;

        //    if (is_print_am) {
        //        //am->PrintForGraphviz(); // Graphviz 用出力
        //    }
        //}
    }

    template<typename T>
    void ParseRange(char* str, std::pair<T, T>* range)
    {
        std::istringstream iss(str);
        char c = iss.peek();
        int n;

        if (isdigit(c)) {
            iss >> n;
            range->first = n;
            range->second = n;
        } else if (c == '[') {
            iss >> c;
            iss >> n;
            range->first = n;
            iss >> c;
            if (c != ',') {
                throw std::runtime_error("Error: Parse failed.");
            }
            iss >> n;
            range->second = n;
        } else {
            throw std::runtime_error("Error: Parse failed.");
        }
    }
};

} // the end of the namespace

#endif // OPTIONPARSER_HPP
