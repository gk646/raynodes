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
#ifndef CXSTRUCTS_SRC_DATASTRUCTURES_DEQUEUE_H_
#  define CXSTRUCTS_SRC_DATASTRUCTURES_DEQUEUE_H_

#  include <algorithm>

#  include <cstdint>
#  include <iostream>
#  include "../cxconfig.h"

namespace cxstructs {
/**
 * <h2>DeQueue</h2> is a double ended queue. It functions very similar to the normal queue but is different in that it allows for retrieval and addition at both ends.
 * Like the Queue this implementation also uses a circular array to manage the data.
 */
template <typename T, typename Allocator = std::allocator<T>, typename size_type = uint32_t>
class DeQueue {
  Allocator alloc;
  T* arr_;
  uint_32_cx len_;
  uint_32_cx size_;

  int_32_cx front_;
  int_32_cx back_;

  inline void grow() {
    len_ *= 2;

    T* n_arr = alloc.allocate(len_);

    /*
     * We cant really optimize this here
     * Whenever grow() is called size_ == len_
     * thus arr_ always completely filled
     * we just use two moves to bring it in the right order again
     */
    if (front_ == 0) {
      std::uninitialized_move(arr_, arr_ + size_, n_arr);
    } else {
      std::uninitialized_move(arr_ + front_, arr_ + size_, n_arr);
      std::uninitialized_move(arr_, arr_ + front_, n_arr + size_ - front_);
    }

    if constexpr (!std::is_trivial_v<T>) {
      for (uint_fast32_t i = 0; i < size_; i++) {
        std::allocator_traits<Allocator>::destroy(alloc, &arr_[i]);
      }
    }
    alloc.deallocate(arr_, len_);

    arr_ = n_arr;
    front_ = 0;
    back_ = size_ - 1;
  }
  inline void shrink() {
    auto old_len = len_;
    len_ = size_ * 1.5;

    T* n_arr = alloc.allocate(len_);

    if (front_ == 0) {
      std::uninitialized_move(arr_, arr_ + size_, n_arr);
    } else {
      std::uninitialized_move(arr_ + front_, arr_ + size_, n_arr);
      std::uninitialized_move(arr_, arr_ + front_, n_arr + size_ - front_);
    }

    if constexpr (!std::is_trivial_v<T>) {
      for (size_t i = 0; i < old_len; i++) {
        std::allocator_traits<Allocator>::destroy(alloc, &arr_[i]);
      }
    }

    alloc.deallocate(arr_, old_len);
    arr_ = n_arr;
    front_ = 0;
    back_ = size_ - 1;
  }

 public:
  inline explicit DeQueue(uint_32_cx len = 32)
      : arr_(alloc.allocate(len)), len_(len), back_(-1), size_(0), front_(0) {}
  DeQueue(const DeQueue& o) : size_(o.size_), len_(o.len_), front_(o.front_), back_(o.back_) {
    arr_ = alloc.allocate(len_);
    if (std::is_trivial_v<T>) {
      std::copy(o.arr_, o.arr_ + o.len_, arr_);
    } else {
      std::uninitialized_copy(o.arr_, o.arr_ + o.len_, arr_);
    }
  }
  DeQueue& operator=(const DeQueue& o) {
    if (this != &o) {
      if (!std::is_trivial_v<T>) {
        for (uint_32_cx i = 0; i < len_; i++) {
          std::allocator_traits<Allocator>::destroy(alloc, &arr_[i]);
        }
      }
      alloc.deallocate(arr_, len_);

      front_ = o.front_;
      len_ = o.len_;
      size_ = o.size_;
      arr_ = alloc.allocate(len_);

      if constexpr (std::is_trivial_v<T>) {
        std::copy(o.arr_, o.arr_ + o.len_, arr_);
      } else {
        std::uninitialized_copy(o.arr_, o.arr_ + o.len_, arr_);
      }
    }
    return *this;
  }
  //move copy
  DeQueue(DeQueue&& o) noexcept
      : front_(o.front_), len_(o.len_), arr_(o.arr_), size_(o.size_), back_(o.back_) {
    o.arr_ = nullptr;  //avoid double deletion
    o.size_ = 0;
  }
  //move assign operator
  DeQueue& operator=(DeQueue&& o) noexcept {
    if (this != &o) {
      if constexpr (!std::is_trivial_v<T>) {
        for (uint_32_cx i = 0; i < len_; i++) {
          std::allocator_traits<Allocator>::destroy(alloc, &arr_[i]);
        }
      }
      alloc.deallocate(arr_, len_);

      arr_ = o.arr_;
      size_ = o.size_;
      len_ = o.len_;
      front_ = o.front_;
      back_ = o.back_;

      o.size_ = 0;
      o.arr_ = nullptr;
    }
    return *this;
  }

