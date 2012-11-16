//
// BigInteger.hpp
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

#ifndef BIGINTEGER_HPP
#define BIGINTEGER_HPP

#include <iostream>
#include <vector>
#include <string>

#include "Global.hpp"

namespace frontier_dd {

//*************************************************************************************************
// BigDecimal: 任意桁の10進数を表すクラス
// BigInteger クラスの数を10進数の文字列に変換するために用いられる
class BigDecimal {
private:
    std::vector<int> place_list_;
public:
    BigDecimal();

    BigDecimal& operator=(uint64 integer);
    BigDecimal& operator+=(const BigDecimal& integer);

    std::string GetString() const;
};

//*************************************************************************************************
// BigInteger: 任意桁の整数を表すクラス
class BigInteger {
private:
    std::vector<uint64> place_list_;
public:
    BigInteger();

    BigInteger& operator=(uint64 integer);
    BigInteger& operator+=(const BigInteger& integer);
    operator double() const;

    void Set(int place, uint64 value);
    std::string GetString() const;
};

std::ostream& operator<<(std::ostream& ost, BigInteger& integer);
BigInteger operator+(BigInteger& integer1, BigInteger& integer2);

} // the end of the namespace

#endif // BIGINTEGER_HPP
