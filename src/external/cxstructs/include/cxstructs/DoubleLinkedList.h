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
#ifndef CXSTRUCTURES_DOUBLELINKEDLIST_H
#  define CXSTRUCTURES_DOUBLELINKEDLIST_H

#  include <algorithm>

#  include <iostream>
#  include "../cxconfig.h"

namespace cxhelper {
template <typename T>
struct DListNode {
  T val_;
  DListNode* next_;
  DListNode* previous_;
  explicit DListNode(const T& val) : val_(val), next_(nullptr), previous_(nullptr){};
  explicit DListNode(const T& val, DListNode<T>* previous)
      : val_(val), next_(nullptr), previous_(previous){};
};
}  // namespace cxhelper

namespace cxstructs {
using namespace cxhelper;

template <typename T>
class DoubleLinkedList {
  DListNode<T>* head_;
  DListNode<T>* end_;

  uint_32_cx size_;

 public:
  DoubleLinkedList() : size_(0), head_(nullptr), end_(nullptr){};
  DoubleLinkedList(const DoubleLinkedList<T>& o) : size_(o.size_) {
    if (o.head_) {
      head_ = new DListNode<T>(o.head_->val_, nullptr);

      DListNode<T>* current_new = head_;
      DListNode<T>* current_old = o.head_->next_;

      while (current_old != nullptr) {
        current_new->next_ = new DListNode<T>(current_old->val_, current_new);

        current_new = current_new->next_;
        current_old = current_old->next_;
      }
      end_ = current_new;
    } else {
      head_ = end_ = nullptr;
    }
  }
  DoubleLinkedList& operator=(const DoubleLinkedList<T>& o) {
    if (this == &o) {
      return *this;
    }

    clear();
    size_ = o.size_;

    if (o.head_) {
      head_ = new DListNode<T>(o.head_->val_, nullptr);

      DListNode<T>* current_new = head_;
      DListNode<T>* current_old = o.head_->next_;

      while (current_old != nullptr) {
        current_new->next_ = new DListNode<T>(current_old->val_, current_new);

        current_new = current_new->next_;
        current_old = current_old->next_;
      }
      end_ = current_new;
    } else {
      delete head_;
      delete end_;
      head_ = end_ = nullptr;
    }
    return *this;
  }
  ~DoubleLinkedList() {
    DListNode<T>* current = head_;
    while (current != nullptr) {
      DListNode<T>* next = current->next_;
      delete current;
      current = next;
    }
  }
  /** Adds a new element to the end of the list
  * @param val - the element to be added
   */
  void add(T val) {
    if (!head_) {
      head_ = new DListNode<T>(val);
      end_ = head_;
    } else {
      end_->next_ = new DListNode<T>(val, end_);
      end_ = end_->next_;
    }
    size_++;
  }
  /**
   * Removes the first node with the given value
  * @param val - the value to be matched
  */
  void clear() {
    DListNode<T>* current = head_;
    while (current != nullptr) {
      DListNode<T>* next = current->next_;
      delete current;
      current = next;
    }
    head_ = nullptr;
    end_ = nullptr;
    size_ = 0;
  }
  /**
    * @return the current n_elem of this Linked List
    */
  [[nodiscard]] uint_32_cx size() const { return size_; }
  /**
  * Removes the element at index counting from the start node
  * @param index  - the index at which to erase the element
  * @return the element removed with this operation
  */
  T removeAt(uint_32_cx index) {
    if (index >= size_) throw std::out_of_range("index out of bounds");

    DListNode<T>* toDelete;
    T val;
    if (index == 0) {
      toDelete = head_;
      head_ = head_->next_;
      if (head_) {
        head_->previous_ = nullptr;
      } else {
        delete head_;
        end_ = nullptr;
      }
    } else {
      DListNode<T>* it = head_;
      for (uint_32_cx i = 0; i < index; i++) {
        it = it->next_;
      }

      toDelete = it;
      if (it->next_) {
        it->next_->previous_ = it->previous_;
      } else {
        end_ = it->previous_;
      }
      if (it->previous_) {
        it->previous_->next_ = it->next_;
      }
    }

    val = toDelete->val_;
    delete toDelete;
    --size_;

    return val;
  }