  inline ~DeQueue() {
    if constexpr (!std::is_trivial_v<T>) {
      uint_32_cx end = (front_ + size_) % len_;
      if (end < front_) {
        for (uint_32_cx i = front_; i < len_; i++) {
          std::allocator_traits<Allocator>::destroy(alloc, &arr_[i]);
        }
        for (uint_32_cx i = 0; i < end; i++) {
          std::allocator_traits<Allocator>::destroy(alloc, &arr_[i]);
        }
      } else {
        for (uint_32_cx i = front_; i < front_ + size_; i++) {
          std::allocator_traits<Allocator>::destroy(alloc, &arr_[i]);
        }
      }
    }
    alloc.deallocate(arr_, len_);
  }
  /**
 * Emplaces an element at the front of the deque.
 * Constructs the element in-place using the given arguments.
 * @param args Arguments to forward to the constructor of the element.
 */
  template <typename... Args>
  inline void emplace_front(Args&&... args) {
    if (size_ == len_) {
      grow();
    }
    if (--front_ < 0) {
      front_ = len_ - 1;
    }
    std::allocator_traits<Allocator>::construct(alloc, &arr_[front_], std::forward<Args>(args)...);
    size_++;
  }
  /**
 * Emplaces an element at the back of the deque.
 * Constructs the element in-place using the given arguments.
 * @param args Arguments to forward to the constructor of the element.
 */
  template <typename... Args>
  inline void emplace_back(Args&&... args) {
    if (size_ == len_) {
      grow();
    }

    if (++back_ >= len_) {
      back_ = 0;
    }
    std::allocator_traits<Allocator>::construct(alloc, &arr_[back_], std::forward<Args>(args)...);
    size_++;
  }
  /**
 * Adds an element to the front of the deque.
 * @param e The element to be added.
 */
  inline void push_front(const T& e) {
    if (size_ == len_) {
      grow();
    }
    if (--front_ < 0) {
      front_ = len_ - 1;
    }
    arr_[front_] = e;
    size_++;
  }
  /**
 * Adds an element to the back of the deque.
 * @param e The element to be added.
 */
  inline void push_back(const T& e) {
    if (size_ == len_) {
      grow();
    }
    if (++back_ >= len_) {
      back_ = 0;
    }
    arr_[back_] = e;
    size_++;
  }
  /**
 * Removes and returns an element from the back of the deque.
 * @return The removed element.
 */
  inline void pop_back() noexcept {
    CX_ASSERT(size_ > 0, "no such element");
    std::allocator_traits<Allocator>::destroy(alloc, &arr_[back_]);
    if (--back_ < 0) {
      back_ = len_ - 1;
    }
    size_--;
  }
  /**
 * Removes and returns an element from the front of the deque.
 * @return The removed element.
 */
  inline void pop_front() noexcept {
    CX_ASSERT(size_ > 0, "no such element");
    std::allocator_traits<Allocator>::destroy(alloc, &arr_[front_]);
    if (++front_ >= len_) {
      front_ = 0;
    }
    size_--;
  }
  /**
 * Accesses the last element in the deque.
 * @return The reference to the last element in the deque.
 */
  [[nodiscard]] inline T& back() const { return arr_[back_]; }
  /**
 * Accesses the first element in the deque.
 * @return The reference to the first element in the deque.
 */
  [[nodiscard]] inline T& front() const { return arr_[front_]; }
  /**
 *
 * @return the current n_elem of the dequeue
 */
  [[nodiscard]] inline uint_32_cx size() const { return size_; }
  /**
   * Clears the queue of all elements
   */
  inline void clear() noexcept {
    if (!std::is_trivial_v<T>) {
      uint_32_cx end = (front_ + size_) % len_;
      if (end < front_) {
        for (uint_32_cx i = front_; i < len_; i++) {
          std::allocator_traits<Allocator>::destroy(alloc, &arr_[i]);
        }
        for (uint_32_cx i = 0; i < end; i++) {
          std::allocator_traits<Allocator>::destroy(alloc, &arr_[i]);
        }
      } else {
        for (uint_32_cx i = front_; i < front_ + size_; i++) {
          std::allocator_traits<Allocator>::destroy(alloc, &arr_[i]);
        }
      }
    }
    alloc.deallocate(arr_, len_);

    size_ = 0;
    front_ = 0;
    len_ = 32;
    back_ = -1;
    arr_ = alloc.allocate(32);
  }
  /**
    * The current max capacity of the dequeue
    * @return
    */
  [[nodiscard]] inline uint_32_cx capacity() const noexcept { return len_; }
  inline void shrink_to_fit() noexcept {
    CX_WARNING(len_ > size_ * 1.5, "");
    shrink();
  }
  friend std::ostream& operator<<(std::ostream& os, DeQueue& q) {
    if (q.size() == 0) {
      return os << "[]";
    }
    os << "[" << q.front();
    for (uint_32_cx i = 2; i < q.back_ + q.front_; i++) {
      os << "," << q.arr_[q.mid_ - q.front_ + i];
    }
    return os << "]";
  }
  class Iterator {
    T* ptr;
    uint_32_cx current;
    uint_32_cx len;

