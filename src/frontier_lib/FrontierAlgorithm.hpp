//
// FrontierAlgorithm.hpp
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

#ifndef FRONTIER_ALGORITHM_HPP
#define FRONTIER_ALGORITHM_HPP

#include <vector>
#include <algorithm>
#include <cstdarg>
#include <cstdio>
#include <climits>
#include <iostream>
#include <fstream>

#include "Global.hpp"
#include "State.hpp"
#include "ZDDNode.hpp"
#include "SolutionArray.hpp"
#include "Mate.hpp"
#include "PseudoZDD.hpp"
#include "HashTable.hpp"

namespace frontier_lib {

//*************************************************************************************************
// FrontierAlgorithm: フロンティア法によるZDDの構築を行うクラス。
// このクラスの Construct メンバ関数を呼び出すことで、アルゴリズムが開始して、
// ZDDが構築される。
class FrontierAlgorithm {
private:
    static const intx INITIAL_HASH_SIZE_ = (1ll << 26);

public:
    static PseudoZDD* Construct(State* state, std::string filename = "")
    {
        PseudoZDD* zdd = new PseudoZDD();
        if (!filename.empty()) {
            zdd->SetHddMode(filename);
        }
        ZDDNode* root_node = zdd->CreateRootNode(); // 根ノードの作成

        HashTable global_hash_table(INITIAL_HASH_SIZE_); // ノード検索用ハッシュ
        zdd->SetHashTable(&global_hash_table);

        Mate* mate = state->Initialize(root_node);

        // 各辺について、以下を実行する。
        for (int edge = 1; edge <= state->GetNumberOfEdges(); ++edge) {

            if (state->IsPrintProgress()) {
                DebugPrintf("start edge: %d\n", edge);
            }

            // 次の辺の処理を開始
            state->StartNextEdge();

            // 次のレベルのノードがどこから始まるかを記録
            zdd->SetLevelStart();
            mate->SetOffset();

            // 現在のレベルの各ノードに対するループ
            for (intx i = 0; i < zdd->GetCurrentLevelSize(); ++i) {
                ZDDNode* node = zdd->GetCurrentLevelNode(i);

                // Lo枝とHi枝について処理をする
                //（child_num が 0 のとき Lo枝、child_num が 1 のとき Hi枝についての処理）
                for (int child_num = 0; child_num < state->GetNumberOfChildren(); ++child_num) {

                    state->UnpackMate(node, mate, child_num);

                    if (state->IsPrintProgress() && child_num == 0) {
                        DebugPrintf("node id = " PERCENT_D "\n", node->node_number);
                        DebugPrintf("%s\n", state->GetString(mate, false).c_str());
                    }

                    // 子ノード
                    ZDDNode* child_node = state->MakeNewNode(node, mate, child_num, zdd);

                    // 終端でないかどうかチェック
                    if (child_node != zdd->ZeroTerminal && child_node != zdd->OneTerminal) {

                        state->PackMate(child_node, mate);

                        // child_node と「等価な」ノードを調べる
                        intx index = zdd->FindNodeFromNextLevel(*child_node, state, mate);
                        if (index >= 0) { // 等価なノードが存在する
                            // 子ノード (child_node) は新たに作らないので解体
                            zdd->DestructNode(child_node, state, mate);
                            // index 番目のノードとマージ
                            child_node = zdd->GetNode(index);
                        } else { // 等価なノードが存在しない
                            // 次レベルに child_node を加える
                            zdd->AddNodeToNextLevel(child_node, state, mate);
                            if (edge == state->GetNumberOfEdges()) { // for multi-terminal
                                state->OutputMultiterminal(mate);
                            }
                        }
                    }
                    zdd->SetChildNode(node, child_node, child_num);

                    if (state->IsPrintProgress()) {
                        DebugPrintf("\t%d-child: id = " PERCENT_D ": ", child_num, child_node->node_number);
                        DebugPrintf("%s\n", state->GetString(mate, true).c_str());
                    }
                }
            }
            global_hash_table.Flush();
#ifndef DEBUG
            state->PrintNodeNum(zdd->GetNumberOfNodes());
#endif
        }
        zdd->SetLevelStart();
        delete mate;
        zdd->SetHashTable(NULL);
        zdd->HddTerminate();
        return zdd;
    }
};


} // the end of the namespace

#endif // FRONTIER_ALGORITHM_HPP
