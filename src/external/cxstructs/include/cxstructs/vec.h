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
#ifndef CXSTRUCTS_ARRAYLIST_H
#  define CXSTRUCTS_ARRAYLIST_H

#  include <initializer_list>
#  include "../cxalgos/Sorting.h"
#  include "../cxconfig.h"

/*This implementation is well optimized and should generally be a bit faster than the std::vector in a lot of use cases
 * Its using explicit allocator syntax to switch between the default and a custom one
 * Used in QuadTree.h
*/

namespace cxstructs {
/**
 * <h2>vec</h2>
 * This is an implementation of a dynamic array data structure, similar to the <code>ArrayList</code> in Java or <code>std::vector</code> in C++.
 * <br><br>
 * <p>A dynamic array is a random access, variable-n_elem list data structure that allows elements to be added or removed.
 * It provides the capability to index into the list, push_back elements to the end, and erase elements from the end in a time-efficient manner.</p>
 */
template <typename T, typename Allocator = std::allocator<T>, typename size_type = uint32_t>
class vec {
 public:
  using value_type = T;
  using reference = T&;
  using const_reference = const T&;
  using iterator = T*;
  using const_iterator = const T*;

 private:
  Allocator alloc;
  T* arr_;
  size_type size_;
  size_type capacity_;

  inline void grow() noexcept {
    capacity_ *= 2;

    T* n_arr = alloc.allocate(capacity_);

    std::uninitialized_move(arr_, arr_ + size_, n_arr);

    // Destroy the original objects
    if constexpr (!std::is_trivial_v<T>) {
      for (size_type i = 0; i < size_; i++) {
        std::allocator_traits<Allocator>::destroy(alloc, &arr_[i]);
      }
    }

    alloc.deallocate(arr_, capacity_);
    arr_ = n_arr;

    if constexpr (!std::is_trivial_v<T>) {
      for (uint_fast32_t i = size_; i < capacity_; i++) {
        std::allocator_traits<Allocator>::construct(alloc, &arr_[i]);
      }
    }
  }
  inline void shrink() noexcept {
    auto old_len = capacity_;
    capacity_ = size_ * 1.5;

    T* n_arr = alloc.allocate(capacity_);

    std::uninitialized_move(arr_, arr_ + size_, n_arr);

    // Destroy the original objects
    if constexpr (!std::is_trivial_v<T>) {
      for (size_t i = 0; i < old_len; i++) {
        std::allocator_traits<Allocator>::destroy(alloc, &arr_[i]);
      }
    }

    alloc.deallocate(arr_, old_len);
    arr_ = n_arr;
  }

