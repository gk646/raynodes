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
#ifndef CXSTRUCTS_SRC_DATASTRUCTURES_HASHSET_H_
#  define CXSTRUCTS_SRC_DATASTRUCTURES_HASHSET_H_

#  include <cstdint>
#  include <deque>
#  include <functional>
#  include <stdexcept>
#  include <string>
#  include "../cxalgos/MathFunctions.h"
#  include "../cxconfig.h"
#  include "row.h"

namespace cxhelper {  // namespace to hide the classes
/**
 * HashSetListNode used in the HashSetLinkedList
 * @tparam V - value type
 */
template <typename V>
struct HashSetListNode {
  inline explicit HashSetListNode(const V& key) : value_(key), next_(nullptr) {}
  inline HashSetListNode(const V& val, HashSetListNode<V>* next) : value_(val), next_(next) {}
  V value_;
  HashSetListNode* next_;
};
template <typename V>
struct ValueContainer {
  V value_;
  bool assigned_;
};
/**
 * HashSetLinkedList used in the buckets of the HashSet
 * @tparam V - value type
 */
template <typename V, uint_16_cx ArrayLength>
struct HashSetLinkedList {
  using HSListNode = HashSetListNode<V>;

  ValueContainer<V> data_[ArrayLength]{};

  HSListNode* head_;
  HSListNode* end_;

  inline HashSetLinkedList() : head_(nullptr), end_(nullptr){};
  inline HashSetLinkedList& operator=(const HashSetLinkedList& o) {
    if (this != &o) {
      std::copy(o.data_, o.data_ + ArrayLength, data_);

      if (o.head_) {
        head_ = new HSListNode(o.head_->value_, o.head_->next_);
        HSListNode* current_new = head_;
        HSListNode* current_old = o.head_->next_;

        while (current_old != nullptr) {
          current_new->next_ = new HSListNode(current_old->value_);

          current_new = current_new->next_;
          current_old = current_old->next_;
        }
        end_ = current_new;
      } else {
        head_ = end_ = nullptr;
      }
    }
    return *this;
  }
  inline ~HashSetLinkedList() {
    HSListNode* current = head_;
    while (current != nullptr) {
      HSListNode* next = current->next_;
      delete current;
      current = next;
    }
  }
  inline bool replaceAdd(const V& value) {
    for (uint_16_cx i = 0; i < ArrayLength; i++) {
      if (data_[i].assigned_ && data_[i].value_ == value) {
        return false;
      } else if (!data_[i].assigned_) {
        data_[i].assigned_ = true;
        data_[i].value_ = value;
        return true;
      }
    }

    HSListNode* current = head_;
    while (current) {
      if (current->value_ == value) {
        return false;
      }
      current = current->next_;
    }

    if (head_ == nullptr) {
      head_ = new HSListNode(value);
      end_ = head_;
    } else {
      end_->next_ = new HSListNode(value);
      end_ = end_->next_;
    }
    return true;
  }
  inline bool remove(const V& value) {
    for (uint_16_cx i = 0; i < ArrayLength; i++) {
      if (data_[i].assigned_ && data_[i].value_ == value) {
        data_[i].assigned_ = false;
        return true;
      }
    }

    if (head_) {
      if (head_->value_ == value) {

        HSListNode* toDelete = head_;
        head_ = head_->next_;
        if (!head_) {
          end_ = nullptr;
        }
        delete toDelete;
        return true;
      } else {
        HSListNode* current = head_;
        while (current->next_ && current->next_->value_ != value) {
          current = current->next_;
        }

        if (current->next_) {
          HSListNode* toDelete = current->next_;
          current->next_ = toDelete->next_;
          if (toDelete == end_) {
            end_ = current;
          }
          delete toDelete;
          return true;
        }
      }
    }
    return false;
  }
  inline bool contains(const V& key) {
    for (uint_fast32_t i = 0; i < ArrayLength; i++) {
      if (data_[i].assigned_ && data_[i].value_ == key) {
        return true;
      }
    }
    HSListNode* it = head_;
    while (it) {
      if (it->value_ == key) {
        return true;
      }
      it = it->next_;
    }
    return false;
  }
};
}  // namespace cxhelper

namespace cxstructs {
using namespace cxhelper;
/**
 * <h2>HashSet</h2>
 * is a basically a HashMap but with only the key stored. This means the only information stored is the existence of a key to a given hash value.
 * Inserting a element with an identical key will override the existing one, which is why its called a set.<p>
 * A set only has unique values and the HashSet allows to look for the existence of a key to the hash value <b>in O(1) time</b>.
 *This makes it very useful when your not required to store information besides the simple existence
 * nce of something, for example the state of visited nodes in a path finding algorithm.
 *
 * @tparam V The type of the values to be stored.
 * @tparam Hash The hash function to be used.
 */
template <typename V, typename Hash = std::function<size_t(const V&)>>
class HashSet {