  class Iterator {
   public:
    DListNode<T>* current;

    explicit Iterator(DListNode<T>* start) : current(start){};

    T& operator*() { return current->val_; }

    Iterator& operator++() {
      if (current) {
        current = current->next_;
      }
      return *this;
    }

    bool operator==(const Iterator& other) const { return current == other.current; }

    bool operator!=(const Iterator& other) const { return current != other.current; }
  };

  Iterator begin() {
    if (size_ > 0) {
      return Iterator(head_);
    }
    return Iterator(nullptr);
  }
  Iterator end() { return Iterator(nullptr); }

  class ReverseIterator {
   public:
    DListNode<T>* current;

    explicit ReverseIterator(DListNode<T>* start) : current(start){};

    T& operator*() { return current->val_; }

    ReverseIterator& operator++() {
      if (current) {
        current = current->previous_;
      }
      return *this;
    }

    bool operator==(const ReverseIterator& other) const { return current == other.current; }

    bool operator!=(const ReverseIterator& other) const { return current != other.current; }
  };

  ReverseIterator begin_re() { return ReverseIterator(end_); }
  ReverseIterator end_re() { return ReverseIterator(nullptr); }
#  ifdef CX_INCLUDE_TESTS
  static void TEST() {
    std::cout << "DOUBLE LINKED LIST TESTS" << std::endl;
    DoubleLinkedList<int> list1;
    list1.add(5);
    list1.add(10);
    list1.add(15);

    // Test Copy Constructor
    DoubleLinkedList<int> list5(list1);

    CX_ASSERT(list1.size() == list5.size(), "");
    std::cout << "  Testing assign operator..." << std::endl;
    // Test Assignment Operator
    DoubleLinkedList<int> list6;
    list6 = list1;

    int num = 5;
    for (auto val : list1) {
      CX_ASSERT(val == val, "");

      val += 5;
    }
    num = 5;
    for (auto val : list6) {
      CX_ASSERT(val == num, "");

      num += 5;
    }
    CX_ASSERT(list1.size() == list6.size(), "");

    DoubleLinkedList<int> list;
    list.add(1);
    CX_ASSERT(list.size() == 1, "");

    list.add(2);
    CX_ASSERT(list.size() == 2, "");

    std::cout << "  Testing copy constructor..." << std::endl;
    list1.clear();
    list1.add(5);
    list1.add(10);
    auto list10 = list1;

    num = 5;
    for (auto val : list10) {
      CX_ASSERT(val == num, "");

      num += 5;
    }
    // CX_ASSERT(list10 == list1 , "");

    std::cout << "  Testing addition..." << std::endl;
    // Testing iterator functionality along with push_back
    auto it = list.begin();
    CX_ASSERT(*it == 1, "");

    ++it;
    CX_ASSERT(*it == 2, "");

    ++it;
    CX_ASSERT(it == list.end(), "");

    std::cout << "  Testing removal..." << std::endl;
    DoubleLinkedList<int> list2;

    list2.add(1);
    list2.add(2);
    list2.add(3);

    auto removedNode = list2.removeAt(1);
    CX_ASSERT(removedNode == 2, "");

    CX_ASSERT(list2.size() == 2, "");

    removedNode = list2.removeAt(0);
    CX_ASSERT(removedNode == 1, "");

    CX_ASSERT(list2.size() == 1, "");

    removedNode = list2.removeAt(0);
    CX_ASSERT(removedNode == 3, "");

    CX_ASSERT(list2.size() == 0, "");

    std::cout << "  Testing clear..." << std::endl;
    DoubleLinkedList<int> list3;

    list3.add(1);
    list3.add(2);
    list3.add(3);
    list3.clear();

    CX_ASSERT(list3.size() == 0, "");

    CX_ASSERT(list3.begin() == list3.end(), "");

    std::cout << "  Testing for memory leaks..." << std::endl;
    const int LARGE_NUMBER = 100;

    for (int k = 0; k < 100; k++) {
      DoubleLinkedList<int> list4;
      for (int i = 0; i < LARGE_NUMBER; i++) {
        list4.add(i);
      }
      CX_ASSERT(list4.size() == LARGE_NUMBER, "");
      for (int i = 0; i < LARGE_NUMBER; i++) {
        list4.removeAt(0);
      }
      CX_ASSERT(list4.size() == 0, "");
    }

    std::cout << "  Testing copy constructor and assignment operator..." << std::endl;
    DoubleLinkedList<int> list7;
    for (int i = 0; i < 5; i++) {
      list7.add(i);
    }
    DoubleLinkedList<int> list8(list7);   // Copy constructor
    DoubleLinkedList<int> list9 = list7;  // Assignment operator

    // Check n_elem
    CX_ASSERT(list7.size() == list8.size(), "");
    CX_ASSERT(list7.size() == list9.size(), "");

    // Check values and correct next and previous pointers
    auto it7 = list7.begin(), it8 = list8.begin(), it9 = list9.begin();
    while (it7 != list7.end()) {
      CX_ASSERT(*it7 == *it8 && *it7 == *it9, "");

      if (it7.current->next_) {
        CX_ASSERT(it8.current->next_->previous_ == it8.current, "");
        CX_ASSERT(it9.current->next_->previous_ == it9.current, "");
      }

      ++it7;
      ++it8;
      ++it9;
    }

    std::cout << "  Testing reverse iterator..." << std::endl;
    DoubleLinkedList<int> list11;
    for (int i = 0; i < 5; i++) {
      list11.add(i);
    }

    // Check values and correct next and previous pointers
    auto it10 = list11.begin_re(), it10end = list11.end_re();
    int expectedVal = 4;  // As we are going in reverse
    while (it10 != it10end) {
      CX_ASSERT(*it10 == expectedVal, "");

      if (it10.current->previous_) {
        CX_ASSERT(it10.current->previous_->next_ == it10.current, "");
      }

      --expectedVal;
      ++it10;
    }

    std::cout << "  Testing empty list..." << std::endl;
    DoubleLinkedList<int> list30;
    CX_ASSERT(list30.size() == 0, "");
    CX_ASSERT(list30.begin() == list30.end(), "");

    // Testing iteration over empty list
    for (auto val : list30) {
      // This block should never execute
      CX_ASSERT(false, "");
    }

    std::cout << "  Testing single element list..." << std::endl;
    DoubleLinkedList<int> list12;
    list12.add(1);
    CX_ASSERT(list12.size() == 1, "");

    // Testing iterator over single-element list
    auto it12 = list12.begin();
    CX_ASSERT(*it12 == 1, "");
    ++it12;
    CX_ASSERT(it12 == list12.end(), "");

    // Testing reverse iterator over single-element list
    auto it12re = list12.begin_re();
    CX_ASSERT(*it12re == 1, "");
    ++it12re;
    CX_ASSERT(it12re == list12.end_re(), "");

    std::cout << "  Testing removing from empty list..." << std::endl;
    DoubleLinkedList<int> list13;
    try {
      list13.removeAt(0);
      // This block should never execute
      CX_ASSERT(false, "");
    } catch (std::out_of_range& e) {
      // Expected behavior
    }

    std::cout << "  Testing removing from single element list..." << std::endl;
    DoubleLinkedList<int> list14;
    list14.add(1);
    int removedValue = list14.removeAt(0);
    CX_ASSERT(removedValue == 1, "");
    CX_ASSERT(list14.size() == 0, "");
    CX_ASSERT(list14.begin() == list14.end(), "");
  }
#  endif
};
}  // namespace cxstructs

#endif  //CXSTRUCTURES_DOUBLELINKEDLIST_H
