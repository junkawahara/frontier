//
// BigInteger.cpp
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

#include <climits>
#include <sstream>

#include "BigInteger.hpp"

namespace frontier_dd {

using namespace std;

//*************************************************************************************************
// BigDecimal

BigDecimal::BigDecimal()
{
    place_list_.resize(1);
    place_list_[0] = 0;
}

BigDecimal& BigDecimal::operator=(uint64 integer)
{
    place_list_.clear();
    while (integer > 0) {
        place_list_.push_back(integer % 10);
        integer /= 10;
    }
    return *this;
}

BigDecimal& BigDecimal::operator+=(const BigDecimal& integer)
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

string BigDecimal::GetString() const
{
    ostringstream oss;
    for (int i = static_cast<int>(place_list_.size()) - 1; i >= 0; --i) {
        oss << place_list_[i];
    }
    return oss.str();
}

//*************************************************************************************************
// BigInteger

BigInteger::BigInteger()
{
    place_list_.resize(1);
    place_list_[0] = 0;
}

BigInteger& BigInteger::operator=(uint64 integer)
{
    place_list_.resize(1);
    place_list_[0] = integer;
    return *this;
}

BigInteger& BigInteger::operator+=(const BigInteger& integer)
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

BigInteger::operator double() const
{
    double d = 0.0;
    for (int i = static_cast<int>(place_list_.size()) - 1; i >= 0; --i) {
        d *= (static_cast<double>(1 << 30) * (1 << 30) * (1 << 4)); // d *= 2^64
        d += place_list_[i];
    }
    return d;
}

void BigInteger::Set(int place, uint64 value)
{
    if (place >= static_cast<int>(place_list_.size())) {
        place_list_.resize(place + 1);
    }
    place_list_[place] = value;
}

string BigInteger::GetString() const
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

ostream& operator<<(ostream& ost, BigInteger& integer)
{
    ost << integer.GetString();
    return ost;
}

BigInteger operator+(BigInteger& integer1, BigInteger& integer2)
{
    BigInteger integer3(integer1);
    integer3 += integer2;
    return integer3;
}

} // the end of the namespace
