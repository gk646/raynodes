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
#ifndef CXSTRUCTS_SRC_CXUTIL_CXTIPS_H_
#  define CXSTRUCTS_SRC_CXUTIL_CXTIPS_H_

//Links provided without warranty of function, implied ownership or support of the linked website

//Extensive ranking of header compile times
//https://artificial-mind.net/projects/compile-health/

//Very good resources and presentations
//https://www.gameenginebook.com/coursemat.html

// -----------------------------------------------------------------------------
// Coding Guidelines
// -----------------------------------------------------------------------------

// === Core Rules ========================================
// No RTTI
// No Multiple inheritance
// No Exceptions


// === Explicit is better than implicit ========================================

// === Use 'const' and 'constexpr' =============================================
// Use `const` and `constexpr` where possible to enforce immutability and
// compile-time evaluation.

// === Use extra variables  ===============================
// Try to keep each code line without line breaks for readability
// Introduce variables if necessary
// Example:
//   const auto dx = 5;
//   const auto dy = 10;
//   DrawText(dx, dy, "Text");

// === Use constexpr for constants =============================================
// Avoid magic numbers by using `constexpr` for constants within functions (and classes).
// Example:
//   constexpr int Threshold = 42;
//   if (value > Threshold) { ... }

// === Use anonymous namespaces for internal linkage ===========================
// Hide functions not meant to be used outside this file within anonymous namespaces.
// Example:
//   namespace {
//       inline void DoSomething() {}
//   }
//   void MyFunction() {
//       DoSomething();
//   }

// === Modern C++ features =====================================================
// Try to use modern C++ features for safer and more readable code.
// Example:
//   std::print({}, 3.0F);  // instead of printf("%.f", 3.0F);

// === Naming Conventions ======================================================
// Lower-level code: Use snake_case.
//    vector::push_back();       // STL-like API
//
// Application-level code: Use camelCase.
//   class MyClass{
//      float myMember;
//      std::string anotherMember;
//   }
//   MyClass myObject;
//   myObject.myFunction();

// === Prefer formatting that reduces line count ===============================
// Use formatting plugins like .clang-format
// Less lines means less to read
//

// === Always group logically =====================================
// Keep the same order of arguments inside an interface
// This include Types
//    MyAPI::DoWork(Context& c, int myArg);
//    MyAPI::DoWork2(Context& c, float myArg);
//
// but could extend to:
//    - complex types first
//    - ...
//
// Also sort variable declarations logically where applicable (inside functions)
//

// === Includes ======================================================
//
// Use "" for project level includes and <> for libraries
//
//  #include "ui/components/Button.h"
//  #include <raylib.h>
//
// Include from the src directory (include it in cmake) and use full path for all includes
//
//  #include "ui/components/Button.h"     //instead of #include "../components/Button.h"

// === Structured Bindings ======================================================
//
// Use structure bindings when possible
//    - if the variable can be split into 2-4 members
//
// Rectange hitBox = {0,0,50,50};
// if(hitBox.x > 0 && hitBox.width == 50){}
//
// -> instead:
//
// auto& [x, y, width, height] = Rectangle{0,0,50,50};
// if(x > 0 &&width == 50){}
//


#endif  //CXSTRUCTS_SRC_CXUTIL_CXTIPS_H_