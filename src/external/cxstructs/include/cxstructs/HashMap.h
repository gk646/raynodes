// Copyright (c) 2023 gk646
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
#define CX_FINISHED
#ifndef CXSTRUCTS_HASHMAP_H
#  define CXSTRUCTS_HASHMAP_H

#  include <cstdint>
#  include <deque>
#  include <functional>
#  include <stdexcept>
#  include <string>
#  include "../cxalgos/MathFunctions.h"
#  include "../cxconfig.h"
#  include "Pair.h"

// HashMap implementation using constant stack arrays as buffer and linked lists
//already medium well optimized, should perform faster than the std:unordered_map in a lot of scenarios
//But is uses about 2-3 times as much memory!
// See Comparison.h

namespace cxhelper {  // namespace to hide the classes
/**
 * HashListNode used in the HashLinkedList
 * @tparam K - key type
 * @tparam V - value type
 */
template <typename K, typename V>
struct HashListNode {
  inline HashListNode(const K& key, const V& val) : key_(key), value_(val), next_(nullptr) {}
  inline explicit HashListNode(HashListNode<K, V>* o)
      : key_(o->key_), value_(o->value_), next_(nullptr){};
  inline HashListNode(HashListNode<K, V>* o, HashListNode<K, V>* next)
      : key_(o->key_), value_(o->value_), next_(next) {}
  K key_;
  V value_;
  HashListNode* next_;
};
/**
 * HashLinkedList used in the buckets of the HashMap
 *@tparam K - key type
 * @tparam V - value type
 */
template <typename K, typename V, uint_16_cx ArrayLength>
struct HashLinkedList {
  using HListNode = HashListNode<K, V>;

  Pair<K, V> data_[ArrayLength]{};

  HListNode* head_;
  HListNode* end_;

