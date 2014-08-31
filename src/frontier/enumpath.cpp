//
// enumpath.cpp
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

#include <iostream>
#include <cstdio>
#include <cmath>
#include <sstream>

#include "Graph.hpp"
#include "StateFrontier.hpp"
#include "MateSTPath.hpp"
#include "PseudoZDD.hpp"
#include "BigInteger.hpp"
#include "SolutionArray.hpp"
#include "MateFGeneral.hpp"

//#include <mcheck.h>

using namespace std;
using namespace frontier_dd;

int main(int argc, char** argv)
{
    //mtrace(); // for debug

    bool is_print_zdd = true;
    bool is_enum = false;
    bool is_reduce_as_zdd = false;
    int start_vertex = 1, end_vertex = -1;
    double max_distance = 9999999.0;

    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];
        if (arg == "--no-print-zdd" || arg == "-n") {
            is_print_zdd = false;
        } else if (arg == "--enum") {
            is_enum = true;
        } else if (arg == "-r") {
            is_reduce_as_zdd = true;
        } else if (arg == "-h" || arg == "--help") {
            cout << "Usage: " << argv[0] << " [-n] [-s N] [-e N] [-r] [-h] [--enum]" << endl;
            cout << "  The input format is an adjacency list of a graph." << endl;
            cout << "    -h: show help" << endl;
            cout << "    -s: set the start vertex number" << endl;
            cout << "    -e: set the end vertex number" << endl;
            cout << "    -r: reduce (constructed) ZDD" << endl;
            cout << "    -n: not output ZDD" << endl;
            cout << "    --enum: enumerate all solutions (paths)" << endl;
            return 0;
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
        } else if (arg == "-k") {
            if (i + 1 < argc) {
                max_distance = atoi(argv[i + 1]);
            }
            ++i;
        }
    }

    Graph* graph = new Graph();
    graph->LoadAdjacencyList(cin);
    cerr << "# of vertices: " << graph->GetNumberOfVertices() << ", # of edges: "
         << graph->GetNumberOfEdges() << endl;

    if (end_vertex < 0) {
        end_vertex = graph->GetNumberOfVertices();
    }

    std::vector<std::vector<mate_t> > D;
    std::vector<ShortPair> P;
    std::vector<ShortPair> S;
    std::vector<mate_t> C;
    std::vector<mate_t> Q;
    std::vector<mate_t> T;

    D.push_back(std::vector<mate_t>());

	for (int i = 1; i <= graph->GetNumberOfVertices(); ++i) {
		D.push_back(std::vector<mate_t>());
		for (int j = 0; j <= graph->GetNumberOfVertices(); ++j) {
			D.back().push_back(j);
		}
	}

	int n = graph->GetNumberOfVertices();
	int nroot = sqrt(n);

	S.push_back(ShortPair(1, 4));
	S.push_back(ShortPair(1, 21));
	S.push_back(ShortPair(4, 21));

	C.push_back(3);
	Q.push_back(0);
	for (int i = 1; i <= graph->GetNumberOfEdges(); ++i) {
		T.push_back(i);
	}

    StateFGeneral* state = new StateFGeneral(graph, D, P, S, C, Q, T);

    PseudoZDD* zdd = FrontierAlgorithm::Construct(state);

    if (is_reduce_as_zdd) {
        zdd->ReduceAsZDD();
    }

    BigInteger bigint = zdd->ComputeNumberOfSolutions<BigInteger>();
    cerr << "# of ZDD nodes = " << zdd->GetNumberOfNodes() << ", # of solutions = "
         << bigint.GetString() << endl;

    if (is_print_zdd) {
        zdd->OutputZDD(stdout, false);
    }

    if (is_enum) {
        zdd->OutputAllSolutions(stdout);
    }

    //srand(time(NULL));
    //vector<int> ar;
    //zdd->SampleUniformlyRandomly(&ar);
    //graph->PrintForGraphviz(cout, ar);

    delete zdd;
    delete state;
    delete graph;

    //muntrace(); // for debug
    return 0;
}