 public:
  /**
   * Default constructor<p>
   * Recommended to leave it at 32 due to optimizations with the allocator
   * @param n_elem number of starting elements
   */
  inline explicit vec(uint_32_cx n_elems = 32)
      : capacity_(n_elems), size_(0), arr_(alloc.allocate(n_elems)) {
    if constexpr (!std::is_trivial_v<T>) {
      for (uint_fast32_t i = 0; i < n_elems; i++) {
        std::allocator_traits<Allocator>::construct(alloc, &arr_[i]);
      }
    }
  }
  inline vec(uint_32_cx n_elems, const T fillVal)
      : capacity_(n_elems), size_(n_elems), arr_(alloc.allocate(n_elems)) {
    if constexpr (std::is_trivial_v<T>) {
      std::fill(arr_, arr_ + n_elems, fillVal);
    } else {
      for (uint_fast32_t i = 0; i < n_elems; i++) {
        std::allocator_traits<Allocator>::construct(alloc, &arr_[i], fillVal);
      }
    }
  }
  /**
* @brief Constructs a vec with the specified number of elements, and initializes them using a provided function.
*
* @tparam fill_form A callable object or function that takes a single int as an argument and returns a float
* @param n_elem The number of elements in the vec.
* @param form The callable object or function used to initialize the elements of the vec. It is invoked for each element with the element's index as an argument.
**/
  template <typename fill_form,
            typename = std::enable_if_t<std::is_invocable_r_v<T, fill_form, int>>>
  inline vec(uint_32_cx n_elem, fill_form form)
      : capacity_(n_elem), size_(n_elem), arr_(alloc.allocate(n_elem)) {
    for (uint_fast32_t i = 0; i < n_elem; i++) {
      std::allocator_traits<Allocator>::construct(alloc, &arr_[i], form(i));
    }
  }
  /**
   * std::vector constructor<p>
   * vec vec1({1,2,3});<p>
   * @param vector
   */
  explicit vec(const std::vector<T>& vector)
      : capacity_(vector.size() * 1.5), size_(vector.size()),
        arr_(alloc.allocate(vector.size() * 1.5)) {
    std::copy(vector.begin(), vector.end(), arr_);
  }
  explicit vec(const std::vector<T>&& move_vector)
      : capacity_(move_vector.size() * 1.5), size_(move_vector.size()),
        arr_(alloc.allocate(move_vector.size() * 1.5)) {
    std::move(move_vector.begin(), move_vector.end(), arr_);
  }
  /**
   * Constructs a new vector by copying from the given pointer
   * Does not take ownership of the pointer
   * @param data
   * @param n_elem
   */
  inline explicit vec(T* data, uint_32_cx n_elem) : size_(n_elem), capacity_(n_elem * 2) {
    arr_ = new T[capacity_];
    std::copy(data, data + n_elem, arr_);
  }
  /**
   * Initializer list constructor<p>
   * vec vec1 = {1,2,3};<p>
   * vec vec2{1,2,3};
   * @param init_list init list elements
   */
  inline vec(std::initializer_list<T> init_list)
      : size_(init_list.size()), capacity_(init_list.size() * 10),
        arr_(alloc.allocate(init_list.size() * 10)) {
    if (std::is_trivial_v<T>) {
      std::copy(init_list.begin(), init_list.end(), arr_);
    } else {
      auto it = init_list.begin();
      for (uint_fast32_t i = 0; i < init_list.size(); i++) {
        std::allocator_traits<Allocator>::construct(alloc, &arr_[i], *it);
        ++it;
      }
    }
  }
  inline vec(const vec<T>& o) : size_(o.size_), capacity_(o.capacity_) {
    arr_ = alloc.allocate(capacity_);
    if (std::is_trivial_v<T>) {
      std::copy(o.arr_, o.arr_ + o.size_, arr_);
    } else {
      std::uninitialized_copy(o.arr_, o.arr_ + o.size_, arr_);
    }
  }
  inline vec& operator=(const vec<T>& o) {
    if (this != &o) {
      if constexpr (!std::is_trivial_v<T>) {
        for (uint_32_cx i = 0; i < capacity_; i++) {
          std::allocator_traits<Allocator>::destroy(alloc, &arr_[i]);
        }
      }
      alloc.deallocate(arr_, capacity_);

      size_ = o.size_;
      capacity_ = o.capacity_;
      arr_ = alloc.allocate(capacity_);

      if (std::is_trivial_v<T>) {
        std::copy(o.arr_, o.arr_ + o.size_, arr_);
      } else {
        std::uninitialized_copy(o.arr_, o.arr_ + o.size_, arr_);
      }
    }
    return *this;
  }
  //move constructor
  inline vec(vec&& o) noexcept : arr_(o.arr_), size_(o.size_), capacity_(o.capacity_) {
    //leve other in previous state
    o.arr_ = nullptr;  // PREVENT DOUBLE DELETION!
  }
  //move assignment
  vec& operator=(vec&& o) noexcept {
    if (this != &o) {
      if (!std::is_trivial_v<T>) {
        for (uint_32_cx i = 0; i < capacity_; i++) {
          std::allocator_traits<Allocator>::destroy(alloc, &arr_[i]);
        }
      }
      alloc.deallocate(arr_, capacity_);

      arr_ = o.arr_;
      size_ = o.size_;
      capacity_ = o.capacity_;

      //other is left in previous state but invalidated
      o.arr_ = nullptr;  // PREVENT DOUBLE DELETION!
    }
    return *this;
  }
  inline ~vec() {
    if (!std::is_trivial_v<T>) {
      for (uint_32_cx i = 0; i < size_; i++) {
        std::allocator_traits<Allocator>::destroy(alloc, &arr_[i]);
      }
    }
    alloc.deallocate(arr_, capacity_);
  }
  /**
   * Direct access to the underlying array
   * @param index the accessed index
   * @return a reference to the value
   */
  [[nodiscard]] inline T& operator[](const uint_32_cx& index) const noexcept {
    CX_ASSERT(index < size_, "accessing undefined memory");
    return arr_[index];
  }
  /**
   * Allows direct access at the specified index starting with index 0 <p>
   * Negative indices can be used to access the list from the last element onwards starting with -1
   * Throws std::out_of_range on invalid index
   * @param T a reference to the value at the given index
   */
  [[nodiscard]] inline T& at(const int_32_cx& index) const noexcept {
    if (index < 0) {
      CX_ASSERT(size_ + index >= 0, "index out of bounds");
      return arr_[size_ + index];
    } else {
      CX_ASSERT(index < size_, "index out of bounds");
      return arr_[index];
    }
  }
  /**
   * Adds a element to the list
   * @param e the element to be added
   */
  inline void push_back(const T& e) noexcept {
    if (size_ == capacity_) {
      grow();
    }
    arr_[size_++] = e;
  }
  [[nodiscard]] inline T& front() const noexcept { return arr_[0]; }
  [[nodiscard]] inline T& back() const noexcept { return arr_[size_ - 1]; }
  /**
   * Construct a new T element at the end of the list
   * @param args T constructor arguments
   */
  template <typename... Args>
  inline void emplace_back(Args&&... args) noexcept {
    if (size_ == capacity_) {
      grow();
    }
    std::allocator_traits<Allocator>::construct(alloc, &arr_[size_++], std::forward<Args>(args)...);
  }