  inline HashLinkedList() : head_(nullptr), end_(nullptr){};
  inline HashLinkedList& operator=(const HashLinkedList& o) {
    if (this != &o) {
      std::copy(o.data_, o.data_ + ArrayLength, data_);

      if (o.head_) {
        head_ = new HashListNode<K, V>(o.head_, o.head_->next_);
        HashListNode<K, V>* current_new = head_;
        HashListNode<K, V>* current_old = o.head_->next_;

        while (current_old != nullptr) {
          current_new->next_ = new HashListNode<K, V>(current_old);

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
  inline ~HashLinkedList() {
    HListNode* current = head_;
    while (current != nullptr) {
      HListNode* next = current->next_;
      delete current;
      current = next;
    }
  }
  inline V& operator[](const K& key) {
    for (uint_16_cx i = 0; i < ArrayLength; i++) {
      if (data_[i].assigned() && data_[i].first() == key) {
        return data_[i].second();
      }
    }

    HListNode* current = head_;
    while (current != nullptr) {
      if (current->key_ == key) {
        return current->value_;
      }
      current = current->next_;
    }
    return data_[0].second();
  }
  inline V& at(const K& key) {
    for (uint_16_cx i = 0; i < ArrayLength; i++) {
      if (data_[i].assigned() && data_[i].first() == key) {
        return data_[i].second();
      }
    }

    HListNode* current = head_;
    while (current != nullptr) {
      if (current->key_ == key) {
        return current->value_;
      }
      current = current->next_;
    }
    throw std::out_of_range("no such key");
  }
  inline bool replaceAdd(const K& key, const V& val) {
    for (int i = 0; i < ArrayLength; i++) {
      if (data_[i].assigned() && data_[i].first() == key) {
        data_[i].second() = val;
        return false;
      } else if (!data_[i].assigned()) {
        data_[i].assigned() = true;
        data_[i].first() = key;
        data_[i].second() = val;
        return true;
      }
    }

    HListNode* current = head_;
    while (current != nullptr) {
      if (current->key_ == key) {
        current->value_ = val;
        return false;
      }
      current = current->next_;
    }

    if (head_ == nullptr) {
      head_ = new HListNode(key, val);
      end_ = head_;
    } else {
      end_->next_ = new HListNode(key, val);
      end_ = end_->next_;
    }
    return true;
  }
  inline bool remove(const K& key) {
    for (uint_16_cx i = 0; i < ArrayLength; i++) {
      if (data_[i].assigned() && data_[i].first() == key) {
        data_[i].assigned() = false;
        return true;
      }
    }

    if (head_) {
      if (head_->key_ == key) {
        HListNode* toDelete = head_;
        head_ = head_->next_;
        if (!head_) {
          end_ = nullptr;
        }
        delete toDelete;
        return true;
      } else {
        HListNode* current = head_;
        while (current->next_ && current->next_->key_ != key) {
          current = current->next_;
        }

        if (current->next_) {
          HListNode* toDelete = current->next_;
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
  inline bool contains(const K& key) {
    for (uint_fast32_t i = 0; i < ArrayLength; i++) {
      if (data_[i].assigned() && data_[i].first() == key) {
        return true;
      }
    }
    HListNode* it = head_;
    while (it) {
      if (it->key_ == key) {
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
 * <h2>HashMap</h2>
 * This data structure is an efficient key-value store, typically providing lookups in constant time (O(1)).
 * <br><br>
 * <b>Important:</b> For non-primitive and non-string types, a custom hash function is recommended.
 * <br><br>
 * A HashMap is notably beneficial due to its speed and ease of use. Hashing keys to numerical indices allows for quick value retrieval from the underlying array.
 * <br><br>
 * A hash function can sometimes create a similar output for the same input which is a collision. One way to combat this is called separate chaining.
 * Separate chaining is a technique to handle hash collisions by using LinkedLists as buckets and simply appending the element to the end of this list.
 * This means that each bucket-array index hosts a linked list that is traversed to locate the correct key as the key is still unique (same keys are replaced).
 */

template <typename K, typename V, typename Hash = std::function<size_t(const K&)>>
class HashMap {
  constexpr static uint_16_cx BufferLen = 1;
  using HList = HashLinkedList<K, V, BufferLen>;

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
    buckets_ <<= 1;
    auto* newArr = new HList[buckets_];

    for (int i = 0; i < oldBuckets; i++) {
      const auto data = arr_[i].data_;
      for (uint_fast32_t j = 0; j < BufferLen; j++) {
        if (data[j].assigned()) {
          size_t hash = hash_func_(data[j].first()) & (buckets_ - 1);
          newArr[hash].replaceAdd(data[j].first(), data[j].second());
        }
      }
      HashListNode<K, V>* current = arr_[i].head_;
      while (current) {
        size_t hash = hash_func_(current->key_) & (buckets_ - 1);
        newArr[hash].replaceAdd(current->key_, current->value_);
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
        size_t hash = hash_func_(arr_[i].data_[j].first()) & (buckets_ - 1);
        newArr[hash].replaceAdd(arr_[i].data_[j].first(), arr_[i].data_[j].second());
      }
      HashListNode<K, V>* current = arr_[i].head_;
      while (current) {
        size_t hash = hash_func_(current->key_) & (buckets_ - 1);
        newArr[hash].replaceAdd(current->key_, current->value_);
        current = current->next_;
      }
    }
    delete[] arr_;
    arr_ = newArr;
    maxSize = buckets_ * load_factor_;
  }

 public:
  explicit HashMap(uint_32_cx initialCapacity = 64, float loadFactor = 0.9)
      : buckets_(next_power_of_2(initialCapacity)),
        initialCapacity_(next_power_of_2(initialCapacity)), size_(0),
        arr_(new HList[next_power_of_2(initialCapacity)]),
        maxSize(next_power_of_2(initialCapacity) * loadFactor), hash_func_(std::hash<K>()),
        load_factor_(loadFactor) {}
  /**
   * This constructor allows the user to supply their own hash function for the key type
   * @tparam HashFunction callable that takes a key with type V and returns int
   * @param hash_function  this function is called with any given key with type V
   * @param initialCapacity the initial size of the container and the growth size
   */

  explicit HashMap(Hash hash_function, uint_32_cx initialCapacity = 64, float loadFactor = 0.9)
      : buckets_(next_power_of_2(initialCapacity)),
        initialCapacity_(next_power_of_2(initialCapacity)), size_(0),
        arr_(new HList[next_power_of_2(initialCapacity)]),
        maxSize(next_power_of_2(initialCapacity) * loadFactor), hash_func_(hash_function),
        load_factor_(loadFactor) {}
  HashMap(const HashMap<K, V>& o)
      : initialCapacity_(o.initialCapacity_), size_(o.size_), buckets_(o.buckets_),
        hash_func_(o.hash_func_), maxSize(o.maxSize), load_factor_(o.load_factor_) {
    arr_ = new HList[buckets_];
    for (uint_32_cx i = 0; i < buckets_; i++) {
      arr_[i] = o.arr_[i];
    }
  }
  HashMap(HashMap<K, V>&& o) noexcept
      : initialCapacity_(o.initialCapacity_), size_(o.size_), buckets_(o.buckets_),
        hash_func_(std::move(o.hash_func_)), maxSize(o.maxSize), load_factor_(o.load_factor_),
        arr_(o.arr_) {
    o.arr_ = nullptr;
    o.size_ = 0;
  }
  HashMap& operator=(const HashMap<K, V>& o) {
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
  HashMap& operator=(HashMap<K, V>&& o) noexcept {
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
  ~HashMap() { delete[] arr_; };
  /**
   * Retrieves the value for the given key<p>
   * If the key doesnt exist will return a dummy value
   * @param key - the key to the value
   * @return the value at this key
   */
  inline V& operator[](const K& key) const { return arr_[hash_func_(key) & (buckets_ - 1)][key]; }
  /**
   * Inserts a key, value Pair into the map
   * @param key - the key to access the stored element
   * @param val - the stored value at the given key
   */
  inline void insert(const K& key, const V& val) {
    if (size_ > maxSize) {
      reHashBig();
    }
    size_ += arr_[hash_func_(key) & (buckets_ - 1)].replaceAdd(key, val);
  }
  /**
   * Retrieves the value for the given key <p>
   * <b>Throws an std::out_of_range if the key doesnt exist</b>
   * @param key - the key to the value
   * @return the value at this key
   */
  [[nodiscard]] inline V& at(const K& key) const {
    return arr_[hash_func_(key) & (buckets_ - 1)].at(key);
  }
  /**
   * Removes this key, value Pair from the hashmap
   * @param key - they key to be removed
   */
  inline void erase(const K& key) {
    size_t hash = hash_func_(key) & (buckets_ - 1);
    size_ -= arr_[hash].remove(key);
    CX_ASSERT(size_ >= 0, "no such element to erase");
  }
  /**
   *
   * @return the current n_elem of the hashMap
   */
  [[nodiscard]] inline uint_32_cx size() const { return size_; }
  /**
   * The initialCapacity the hashmaps started with and expands along
   * @return the initial capacity
   */
  [[nodiscard]] inline uint_32_cx capacity() const { return buckets_; }
  /**
   * Clears the hashMap of all its contents
   */
  inline void clear() {
    delete[] arr_;
    arr_ = new HList[initialCapacity_];
    buckets_ = initialCapacity_;
    size_ = 0;
    maxSize = buckets_ * load_factor_;
  }
  /**
 * @brief Checks if the HashMap contained a specific key.
 *
 * @param key The key to search for in the HashMap.
 * @return true if the key is present in the HashMap, false otherwise.
 */
  inline bool contains(const K& key) { return arr_[hash_func_(key) % buckets_].contains(key); }
  /**
   * Reduces the underlying array size to something close to the actual data size.
   * This decreases memory usage.
   */
  inline void shrink_to_fit() {
    if (buckets_ > size_ * 1.5) {
      reHashSmall();
    }
  }
  class ValueIterator {
    std::deque<V> values_;
    explicit ValueIterator(HList* h_list, uint_32_cx size)
        : values_(){

          };
  };
#  ifdef CX_INCLUDE_TESTS
  static void TEST() {
    std::cout << "HASHMAP TESTS" << std::endl;
    // Test insert and operator[key]
    std::cout << "  Testing insertion and operator[key]..." << std::endl;
    HashMap<int, std::string> map1;
    map1.insert(1, "One");
    map1.insert(2, "Two");
    CX_ASSERT(map1[1] == "One", "");
    CX_ASSERT(map1[2] == "Two", "");

    // Test replace value
    std::cout << "  Testing replacement of values..." << std::endl;
    map1.insert(1, "One_Updated");
    CX_ASSERT(map1[1] == "One_Updated", "");

    // Test erase
    std::cout << "  Testing erase method..." << std::endl;
    map1.erase(1);
    try {
      std::string nodiscard = map1.at(1);
      CX_ASSERT(false, "");
    } catch (const std::exception& e) {
      CX_ASSERT(true, "");
    }

    // Test copy constructor
    std::cout << "  Testing copy constructor..." << std::endl;
    HashMap<int, std::string> map2(map1);
    CX_ASSERT(map2[2] == "Two", "");

    // Test assignment operator
    std::cout << "  Testing assignment operator..." << std::endl;
    HashMap<int, std::string> map3;
    map3 = map1;
    CX_ASSERT(map3[2] == "Two", "");
    // Test n_elem
    std::cout << "  Testing size()..." << std::endl;
    CX_ASSERT(map1.size() == 1, "");
    CX_ASSERT(map2.size() == 1, "");
    CX_ASSERT(map3.size() == 1, "");

    // Test at
    std::cout << "  Testing at method..." << std::endl;
    CX_ASSERT(map1.at(2) == "Two", "");
    // Test clear
    std::cout << "  Testing clear method..." << std::endl;
    map1.clear();
    CX_ASSERT(map1.size() == 0, "");

    // Test large additions
    std::cout << "  Testing large additions..." << std::endl;
    HashMap<int, double> map4;
    for (int i = 0; i < 10000; i++) {
      map4.insert(i, i * 2);
    }
    for (int i = 0; i < 10000; i++) {
      CX_ASSERT(map4[i] == i * 2, "");
    }
    std::cout << "  Testing contained method..." << std::endl;
    HashMap<int, std::string> map5;
    map5.insert(1, "One");
    map5.insert(2, "Two");
    CX_ASSERT(map5.contains(1), "");
    CX_ASSERT(map5.contains(2), "");
    CX_ASSERT(!map5.contains(3), "");

    // Test shrink_to_fit
    std::cout << "  Testing shrink_to_fit method..." << std::endl;
    map5.shrink_to_fit();
    CX_ASSERT(map5.capacity() == map5.size() * 1.5, "");
    HashMap<int, int> map6;
    for (int i = 0; i < 10000; i++) {
      map6.insert(i, i * 2);
    }
    for (int i = 0; i < 10000; i++) {
      CX_ASSERT(map6[i] == i * 2, "");
    }
    map6.shrink_to_fit();
    CX_ASSERT(map6.capacity() == map6.size() * 1.5, "");

    // Test move constructor
    std::cout << "  Testing move constructor..." << std::endl;
    HashMap<int, std::string> map7(std::move(map3));
    CX_ASSERT(map7[2] == "Two", "");
    CX_ASSERT(map3.size() == 0, "");

    // Test move assignment operator
    std::cout << "  Testing move assignment operator..." << std::endl;
    HashMap<int, std::string> map8;
    map8 = std::move(map2);
    CX_ASSERT(map8[2] == "Two", "");
    CX_ASSERT(map2.size() == 0, "");

    // Test destructor
    std::cout << "  Testing destructor indirectly..." << std::endl;
    {
      HashMap<int, std::string> map9;
      map9.insert(1, "One");
    }  // map9 goes out of scope here, its destructor should be called.

    // Stress test
    std::cout << "  Stress testing..." << std::endl;
    HashMap<int, int> map10;
    for (int i = 0; i < 1000000; i++) {
      map10.insert(i, i);
    }
    for (int i = 0; i < 1000000; i += 2) {
      map10.erase(i);
    }
    for (int i = 1; i < 1000000; i += 2) {
      CX_ASSERT(map10[i] == i, "");
    }
    HashMap<int, std::string> map11;
    // Test at with non-existent key
    std::cout << "  Testing at with non-existent key..." << std::endl;
    try {
      std::string nodiscard = map11.at(3);
      CX_ASSERT(false, "");
    } catch (const std::exception& e) {
      CX_ASSERT(true, "");
    }
  }
#  endif
};
}  // namespace cxstructs
#endif  // CXSTRUCTS_HASHMAP_H
