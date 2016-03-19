//
// testfrontier.cpp
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

#include <cstring>
#include <utility>

#include "../frontier_lib/Global.hpp"
#include "../frontier_lib/BigInteger.hpp"
#include "OptionParser.hpp"

using namespace std;
using namespace frontier_lib;

void MakeTestCombination(std::vector<std::pair<string, string> >* test_list)
{
    string result1[] = {"1", "7", "21", "35", "35", "21", "7", "1"};
    string result2[] = {"128", "127", "127", "112", "21"};

    const char* range_array[] = {"[0,7]", "[1,7]", "[0,6]", "[2,5]", "[5,5]"};

    char ss[1024];

    for (int i = 0; i <= 7; ++i) {
        sprintf(ss, "-t combination -n --elimit %d -c --input testdata/random_graph1.txt", i);

        test_list->push_back(std::make_pair(string(ss), result1[i]));
    }

    for (int i = 0; i < 5; ++i) {
        sprintf(ss, "-t combination -n --elimit %s -c --input testdata/random_graph1.txt", range_array[i]);

        test_list->push_back(std::make_pair(string(ss), result2[i]));
    }

}

void MakeTestSTPathOnGrid(std::vector<std::pair<string, string> >* test_list)
{
    // The following numbers come from OEIS
    // (http://oeis.org/A007764).
    string result[] = {"0", "1", "2", "12", "184", "8512", "1262816",
                       "575780564", "789360053252", "3266598486981642"};
    char ss[1024];

    for (int i = 3; i <= 9; ++i) {
        sprintf(ss, "-t stpath -n --input testdata/grid%dx%d.txt", i, i);

        test_list->push_back(std::make_pair(string(ss), result[i]));
    }
}

void MakeTestCycleOnGrid(std::vector<std::pair<string, string> >* test_list)
{
    // The following numbers come from OEIS
    // (http://oeis.org/A140517).
    string result[] = {"0", "0", "1", "13", "213", "9349", "1222363",
                       "487150371", "603841648931", "2318527339461265",
                       "27359264067916806101", "988808811046283595068099",
                       "109331355810135629946698361371",
                       "36954917962039884953387868334644457"};

    char ss[1024];

    for (int i = 3; i <= 9; ++i) {
        sprintf(ss, "-t stpath --cycle -n --input testdata/grid%dx%d.txt", i, i);

        test_list->push_back(std::make_pair(string(ss), result[i]));
    }
}

void MakeTestHamiltonianPathOnGrid(std::vector<std::pair<string, string> >* test_list)
{
    // The following numbers come from OEIS
    // (http://oeis.org/A001184).
    string result[] = {"1", "2", "104", "111712", "2688307514",
                       "1445778936756068", "17337631013706758184626"};

    char ss[1024];

    for (int i = 1; i <= 4; ++i) {
        sprintf(ss, "-t stpath --hamilton -n --input testdata/grid%dx%d.txt", i * 2 + 1, i * 2 + 1);

        test_list->push_back(std::make_pair(string(ss), result[i]));
    }
}


void MakeTestHamiltonianCycleOnGrid(std::vector<std::pair<string, string> >* test_list)
{
    // The following numbers come from OEIS
    // (http://oeis.org/A003763).
    string result[] = {"0", "1", "6", "1072", "4638576", "467260456608",
                       "1076226888605605706"};

    char ss[1024];

    for (int i = 2; i <= 5; ++i) {
        sprintf(ss, "-t stpath --cycle --hamilton -n --input testdata/grid%dx%d.txt", i * 2, i * 2);

        test_list->push_back(std::make_pair(string(ss), result[i]));
    }
}


void MakeTestSForestOnGrid(std::vector<std::pair<string, string> >* test_list)
{
    // The following numbers come from OEIS
    // (http://oeis.org/A080691).
    string result[] = {"0", "1", "15", "3102", "8790016", "341008617408",
                       "181075508242067552", "1315927389374152034113856",
                       "130877523274817580209987036404864",
                       "178135975585132088643635627145305047963624"};
    char ss[1024];

    for (int i = 3; i <= 9; ++i) {
        sprintf(ss, "-t sforest -n --input testdata/grid%dx%d.txt", i, i);

        test_list->push_back(std::make_pair(string(ss), result[i]));
    }
}

void MakeTestSTreeOnGrid(std::vector<std::pair<string, string> >* test_list)
{
    // The following numbers come from OEIS
    // (http://oeis.org/A007341).
    string result[] = {"0", "1", "4", "192", "100352", "557568000",
                       "32565539635200", "19872369301840986112",
                       "126231322912498539682594816",
                       "8326627661691818545121844900397056",
                       "5694319004079097795957215725765328371712000"};

    char ss[1024];

    for (int i = 3; i <= 9; ++i) {
        sprintf(ss, "-t stree -n --input testdata/grid%dx%d.txt", i, i);

        test_list->push_back(std::make_pair(string(ss), result[i]));
    }
}

void MakeTestSTPathOnRandom(std::vector<std::pair<string, string> >* test_list)
{
    string result[] = {"1", "7", "88"};
    int st[] = {2, 4, 2, 1, 6, 8};

    char ss[1024];

    for (int i = 0; i < 3; ++i) {
        sprintf(ss, "-t stpath -s %d -e %d -n -c --input testdata/random_graph%d.txt", st[2 * i], st[2 * i + 1], i + 1);

        test_list->push_back(std::make_pair(string(ss), result[i]));
    }
}

