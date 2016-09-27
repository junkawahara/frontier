//
// SolutionArray.hpp
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

#ifndef SOLUTIONARRAY_HPP
#define SOLUTIONARRAY_HPP

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <iostream>
#include <vector>
#include <stdexcept>

namespace frontier_lib {

template <typename T>
double Divide(T nume, T deno)
{
    return static_cast<double>(nume) / static_cast<double>(deno);
}

#if HAVE_LIBGMPXX
template <> double Divide<MpInt>(MpInt nume, MpInt deno);

template <>
inline double Divide<MpInt>(MpInt nume, MpInt deno)
{
    mpf_class num = nume;
    mpf_class den = deno;
    num.set_prec(10);
    den.set_prec(10);

    mpf_class probap = num / den;
    return probap.get_d();
}
#endif

//*************************************************************************************************
// SolutionArray: 解の数を数えるためのインターフェースクラス。
class SolutionArray {
public:
    virtual ~SolutionArray() {}
    virtual void SampleUniformlyRandomly(const std::vector<std::pair<intx, intx> >& node_array,
                                         const std::vector<intx>& level_first_array,
                                         std::vector<int>* result) = 0;
};

//*************************************************************************************************
// SolutionArrayDerive<T>: 解の数を数えるためのクラス。
// T は解の数を表現する型。intx, double, ApInt, BigNumber 等。
template <typename T>
class SolutionArrayDerive : public SolutionArray {
private:
    std::vector<T> solution_array_;
public:
    virtual ~SolutionArrayDerive() {}

    // 解の数の計算を行う。overflow チェックは行わない。
    T ComputeNumberOfSolutions(const std::vector<std::pair<intx, intx> >& node_array)
    {
        solution_array_.resize(node_array.size());
        solution_array_[0] = 0;
        solution_array_[1] = 1;

        for (intx i = static_cast<intx>(node_array.size()) - 1; i >= 2; --i) {
            solution_array_[i] = solution_array_[node_array[i].first]
                                  + solution_array_[node_array[i].second];
        }

        return solution_array_[2];
    }

    // 解の数の計算を行う。計算時に overflow チェックを行う。uintx 型限定
    uintx ComputeNumberOfSolutionsOF(const std::vector<std::pair<intx, intx> >& node_array)
    {
        solution_array_.resize(node_array.size());
        solution_array_[0] = 0;
        solution_array_[1] = 1;

        for (intx i = static_cast<intx>(node_array.size()) - 1; i >= 2; --i) {
            if (solution_array_[node_array[i].second] > UINTX_MAX
                                    - solution_array_[node_array[i].first]) { // overflow
                throw std::overflow_error("overflow!");
            }
            solution_array_[i] = solution_array_[node_array[i].first]
                                  + solution_array_[node_array[i].second];
        }

        return solution_array_[2];
    }

    virtual void SampleUniformlyRandomly(const std::vector<std::pair<intx, intx> >& node_array,
                                         const std::vector<intx>& level_first_array,
                                         std::vector<int>* result)
    {
        intx current_node = 2;
        while (current_node >= 2) {
            bool is_hi = false;
            T lo = solution_array_[node_array[current_node].first];
            T hi = solution_array_[node_array[current_node].second];
            if (lo == 0) {
                is_hi = true;
            } else if (hi == 0) {
                is_hi = false;
            } else {
                T sum = lo + hi;
                double prob = Divide<T>(lo, sum);
                double ra = static_cast<double>(rand()) / RAND_MAX;
                if (ra < prob) {
                    is_hi = false;
                } else {
                    is_hi = true;
                }
            }
            if (is_hi) {
                for (uint i = 0; i < level_first_array.size(); ++i) {
                    if (level_first_array[i] <= current_node
                        && current_node < level_first_array[i + 1]) {
                        result->push_back(i + 1);
                        break;
                    }
                }
                current_node = node_array[current_node].second;
            } else {
                current_node = node_array[current_node].first;
            }
        }
    }
};

class OptimalCalculator {
public:
    static int GetValiableNumber(int node_num,
                                 const std::vector<intx>& level_first_array) {

        for (uint i = 0; i < level_first_array.size(); ++i) {
            if (level_first_array[i] <= node_num
                && node_num < level_first_array[i + 1]) {
                return i;
            }
        }
        return -1;
    }

    // This code has not been checked yet!!!
    static double GetMaximum(const std::vector<std::pair<intx, intx> >& node_array,
                            const std::vector<intx>& level_first_array,
                            const Graph& graph,
                            std::vector<int>* result) {

        std::vector<double> value_array;
        std::vector<int> num_array;

        value_array.resize(node_array.size());
        num_array.resize(node_array.size());

        value_array[0] = -99999999.9;
        value_array[1] = 0;

        for (intx i = static_cast<intx>(node_array.size()) - 1; i >= 2; --i) {
            double val = value_array[node_array[i].second] +
                graph.GetEdge(GetValiableNumber(i, level_first_array)).weight;

            if (value_array[node_array[i].first] >= val) {
                value_array[i] = value_array[node_array[i].first];
                num_array[i] = 0;
            } else {
                value_array[i] = val;
                num_array[i] = 1;
            }
        }

        intx node = 2;
        double val2 = 0.0;
        while (true) {
            if (num_array[node] == 1) {
                result->push_back(node);
                val2 += graph.GetEdge(GetValiableNumber(node, level_first_array)).weight;
                node = node_array[node].second;
            } else {
                node = node_array[node].first;
            }
        }
        return val2;
    }
};

} // the end of the namespace

#endif // SOLUTIONARRAY_HPP