  constexpr inline static uint_16_cx BufferLen = 2;
  using HList = cxhelper::HashSetLinkedList<V, BufferLen>;
  uint_32_cx initialCapacity_;
  uint_32_cx size_;
  uint_32_cx buckets_;
  uint_32_cx maxSize;
  float load_factor_;

  HList* arr_;
  Hash hash_func_;

  inline void reHashBig() {
    // once the n_elem limit is reached all values needs to be rehashed to fit to the keys with the new bucket n_elem
    auto oldBuckets = buckets_;
    buckets_ = buckets_ * 2;
    auto* newArr = new HList[buckets_];

#  pragma omp simd  //single instruction multiple d?
    for (int i = 0; i < oldBuckets; i++) {
      const auto& data = arr_[i].data_;
      for (uint_fast32_t j = 0; j < BufferLen; j++) {
        if (data[j].assigned_) {
          size_t hash = hash_func_(data[j].value_) & (buckets_ - 1);
          newArr[hash].replaceAdd(data[j].value_);
        }
      }
      HashSetListNode<V>* current = arr_[i].head_;
      while (current) {
        size_t hash = hash_func_(current->value_) & (buckets_ - 1);
        newArr[hash].replaceAdd(current->value_);
        current = current->next_;
      }
    }
    delete[] arr_;
    arr_ = newArr;
    maxSize = buckets_ * load_factor_;
  }
  inline void reHashSmall() {
    //only used in shrink_to_fit()
    auto oldBuckets = buckets_;
    buckets_ = size_ * 1.5;
    auto* newArr = new HList[buckets_];

    for (int i = 0; i < oldBuckets; i++) {
      for (uint_fast32_t j = 0; j < BufferLen; j++) {
        size_t hash = hash_func_(arr_[i].data_[j].first()) % buckets_;
        newArr[hash].replaceAdd(arr_[i].data_[j].first(), arr_[i].data_[j].second());
      }
      HList* current = arr_[i].head_;
      while (current) {
        size_t hash = hash_func_(current->key_) % buckets_;
        newArr[hash].replaceAdd(current->key_, current->value_);
        current = current->next_;
      }
    }
    delete[] arr_;
    arr_ = newArr;
    maxSize = buckets_ * load_factor_;
  }

 public:
  explicit HashSet(uint_32_cx initialCapacity = 64, float loadFactor = 0.9)
      : buckets_(next_power_of_2(initialCapacity)),
        initialCapacity_(next_power_of_2(initialCapacity)), size_(0),
        arr_(new HList[next_power_of_2(initialCapacity)]),
        maxSize(next_power_of_2(initialCapacity) * loadFactor), hash_func_(std::hash<V>()),
        load_factor_(loadFactor) {}
  /**
   * This constructor allows the user to supply their own hash function for the key type
   * @tparam HashFunction callable that takes a key with type V and returns int
   * @param hash_function  this function is called with any given key with type V
   * @param initialCapacity the initial size of the container and the growth size
   */

