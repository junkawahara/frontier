//
// Global.hpp
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

#ifndef GLOBAL_HPP
#define GLOBAL_HPP

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <climits>
#include <iostream>

#ifdef HAVE_LIBGMPXX
#include <gmpxx.h>
#endif

namespace frontier_dd {

typedef unsigned char byte;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef long long int int64;
typedef unsigned long long int uint64;

// for 32-bit architecture
#ifdef BIT32

typedef int intx;
typedef unsigned int uintx;
const uintx UINTX_MAX = UINT_MAX;
#define PERCENT_D "%d"
#define PERCENT_X "%x"

// for 64-bit architecture
#else

typedef int64 intx;
typedef uint64 uintx;
const uintx UINTX_MAX = ULLONG_MAX;
#define PERCENT_D "%lld"
#define PERCENT_X "%llx"

#endif // end BIT32


// least significant bit
const uintx LSBIT = static_cast<uintx>(1u);
// bit width of intx (32 or 64)
const int INTX_BITSIZE = sizeof(intx) * 8;

// type for storing an integer (vertex index) of a mate
typedef short mate_t;

// type for big integers
// If this program is built without GMP library (i.e. HAVE_LIBGMPXX isn't defined),
// the type of "MpInt" is not used.
#ifdef HAVE_LIBGMPXX
typedef mpz_class MpInt;
#else
typedef uintx MpInt; // dummy typedef
#endif

struct ShortPair {
    short first;
    short second;

	ShortPair() { }

	ShortPair(short f, short s)
	{
		first = f;
		second = s;
	}
};

#ifdef DEBUG
#define DebugPrintf(format, ...)  fprintf(stderr, format, __VA_ARGS__)
#else
#define DebugPrintf(format, ...)
#endif

} // the end of the namespace

#endif // GLOBAL_HPP