void MakeTestSTPathDistOnRandom(std::vector<std::pair<string, string> >* test_list)
{
    string result[] = {"1", "1", "1", "1", "1", "0", "0", "0",
                       "6", "4", "6", "2", "4", "0", "2", "0",
                       "54", "35", "54", "32", "51", "14", "33", "9"};

    const char* elimit_array[] = {"[0,14]", "[0,5]", "[0,8]", "[3,5]", "[3,8]", "[5,5]", "[5,7]", "[7,7]"};

    char ss[1024];

    int count = 0;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 8; ++j) {
            sprintf(ss, "-t stpath -n -c --elimit %s --input testdata/random_graph%d.txt", elimit_array[j], i + 1);

            test_list->push_back(std::make_pair(string(ss), result[count]));
            ++count;
        }
    }
}

void MakeTestMTPathOnRandom(std::vector<std::pair<string, string> >* test_list)
{
    string result[] = {"0", "3", "11"};

    char ss[1024];

    for (int i = 0; i < 3; ++i) {
        sprintf(ss, "-t mtpath --terminal testdata/terminal1.txt -n -c --input testdata/random_graph%d.txt", i + 1);

        test_list->push_back(std::make_pair(string(ss), result[i]));
    }
}


void MakeTestPathMatchingOnRandom(std::vector<std::pair<string, string> >* test_list)
{
    string result[] = {"80", "404", "4932"};

    char ss[1024];

    for (int i = 0; i < 3; ++i) {
        sprintf(ss, "-t pathmatching -n -c --input testdata/random_graph%d.txt", i + 1);

        test_list->push_back(std::make_pair(string(ss), result[i]));
    }
}

void MakeTestKcutOnRandom(std::vector<std::pair<string, string> >* test_list)
{
    string result[] = {"127", "939", "36678"};


    char ss[1024];

    for (int i = 0; i < 3; ++i) {
        sprintf(ss, "-t kcut -n -c --input testdata/random_graph%d.txt", i + 1);

        test_list->push_back(std::make_pair(string(ss), result[i]));
    }
}

void MakeTestRForestOnRandom(std::vector<std::pair<string, string> >* test_list)
{
    string result[] = {"4", "44", "1045"};

    char ss[1024];

    for (int i = 0; i < 3; ++i) {
        sprintf(ss, "-t rforest -n -f 1 8 -c --input testdata/random_graph%d.txt", i + 1);

        test_list->push_back(std::make_pair(string(ss), result[i]));
    }
}

void MakeTestRcutOnRandom(std::vector<std::pair<string, string> >* test_list)
{
    string result[] = {"4", "62", "3398"};

    char ss[1024];

    for (int i = 0; i < 3; ++i) {
        sprintf(ss, "-t rcut -n -f 1 8 -c --input testdata/random_graph%d.txt", i + 1);

        test_list->push_back(std::make_pair(string(ss), result[i]));
    }
}

void MakeTestSetptpkc(std::vector<std::pair<string, string> >* test_list)
{
    string result[] = {"2", "1", "72", "38", "72", "161"};
    string kind[] = {"setpt", "setpk", "setc"};

    char ss[1024];

    for (int i = 0; i < 6; ++i) {
        sprintf(ss, "-t %s -n -c --input testdata/hyper_graph%d.txt", kind[i / 2].c_str(), i % 2 + 1);

        test_list->push_back(std::make_pair(string(ss), result[i]));
    }
}

int main()
{
    //mtrace(); // for debug

    srand(static_cast<unsigned int>(time(NULL)));

    std::vector<std::pair<string, string> > test_list;

    MakeTestCombination(&test_list);
    MakeTestSTPathOnGrid(&test_list);
    MakeTestCycleOnGrid(&test_list);
    MakeTestHamiltonianPathOnGrid(&test_list);
    MakeTestHamiltonianCycleOnGrid(&test_list);
    MakeTestSForestOnGrid(&test_list);
    MakeTestSTreeOnGrid(&test_list);
    //MakeTestPartitionOnGrid(&test_list);
    MakeTestSTPathOnRandom(&test_list);
    MakeTestSTPathDistOnRandom(&test_list);
    MakeTestMTPathOnRandom(&test_list);
    MakeTestPathMatchingOnRandom(&test_list);
    MakeTestKcutOnRandom(&test_list);
    MakeTestRForestOnRandom(&test_list);
    MakeTestRcutOnRandom(&test_list);
    //MakeTestPartition2OnRandom(&test_list);
    //MakeTestPartition3OnRandom(&test_list);
    //MakeTestVertexWeightOnRandom(&test_list);
    //MakeTestVertexWeightBoundOnRandom(&test_list);
    MakeTestSetptpkc(&test_list);

    for (frontier_lib::uint i = 0; i < test_list.size(); ++i) {
        OptionParser* parser = new OptionParser;

        parser->ParseCommandLine(test_list[i].first);

        parser->PrepareGraph();
        parser->MakeState();

        PseudoZDD* zdd = FrontierAlgorithm::Construct(parser->state);

        assert(zdd->ComputeNumberOfSolutions<BigInteger>().GetString() == test_list[i].second);

        //parser.Output(zdd);

        delete zdd;
        delete parser;
    }

    //muntrace(); // for debug

    return 0;
}
