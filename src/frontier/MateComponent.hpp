//
// MateComponent.hpp
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

#ifndef MATECOMPONENT_HPP
#define MATECOMPONENT_HPP

#include "MateSForest.hpp"

namespace frontier_dd {

//*************************************************************************************************
// MateConfComponent
struct MateConfComponent {
    short number_of_components;
};

//*************************************************************************************************
// StateComponent
class StateComponent : public StateForestBase<MateConfComponent> {
private:
    short component_limit_;
    bool is_le_; // true なら component 数を component_limit_ 以下に制限
    bool is_me_; // true なら component 数を component_limit_ 以上に制限
    // is_le_, is_me_ ともに false なら component 数を component_limit_ に制限
public:
    StateComponent(Graph* graph, short component_limit, bool is_le, bool is_me);
    ~StateComponent();

    int GetComponentLimit() const
    {
       return component_limit_;
    }

    bool IsLe() const
    {
       return is_le_;
    }

    bool IsMe() const
    {
        return is_me_;
    }

    virtual Mate* UnpackMate(ZDDNode* node, int child_num);
};

//*************************************************************************************************
// MateComponent
class MateComponent : public MateForestBase<MateConfComponent> {
public:
    MateComponent(State* state);

    virtual void Update(State* state, int child_num);
    virtual int CheckTerminalPre(State* state, int child_num);
    virtual int CheckTerminalPost(State* state);
};

} // the end of the namespace

#endif // MATECOMPONENT_HPP