  /**
   * Reduces the underlying array size to something close to the actual data size.
   * This decreases memory usage.
   */
  inline void shrink_to_fit() noexcept {
    if (capacity_ > size_ * 1.5) {
      shrink();
    }
    CX_WARNING(capacity_ < size_ * 1.5, "trying to shrink already fitting vec");
  }
  /**
  * Removes the last element of the vec.
  * Reduces the size by one.
  */
  inline void pop_back() noexcept {
    CX_ASSERT(size_ > 0, "out of bounds");
    size_--;
    if (!std::is_trivial_v<T>) {
      std::allocator_traits<Allocator>::destroy(alloc, &arr_[size_]);
    }
  }
  /**
   * Removes the first element of the vec<p>
   * This is rather slow method(O(n)) use a Queue if you call it frequently
   * @return first element as value
   */
  inline void pop_front() noexcept {
    CX_ASSERT(size_ > 0, "out of bounds");
    if (!std::is_trivial_v<T>) {
      std::allocator_traits<Allocator>::destroy(alloc, &arr_[0]);
    }
    std::move(arr_ + 1, arr_ + size_--, arr_);
  }
  /**
   * Removes the element at index i of the vec<p>
   * This is rather slow method(O(n-i))
   * @return first element as value
   */
  inline void pop(const uint_32_cx& i) noexcept {
    CX_ASSERT(i < size_, "out of bounds");
    if (!std::is_trivial_v<T>) {
      std::allocator_traits<Allocator>::destroy(alloc, &arr_[i]);
    }
    std::move(arr_ + i + 1, arr_ + size_--, arr_ + i);
  }
  /**
   * Removes the first occurrence of the given element from the list
   * @param e element to be removed
   */
  inline void erase(const T& e) noexcept {
#  pragma omp simd linear(i : 1)
    for (uint_32_cx i = 0; i < capacity_; i++) {
      if (arr_[i] == e) {
        std::move(arr_ + i + 1, arr_ + size_, arr_ + i);
        size_--;
        return;
      }
    }
  }
  template <typename lambda>
  inline void erase_if(lambda condition) {
    for (uint_32_cx i = 0; i < capacity_; i++) {
      if (condition(arr_[i])) {
        std::move(arr_ + i + 1, arr_ + size_, arr_ + i);
        size_--;
        return;
      }
    }
  }
  /**
   * Removes the element at the given index
   * @param index index of removal
   */
  inline void removeAt(const uint_32_cx& index) noexcept {
    CX_ASSERT(index < capacity_, "index out of bounds");
    std::move(arr_ + index + 1, arr_ + size_--, arr_ + index);
  }
  /**
 *
 * @return the current n_elem of the list
 */
  [[nodiscard]] inline uint_32_cx size() const noexcept { return size_; }
  [[nodiscard]] inline uint_32_cx capacity() const noexcept { return capacity_; }
  inline void reserve(uint_32_cx new_capacity) noexcept {
    if (capacity_ < new_capacity) {
      capacity_ = new_capacity;

      T* n_arr = alloc.allocate(capacity_);

      std::uninitialized_move(arr_, arr_ + size_, n_arr);

      // Destroy the original objects
      if (!std::is_trivial_v<T>) {
        for (size_t i = 0; i < size_; i++) {
          std::allocator_traits<Allocator>::destroy(alloc, &arr_[i]);
        }
      }
      alloc.deallocate(arr_, size_);

      arr_ = n_arr;
    }
  }
  /**
   * Clears the list of all its elements <br>
   * Resets the length back to its starting value
   */
  inline void clear() noexcept {
    int ar[3] = {2, 2, 1};
    if (!std::is_trivial_v<T>) {
      for (uint_32_cx i = 0; i < capacity_; i++) {
        std::allocator_traits<Allocator>::destroy(alloc, &arr_[i]);
      }
    }
    alloc.deallocate(arr_, capacity_);
    size_ = 0;
    capacity_ = 32;
    arr_ = alloc.allocate(32);
  }
  /**
   * Provides access to the underlying array which can be used for sorting
   * Use with caution! Valid data is only guaranteed from 0 up to n_elem
   * @return a pointer to the data array
   */
  [[nodiscard]] inline T* get_raw() const noexcept { return arr_; }
  /**
   * Searches the whole vector starting from the beginning (default)
   * @param val value to search for
   * @param startFront whether to start from the front
   * @return true if the value was found
   */
  [[nodiscard]] inline bool contains(const T& val, bool startFront = true) const noexcept {
    if (startFront) {
      for (int i = 0; i < size_; i++) {
        if (arr_[i] == val) {
          return true;
        }
      }
      return false;
    } else {
      for (int i = size_ - 1; i > -1; i++) {
        if (arr_[i] == val) {
          return true;
        }
      }
      return false;
    }
  }
  /**
 * Appends the contents of another vec at the end of this vec <p>
 * If necessary, the capacity of this vec is expanded
 * @param vec  the vec to append
 */
  inline void append(const vec<T>& vec) noexcept {
    while (capacity_ - size_ < vec.size_) {
      grow();
    }
    std::copy(vec.arr_, vec.arr_ + vec.size_, arr_ + size_);
    size_ += vec.size_;
  }
  /**
 * Appends a specified range of elements from another vec to this vec.
 * If necessary, the capacity of this vec is expanded.
 * Throws a std::out_of_range exception if the start or end indices are not valid.
 *
 * @param list the list to append
 * @param end index of the last element (exclusive)
 * @param start the index of the first element (inclusive)
 */
  inline void append(const vec<T>& list, uint_32_cx endIndex, uint_32_cx startIndex = 0) noexcept {
    CX_ASSERT(startIndex < endIndex || endIndex <= list.size_, "index out of bounds");
    while (capacity_ - size_ < endIndex - startIndex) {
      grow();
    }
    std::copy(list.arr_ + startIndex, list.arr_ + endIndex, arr_ + size_);
    size_ += endIndex - startIndex;
  }
  /**
  * Attempts to bits_print the complete list to std::cout
  * @param prefix optional prefix
  */
  inline void print(const std::string& prefix = "") {
    if (!prefix.empty()) {
      std::cout << prefix << std::endl;
      std::cout << "   ";
    }
    std::cout << *this << std::endl;
  }
  friend std::ostream& operator<<(std::ostream& os, const vec& o) noexcept {
    if (o.size_ == 0) {
      return os << "[]";
    }
    os << "[" << o.arr_[0];

    for (int i = 1; i < o.size_; i++) {
      os << "," << o.arr_[i];
    }
    return os << "]";
  }
  /**
   * Performs an empty check
   * @return true if the vec is empty
   */
  [[nodiscard]] inline bool empty() const noexcept { return size_ == 0; }
  /**
   * Sorts the vector in the given direction<p>
   * Uses the cxalgos::quicksort which should be slightly faster than STL sort()
   * @param ascending true if ascending, false if descending
   */
  inline void sort(bool ascending = true) noexcept { quick_sort(arr_, size_, ascending); }
  /**
   * Sorts the vector using a custom comparator of the form: comp(T,T)(bool)
   * If true swaps the elements
   * @tparam Comparator callable taking two T and returning bool
   * @param comp a callable function (lambda)
   */
  template <typename Comparator,
            typename = std::enable_if_t<std::is_invocable_r_v<bool, Comparator, T, T>>>
  inline void sort(Comparator comp) noexcept {
    quick_sort_comparator(arr_, size_, comp);
  }
  /**
   * Iterates through the vector finding the biggest element by ">" comparison
   * @return the index of the biggest element
   */
  inline uint_32_cx max_element() const noexcept {
    T max = arr_[0];
    uint_32_cx index = 0;
    for (uint_fast32_t i = 1; i < size_; i++) {
      if (arr_[i] > max) {
        max = arr_[i];
        index = i;
      }
    }
    return index;
  }
  /**
 * Trims the vector to size new_size counting from the front
 * @param new_size  size the vector will have after calling this method
 */
  inline void resize(uint_32_cx new_size) noexcept {
    CX_WARNING(!(size_ <= new_size), "calling grow for no reason");
    if (size_ > new_size) {
      auto old_len = capacity_;
      capacity_ = new_size;

      T* n_arr = alloc.allocate(capacity_);

      std::uninitialized_move(arr_, arr_ + new_size, n_arr);

      // Destroy the original objects
      if (!std::is_trivial_v<T>) {
        for (size_t i = 0; i < old_len; i++) {
          std::allocator_traits<Allocator>::destroy(alloc, &arr_[i]);
        }
      }

      alloc.deallocate(arr_, old_len);
      arr_ = n_arr;
      size_ = new_size;
    }
  }

