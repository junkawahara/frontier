//
// distfilter.cpp
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
#include <cstdlib>
#include <algorithm>

#include "../frontier_lib/Graph.hpp"

using namespace std;
using namespace frontier_lib;

int main(int argc, char** argv)
{
    if (argc <= 2) {
        cerr << "Usage: " << argv[0] << " start end [max_distance]" << endl;
        return 1;
    }

    int start = atoi(argv[1]);
    int end = atoi(argv[2]);
    int max_distance = (argc >= 4 ? atof(argv[3]) : -1);

    Graph graph;
    graph.LoadEdgeList(cin);
    graph.FloydWarshall();

    const vector<vector<int> >& dist_matrix = graph.GetDistMatrix();

    cerr << "start: " << start << ", end:" << end << endl;
    cerr << "min dist: " << dist_matrix[start][end] << endl;

    if (max_distance > 0) {
        std::vector<int> new_var_array;
        std::vector<int> remove_array;
        new_var_array.resize(graph.GetNumberOfVertices() + 1);
        int new_vertex_num = 1;

        for (int i = 1; i <= graph.GetNumberOfVertices(); ++i) {
            if (dist_matrix[start][i] + dist_matrix[i][end] > max_distance) {
                remove_array.push_back(i);
            } else {
                new_var_array[i] = new_vertex_num;
                ++new_vertex_num;
            }
        }

        //for (unsigned int i = 0; i < remove_array.size(); ++i) {
        //    cout << remove_array[i] << ", ";
        //}
        cerr << "rename vertices:" << endl;
        for (int i = 1; i <= graph.GetNumberOfVertices(); ++i) {
            if (std::find(remove_array.begin(), remove_array.end(), i) == remove_array.end()) {
                cerr << i << " -> " << new_var_array[i] << endl;
            }
        }
        graph.RemoveVertices(remove_array, new_var_array);
        graph.PrintEdgeList(cout);
    }

    return 0;
}
