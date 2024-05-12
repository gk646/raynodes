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
#ifndef CXSTRUCTS_SRC_DATASTRUCTURES_PAIR_H_
#  define CXSTRUCTS_SRC_DATASTRUCTURES_PAIR_H_

#  include "../cxconfig.h"

//used in the HashLinkedList in the buffer array

/**
 * @brief A template class that represents a Pair of elements.
 *
 * This class is a container for two elements of arbitrary types meant for static arrays.
 * The assigned flag can be used similar to a nullptr check. Handling it is completely manual.
 * <P>
 * The used types are initialized to default values. Accessing them before assignment leads to undefined behavior
 * <p>
 * It can be seen as a minimal version of the std::Pair available in the C++ Standard Library.
 *
 * @tparam T1 The type of the first element in the Pair.
 * @tparam T2 The type of the second element in the Pair.
 */
template <typename T1_, typename T2_>
class Pair {
  bool assigned_;
  T1_ first_;
  T2_ second_;

 public:
  /**
     * @brief Default constructor.
     *
     * Initializes the Pair with default-constructed elements.
     * The assigned flag is set to false.
     */
  inline Pair() : assigned_(false) {}
  /**
     * @brief Parameterized constructor.
     *
     * Initializes the Pair with the given elements.
     *
     * @param first The first element.
     * @param second The second element.
     */
  inline Pair(const T1_& first, const T2_& second)
      : first_(first), second_(second), assigned_(true) {}
  /**
     * @brief Returns a reference to the first element.
     *
     * @return A reference to the first element.
     */
  inline T1_& first() { return first_; }

  /**
     * @brief Returns a reference to the second element.
     *
     * @return A reference to the second element.
     */
  inline T2_& second() { return second_; }
  /**
     * @brief Returns a reference to the first element.
     *
     * @return A reference to the first element.
     */
  inline const T1_& first() const { return first_; }

  /**
     * @brief Returns a reference to the second element.
     *
     * @return A reference to the second element.
     */
  inline const T2_& second() const { return second_; }
  /**
     * @brief Checks if the Pair has been assigned values.
     *
     * @return True if the Pair has values assigned, false otherwise.
     */
  [[nodiscard]] inline bool& assigned() { return assigned_; }
};

#endif  //CXSTRUCTS_SRC_DATASTRUCTURES_PAIR_H_