  //multidimensional distance functions
  inline static float dist_chebyshev(const vec<float>& p1, const vec<float>& p2) noexcept {
    float max = 0;
    for (uint_fast32_t i = 0; i < p1.size(); i++) {
      if (p2[i] - p1[i] > max) {
        max = p2[i] - p1[i];
      }
      return max;
    }
  }
  class Iterator {
    T* ptr;

   public:
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = T*;
    using reference = T&;
    using iterator_category = std::random_access_iterator_tag;

    inline explicit Iterator(T* p) noexcept : ptr(p) {}

    inline reference operator*() const noexcept { return *ptr; }
    inline pointer operator->() const noexcept { return ptr; }

    inline Iterator& operator++() noexcept {
      ++ptr;
      return *this;
    }
    inline Iterator operator++(int) const noexcept {
      Iterator temp = *this;
      ++ptr;
      return temp;
    }
    inline Iterator& operator--() noexcept {
      --ptr;
      return *this;
    }
    inline Iterator operator--(int) noexcept {
      Iterator temp = *this;
      --ptr;
      return temp;
    }
    inline Iterator operator+(difference_type n) const noexcept { return Iterator(ptr + n); }
    inline Iterator operator-(difference_type n) const noexcept { return Iterator(ptr - n); }
    inline difference_type operator-(const Iterator& other) const noexcept {
      return ptr - other.ptr;
    }
    inline Iterator& operator+=(difference_type n) noexcept {
      ptr += n;
      return *this;
    }
    inline Iterator& operator-=(difference_type n) noexcept {
      ptr -= n;
      return *this;
    }