   public:
    explicit Iterator(T* p, uint_32_cx start, uint_32_cx len) : len(len), ptr(p), current(start) {}
    T& operator*() { return ptr[current]; }
    Iterator& operator++() {
      if (++current >= len) {
        current = 0;
      }
      return *this;
    }
    bool operator!=(const Iterator& other) const { return current != other.current; }
    bool operator==(const Iterator& other) const { return current == other.current; }
  };
  inline Iterator begin() { return Iterator(arr_, front_, len_); }
  inline Iterator end() { return Iterator(arr_, (back_ + 1) % len_, len_); }
#  ifdef CX_INCLUDE_TESTS
#    include <deque>
  static void TEST() {
    std::cout << "TESTING DEQUEUE" << std::endl;

    // Testing push_front and front
    std::cout << "   Testing push_front and front..." << std::endl;
    DeQueue<int> de_queue;
    for (uint_32_cx i = 0; i < 100; i++) {
      de_queue.push_front(i);
      CX_ASSERT(de_queue.front() == i, "");
      CX_ASSERT(de_queue.size() == i + 1, "");
    }

    // Testing push_back and back
    std::cout << "   Testing push_back and back..." << std::endl;
    for (uint_32_cx i = 100; i < 200; i++) {
      de_queue.push_back(i);
      CX_ASSERT(de_queue.back() == i, "");
      CX_ASSERT(de_queue.size() == i + 1, "");
    }

    // Testing pop_front
    std::cout << "   Testing pop_front..." << std::endl;
    for (int i = 99; i >= 0; i--) {
      CX_ASSERT(de_queue.front() == i, "");
      de_queue.pop_front();
      CX_ASSERT(de_queue.size() == i + 100, "");
    }

    // Testing pop_back
    std::cout << "   Testing pop_back..." << std::endl;
    for (uint_32_cx i = 199; i >= 100; i--) {
      CX_ASSERT(de_queue.back() == i, "");
      de_queue.pop_back();
      CX_ASSERT(de_queue.size() == i - 100, "");
    }

    // Testing edge case: adding and removing a large number of elements
    std::cout << "   Testing edge case: adding and removing a large number of elements..."
              << std::endl;
    for (uint_32_cx i = 0; i < 100000; i++) {
      de_queue.push_back(i);
    }
    for (uint_32_cx i = 0; i < 100000; i++) {
      CX_ASSERT(de_queue.size() == 100000 - i, "");

      CX_ASSERT(de_queue.front() == i, "");

      de_queue.pop_front();
    }
    CX_ASSERT(de_queue.size() == 0, "");

    // Testing clear
    std::cout << "   Testing clear..." << std::endl;
    for (int i = 0; i < 1000; i++) {
      de_queue.push_back(i);
    }
    de_queue.clear();
    CX_ASSERT(de_queue.size() == 0, "");

    // Testing back and pop_back
    std::cout << "   Testing back and pop_back..." << std::endl;
    for (int i = 0; i < 1000; i++) {
      de_queue.push_back(i);
    }
    for (int i = 999; i >= 0; i--) {
      CX_ASSERT(de_queue.back() == i, "");

      de_queue.pop_back();
    }
    CX_ASSERT(de_queue.size() == 0, "");

    // Testing copy constructor and assignment
    std::cout << "   Testing copy constructor and assignment..." << std::endl;
    for (int i = 0; i < 1000; i++) {
      de_queue.push_back(i);
    }
    DeQueue<int> de_queue_copy = de_queue;
    DeQueue<int> de_queue_assign;
    de_queue_assign = de_queue;
    CX_ASSERT(de_queue.size() == de_queue_copy.size(), "");

    CX_ASSERT(de_queue.size() == de_queue_assign.size(), "");

    // Test constructors and assignment operators
    std::cout << "   Testing constructors and assignment operators..." << std::endl;
    DeQueue<int> dq1;
    for (uint_32_cx i = 0; i < 100; i++) {
      dq1.push_back(i);
    }
    // Copy constructor
    DeQueue<int> dq2(dq1);
    CX_ASSERT(dq2.size() == dq1.size(), "");

    for (uint_32_cx i = 0; i < 50; i++) {
      CX_ASSERT(dq2.front() == dq1.front(), "");

      dq2.pop_front();
      dq1.pop_front();
    }
    // Copy assignment operator
    dq1 = dq2;
    CX_ASSERT(dq2.size() == dq1.size(), "");

    for (uint_32_cx i = 0; i < 25; i++) {
      CX_ASSERT(dq2.front() == dq1.front(), "");

      dq2.pop_front();
      dq1.pop_front();
    }
    // Move constructor
    DeQueue<int> dq3(std::move(dq2));
    CX_ASSERT(dq3.size() == 25, "");

    CX_ASSERT(dq2.size() == 0, "");

    for (uint_32_cx i = 75; i < 100; i++) {
      CX_ASSERT(dq3.front() == i, "");

      dq3.pop_front();
    }
    for (uint_fast32_t i = 0; i < 10; i++) {
      dq3.push_back(i);
    }
    // Move assignment operator
    dq1 = std::move(dq3);
    CX_ASSERT(dq1.size() == 10, "");

    CX_ASSERT(dq3.size() == 0, "");

    for (uint_32_cx i = 0; i < 10; i++) {
      CX_ASSERT(dq1.front() == i, "");

      dq1.pop_front();
    }

    de_queue.clear();
    std::cout
        << "   Testing edge case: adding and removing a large number of elements with iterator..."
        << std::endl;
    for (uint_32_cx i = 0; i < 100000; i++) {
      de_queue.push_back(i);
    }

    uint_32_cx i = 0;
    for (auto it = de_queue.begin(); it != de_queue.end(); ++it, ++i) {
      CX_ASSERT(*it == i, "");
    }
    de_queue.clear();
    CX_ASSERT(de_queue.size() == 0, "");

    std::cout << "   Testing clear with iterator..." << std::endl;
    for (int i = 0; i < 1000; i++) {
      de_queue.push_back(i);
    }
    de_queue.clear();
    CX_ASSERT(de_queue.size() == 0, "");

    CX_ASSERT(de_queue.begin() == de_queue.end(), "");

    std::cout << "   Testing wrapping around with iterator..." << std::endl;
    DeQueue<int> qq;
    std::deque<int> stdqq;
    for (uint_fast32_t j = 0; j < 10; j++) {
      qq.push_back(j);
      stdqq.push_back(j);
    }
    for (uint_fast32_t j = 0; j < 5; j++) {
      qq.pop_front();
      stdqq.pop_front();
    }
    for (uint_fast32_t j = 0; j < 25; j++) {
      qq.push_back(j + 20);
      stdqq.push_back(j + 20);
    }
    CX_ASSERT(qq.front() == stdqq.front(), "");

    for (uint_fast32_t j = 0; j < 5; j++) {
      qq.pop_front();
    }
    int b = 20;
    for (auto val : qq) {
      CX_ASSERT(b++ == val, "");
    }
  }
#  endif
};
}  // namespace cxstructs
#endif  //CXSTRUCTS_SRC_DATASTRUCTURES_DEQUEUE_H_
