// Copyright (c) 2023 gk646
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
#define CX_FINISHED
#ifndef CXSTRUCTS_SRC_CONFIG_H_
#  define CXSTRUCTS_SRC_CONFIG_H_

#  include <cstdint>            //Used for almost all size_types
#  include "cxutil/cxassert.h"  //includes only <cstdio>

//-----------DEFINES-----------//

//#define CX_USE_INT            : uses type int for all custom types
//#define CX_STACK_ABORT        : calls std::abort() when the size limit of stack structures is reached
//
//

#  define CX_INL inline
#  define CX_NDISC [[nodiscard]]

// namespace for exposed structs and functions
namespace cxstructs {}


#  ifdef CX_STACK_ABORT
#    define CX_STACK_ABORT_IMPL()                                                                  \
      if (size_ >= N) std::abort()
#  else
#    define CX_STACK_ABORT_IMPL() (void(0))
#  endif

#  ifndef CX_USE_INT
using uint_32_cx = uint_fast32_t;
using uint_16_cx = uint_fast16_t;
using int_32_cx = int_fast32_t;
#  else
typedef int uint_32_cx;
typedef int uint_16_cx;
typedef int int_32_cx;
#  endif

#endif  //CXSTRUCTS_SRC_CONFIG_H_