    inline reference operator[](difference_type n) const noexcept { return ptr[n]; }

    inline bool operator==(const Iterator& other) const noexcept { return ptr == other.ptr; }
    inline bool operator!=(const Iterator& other) const noexcept { return ptr != other.ptr; }
    inline bool operator<(const Iterator& other) const noexcept { return ptr < other.ptr; }
    inline bool operator>(const Iterator& other) const noexcept { return ptr > other.ptr; }
    inline bool operator<=(const Iterator& other) const noexcept { return ptr <= other.ptr; }
    inline bool operator>=(const Iterator& other) const noexcept { return ptr >= other.ptr; }
  };
  inline Iterator begin() { return Iterator(arr_); }
  inline Iterator end() { return Iterator(arr_ + size_); }

#  ifdef CX_INCLUDE_TESTS
  static void TEST() {
    std::cout << "TESTING VEC\n";

    // Test: Testing push_back and erase
    std::cout << "   Testing push_back and erase...\n";
    vec<int> list1;
    list1.push_back(5);
    list1.push_back(10);
    list1.push_back(15);

    list1.erase(10);
    CX_ASSERT(list1.size() == 2, "");
    CX_ASSERT(list1[1] == 15, "");

    // Test: Testing List access
    std::cout << "   Testing List access...\n";
    CX_ASSERT(list1[0] == 5, "");
    CX_ASSERT(list1.at(-1) == 15, "");
    CX_ASSERT(list1.at(-2) == 5, "");
    CX_ASSERT(list1[1] == 15, "");

    // Test: Testing iterator
    std::cout << "   Testing iterator...\n";
    list1.clear();
    list1.push_back(5);
    list1.push_back(10);
    list1.push_back(15);

    int checkNum = 0;
    for (auto& num : list1) {
      checkNum += 5;
      CX_ASSERT(num == checkNum, "");
    }
    CX_ASSERT(checkNum == 15, "");

    // Test: Testing resizing and shrink_to_fit
    std::cout << "   Testing resizing and shrink_to_fit...\n";
    list1.clear();
    for (int i = 0; i < 10000; i++) {
      list1.push_back(i);
    }
    list1.shrink_to_fit();
    CX_ASSERT(list1.capacity() == list1.size() * 1.5, "");

    for (int i = 0; i < 10000; i++) {
      list1.erase(i);
    }
    CX_ASSERT(list1.size() == 0, "");

    // Test: Testing contained
    std::cout << "   Testing contained...\n";
    list1.clear();
    list1.push_back(5);
    CX_ASSERT(list1.contains(5) == true, "");
    CX_ASSERT(list1.contains(5, false) == true, "");

    // Test: Testing append
    std::cout << "   Testing append...\n";
    list1.clear();

    list1.push_back(5);
    list1.push_back(10);
    vec<int> list2;
    for (int i = 0; i < 1000000; i++) {
      list2.push_back(i);
    }
    CX_ASSERT(list2.size() == 1000000, "");

    list1.append(list2);
    CX_ASSERT(list1.size() == 1000002, "");
    CX_ASSERT(list2[10] == 10, "");

    list1.clear();

    list1.append(list2, 10, 1);
    int check = 1;
    for (auto num : list1) {
      CX_ASSERT(check++ == num, "");
    }
    CX_ASSERT(list1.size() == 9, "");

    // Test: Testing copy constructor
    std::cout << "   Testing copy constructor...\n";
    vec<int> list5(10);
    for (uint_32_cx i = 0; i < 10; i++) {
      list5.push_back(i);
    }
    vec<int> list6 = list5;  // copy constructor
    for (uint_32_cx i = 0; i < 10; i++) {
      CX_ASSERT(list6[i] == i, "");
    }

    // Test: Testing copy assignment
    std::cout << "   Testing copy assignment...\n";
    vec<int> list7(10);
    list7 = list5;  // copy assignment
    for (uint_32_cx i = 0; i < 10; i++) {
      CX_ASSERT(list7[i] == i, "");
    }

    // Test: Testing move constructor
    std::cout << "   Testing move constructor...\n";
    vec<int> list8 = std::move(list5);  // move constructor
    for (uint_32_cx i = 0; i < 10; i++) {
      CX_ASSERT(list8[i] == i, "");
    }

    // Test: Testing move assignment
    std::cout << "   Testing move assignment...\n";
    vec<int> list9(10);
    list9 = std::move(list6);  // move assignment
    for (uint_32_cx i = 0; i < 10; i++) {
      CX_ASSERT(list9[i] == i, "");
    }

    // Test: Testing pop_back()
    std::cout << "   Testing pop_back()...\n";
    list9.push_back(100);
    CX_ASSERT(list9.back() == 100, "");
    list9.pop_back();
    CX_ASSERT(list9.size() == 10, "");

    // Test: Checking for memory leaks
    std::cout << "   Checking for memory leaks...\n";
    list1.clear();
    for (int i = 0; i < 10000000; i++) {
      list1.push_back(i);
    }

    std::cout << "   Testing pop_front()...\n";
    list1.clear();
    for (uint_fast32_t i = 0; i < 10; i++) {
      list1.push_back(i);
    }
    CX_ASSERT(list1.front() == 0, "");
    list1.pop_front();
    CX_ASSERT(list1.size() == 9, "");
    for (uint_fast32_t i = 1; i < 10; i++) {
      CX_ASSERT(list1[i - 1] == i, "");
    }
    CX_ASSERT(list1.back() == 9, "");
    list1.pop_back();

    CX_ASSERT(list1.size() == 8, "");
    for (uint_fast32_t i = 1; i < 9; i++) {
      CX_ASSERT(list1[i - 1] == i, "");
    }

    list1.pop(3);
    CX_ASSERT(list1.size() == 7, "");
    CX_ASSERT(list1[3] == 5, "");
    CX_ASSERT(list1[3] == 5, "");
    list1.pop(3);
    CX_ASSERT(list1.size() == 6, "");
    CX_ASSERT(list1[3] == 6, "");
  }
#  endif
};
}  // namespace cxstructs
#endif  // CXSTRUCTS_ARRAYLIST_H
