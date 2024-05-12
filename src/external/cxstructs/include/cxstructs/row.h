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
#ifndef CXSTRUCTS_SRC_DATASTRUCTURES_ROW_H_
#  define CXSTRUCTS_SRC_DATASTRUCTURES_ROW_H_

#  include <algorithm>
#  include <cstdint>
#  include <type_traits>
#  include "../cxconfig.h"

namespace cxstructs {

/**
 * @class row
 *
 * @brief A non-mutable, fixed-size container for elements of type T.
 *
 * The `row` class is a lightweight container for a fixed number of elements.
 * Unlike dynamic containers, the size
 * is determined at compile-time, which eliminates the overhead of
 * dynamic memory allocation and resizing.<p>
 *
 * It is non-mutable so its size cannot be changed after construction,
 * and elements cannot be inserted or removed.
 *
 * @tparam n_elem The number of elements in the row (fixed at compile-time).
 * @tparam T The type of elements in the row. Defaults to `float`.
 */
template <uint_32_cx n_elem, typename T = float>
class row {
  T arr_[n_elem];

 public:
  /**
   * @brief Default constructor.
   *
   * Initializes the elements with default values.
   */
  inline row() = default;

  /**
   * @brief Constructs a row with all elements initialized to the given value.
   *
   * @param val The value to initialize all elements with.
   */
  inline explicit row(const T val) { std::fill(arr_, arr_ + n_elem, val); }

  /**
   * @brief Constructs a row with elements initialized using a provided function.
   *
   * @tparam fill_form A callable object or function that takes a single integer as an argument and returns a value of type T.
   * @param form The callable object or function used to initialize the elements of the row. It is invoked for each element with the element's index as an argument.
   */
  template <typename fill_form,
            typename = std::enable_if_t<std::is_invocable_r_v<T, fill_form, uint_32_cx>>>
  inline explicit row(fill_form form) {
    for (uint_32_cx i = 0; i < n_elem; i++) {
      arr_[i] = form(i);
    }
  }
  row(std::initializer_list<T> init_list) {
    CX_ASSERT(init_list.size() == n_elem, "");
    std::copy(init_list.begin(), init_list.end(), arr_);
  }
  /**
   * @brief Accesses the element at the specified index.
   *
   * @param i The index of the element to access.
   * @return A reference to the element at the specified index.
   */
  [[nodiscard]] inline T& operator[](const uint_32_cx& i) { return arr_[i]; }
  [[nodiscard]] inline const T& operator[](const uint_32_cx& i) const { return arr_[i]; }

  /**
   * @brief Returns the number of elements in the row.
   *
   * @return The number of elements in the row.
   */
  [[nodiscard]] constexpr inline uint_32_cx size() const { return n_elem; }
};

}  // namespace cxstructs

#endif  //CXSTRUCTS_SRC_DATASTRUCTURES_ROW_H_
