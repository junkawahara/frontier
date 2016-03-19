//
// BigInteger.hpp
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

#ifndef BIGINTEGER_HPP
#define BIGINTEGER_HPP

#include <iostream>
#include <vector>
#include <string>
#include <climits>
#include <sstream>

#include "Global.hpp"

namespace frontier_lib {

//*************************************************************************************************
// BigDecimal: 任意桁の10進数を表すクラス
// BigInteger クラスの数を10進数の文字列に変換するために用いられる
class BigDecimal {
private:
    std::vector<int> place_list_;
public:
    BigDecimal()
    {
        place_list_.resize(1);
        place_list_[0] = 0;
    }

    BigDecimal& operator=(uint64 integer)
    {
        place_list_.clear();
        while (integer > 0) {
            place_list_.push_back(integer % 10);
            integer /= 10;
        }
        return *this;
    }

    BigDecimal& operator+=(const BigDecimal& integer)
    {
        if (place_list_.size() < integer.place_list_.size()) {
            place_list_.resize(integer.place_list_.size());
        }
        uint64 c = 0;
        for (uint i = 0; i < place_list_.size(); ++i) {
            uint64 q1 = place_list_[i];
            uint64 q2 = (i < integer.place_list_.size() ? integer.place_list_[i] : 0);
            place_list_[i] = (q1 + q2 + c) % 10;
            c = (q1 + q2 + c) / 10;
        }
        if (c > 0) {
            place_list_.push_back(c);
        }
        return *this;
    }

    std::string GetString() const
    {
        std::ostringstream oss;
        for (int i = static_cast<int>(place_list_.size()) - 1; i >= 0; --i) {
            oss << place_list_[i];
        }
        return oss.str();
    }
};

//*************************************************************************************************
// BigInteger: 任意桁の整数を表すクラス
class BigInteger {
private:
    std::vector<uint64> place_list_;
public:
    BigInteger()
    {
        place_list_.resize(1);
        place_list_[0] = 0;
    }

    BigInteger& operator=(uint64 integer)
    {
        place_list_.resize(1);
        place_list_[0] = integer;
        return *this;
    }

    BigInteger& operator+=(const BigInteger& integer)
    {
        if (place_list_.size() < integer.place_list_.size()) {
            place_list_.resize(integer.place_list_.size());
        }
        uint64 c = 0;
        for (uint i = 0; i < place_list_.size(); ++i) {
            uint64 q1 = place_list_[i];
            uint64 q2 = (i < integer.place_list_.size() ? integer.place_list_[i] : 0);
            place_list_[i] = static_cast<uint64>(q1 + q2 + c);
            if (q1 > ULLONG_MAX - q2) { // check overflow (whether q1 + q2 is larger than 2^64 - 1)
                c = 1;
            } else if (c > ULLONG_MAX - (q1 + q2)) {
                c = 1;
            } else {
                c = 0;
            }
        }
        if (c > 0) {
            place_list_.push_back(c);
       }
       return *this;
    }

    operator double() const
    {
        double d = 0.0;
        for (int i = static_cast<int>(place_list_.size()) - 1; i >= 0; --i) {
            d *= (static_cast<double>(1 << 30) * (1 << 30) * (1 << 4)); // d *= 2^64
            d += place_list_[i];
        }
        return d;
    }

    void Set(int place, uint64 value)
    {
        if (place >= static_cast<int>(place_list_.size())) {
            place_list_.resize(place + 1);
        }
        place_list_[place] = value;
    }

    std::string GetString() const
    {
        BigDecimal d;
        for (int i = static_cast<int>(place_list_.size()) - 1; i >= 0; --i) {
            for (int j = 0; j < 64; ++j) { // compute d * 2^64
                BigDecimal d2 = d;
                d += d2;
            }
            BigDecimal d3;
            d3 = place_list_[i];
            d += d3;
        }
        return d.GetString();
    }

};

inline std::ostream& operator<<(std::ostream& ost, BigInteger& integer)
{
    ost << integer.GetString();
    return ost;
}

inline BigInteger operator+(BigInteger& integer1, BigInteger& integer2)
{
    BigInteger integer3(integer1);
    integer3 += integer2;
    return integer3;
}

} // the end of the namespace

#endif // BIGINTEGER_HPP