  explicit HashSet(Hash hash_function, uint_32_cx initialCapacity = 32, float loadFactor = 0.9)
      : buckets_(next_power_of_2(initialCapacity)),
        initialCapacity_(next_power_of_2(initialCapacity)), size_(0),
        arr_(new HList[next_power_of_2(initialCapacity)]),
        maxSize(next_power_of_2(initialCapacity) * loadFactor), hash_func_(hash_function),
        load_factor_(loadFactor) {}
  HashSet(const HashSet& o)
      : initialCapacity_(o.initialCapacity_), size_(o.size_), buckets_(o.buckets_),
        hash_func_(o.hash_func_), maxSize(o.maxSize), load_factor_(o.load_factor_) {
    arr_ = new HList[buckets_];
    for (uint_32_cx i = 0; i < buckets_; i++) {
      arr_[i] = o.arr_[i];
    }
  }
  HashSet(HashSet&& o) noexcept
      : initialCapacity_(o.initialCapacity_), size_(o.size_), buckets_(o.buckets_),
        hash_func_(std::move(o.hash_func_)), maxSize(o.maxSize), load_factor_(o.load_factor_),
        arr_(o.arr_) {
    o.arr_ = nullptr;
    o.size_ = 0;
  }
  HashSet& operator=(const HashSet& o) {
    if (this != &o) {
      delete[] arr_;

      initialCapacity_ = o.initialCapacity_;
      size_ = o.size_;
      buckets_ = o.buckets_;
      hash_func_ = o.hash_func_;
      maxSize = o.maxSize;

      arr_ = new HList[buckets_];
      for (uint_32_cx i = 0; i < buckets_; ++i) {
        arr_[i] = o.arr_[i];
      }
    }
    return *this;
  }
  HashSet& operator=(HashSet&& o) noexcept {
    if (this != &o) {
      delete[] arr_;

      initialCapacity_ = o.initialCapacity_;
      size_ = o.size_;
      buckets_ = o.buckets_;
      hash_func_ = std::move(o.hash_func_);
      maxSize = o.maxSize;
      arr_ = o.arr_;

      o.arr_ = nullptr;
      o.size_ = 0;
    }
    return *this;
  }
  ~HashSet() { delete[] arr_; };
  /**
   * Inserts a key, value Pair into the map
   * @param key - the key to access the stored element
   * @param val - the stored value at the given key
   */
  inline void insert(const V& val) {
    if (size_ > maxSize) {
      reHashBig();
    }
    size_ += arr_[hash_func_(val) & (buckets_ - 1)].replaceAdd(val);
  }
  /**
   * Removes this val, value Pair from the HashSet
   * @param val - they val to be removed
   */
  inline void erase(const V& val) {
    size_ -= arr_[hash_func_(val) & (buckets_ - 1)].remove(val);
    CX_ASSERT(size_ >= 0, "no such element to erase");
  }
  /**
   *
   * @return the current n_elem of the HashSet
   */
  [[nodiscard]] inline uint_32_cx size() const { return size_; }
  /**
   * The initialCapacity the HashSets started with and expands along
   * @return the initial capacity
   */
  [[nodiscard]] inline uint_32_cx capacity() const { return buckets_; }
  /**
   * Clears the HashSet of all its contents
   */
  inline void clear() {
    delete[] arr_;
    arr_ = new HList[buckets_];
    size_ = 0;
    maxSize = buckets_ * load_factor_;
  }
  /**
 * @brief Checks if the HashSet contained a specific key.
 *
 * @param key The key to search for in the HashSet.
 * @return true if the key is present in the HashSet, false otherwise.
 */
  inline bool contains(const V& key) { return arr_[hash_func_(key) % buckets_].contains(key); }
  /**
   * Reduces the underlying array size to something close to the actual data size.
   * This decreases memory usage.
   */
  inline void shrink_to_fit() {
    if (buckets_ > size_ * 1.5) {
      reHashSmall();
    }
  }
#  ifdef CX_INCLUDE_TESTS
  static void TEST() {
    std::cout << "HASHSET TESTS" << std::endl;

    // Test insert
    std::cout << "  Testing insertion..." << std::endl;
    HashSet<int> set1;
    set1.insert(1);
    set1.insert(2);
    CX_ASSERT(set1.contains(1), "");
    CX_ASSERT(set1.contains(2), "");

    // Test erase
    std::cout << "  Testing erase method..." << std::endl;
    set1.erase(1);
    CX_ASSERT(!set1.contains(1), "");

    // Test copy constructor
    std::cout << "  Testing copy constructor..." << std::endl;
    HashSet<int> set2(set1);
    CX_ASSERT(set2.contains(2), "");

    // Test assignment operator
    std::cout << "  Testing assignment operator..." << std::endl;
    HashSet<int> set3;
    set3 = set1;
    CX_ASSERT(set3.contains(2), "");

    // Test size
    std::cout << "  Testing size()..." << std::endl;
    CX_ASSERT(set1.size() == 1, "");
    CX_ASSERT(set2.size() == 1, "");
    CX_ASSERT(set3.size() == 1, "");

    // Test clear
    std::cout << "  Testing clear method..." << std::endl;
    set1.clear();
    CX_ASSERT(set1.size() == 0, "");

    // Test large additions
    std::cout << "  Testing large additions..." << std::endl;
    HashSet<int> set4;
    for (int i = 0; i < 10000; i++) {
      set4.insert(i);
    }
    for (int i = 0; i < 10000; i++) {
      CX_ASSERT(set4.contains(i), "");
    }

    // Test move constructor
    std::cout << "  Testing move constructor..." << std::endl;
    HashSet<int> set5(std::move(set3));
    CX_ASSERT(set5.contains(2), "");
    CX_ASSERT(set3.size() == 0, "");

    // Test move assignment operator
    std::cout << "  Testing move assignment operator..." << std::endl;
    HashSet<int> set6;
    set6 = std::move(set2);
    CX_ASSERT(set6.contains(2), "");
    CX_ASSERT(set2.size() == 0, "");

    // Test destructor
    std::cout << "  Testing destructor indirectly..." << std::endl;
    {
      HashSet<int> set7;
      set7.insert(1);
    }  // set7 goes out of scope here, its destructor should be called.

    // Stress test
    std::cout << "  Stress testing..." << std::endl;
    HashSet<int> set8;
    for (int i = 0; i < 100000; i++) {
      set8.insert(i);
    }
    for (int i = 0; i < 100000; i += 2) {
      set8.erase(i);
    }
    for (int i = 1; i < 100000; i += 2) {
      CX_ASSERT(set8.contains(i), "");
    }
  };
#  endif
};
}  // namespace cxstructs
#endif  //CXSTRUCTS_SRC_DATASTRUCTURES_HASHSET_H_
