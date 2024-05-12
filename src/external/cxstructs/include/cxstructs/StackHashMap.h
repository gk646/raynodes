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
#ifndef CXSTRUCTS_SRC_CXSTRUCTS_STACKHASHMAP_H_
#  define CXSTRUCTS_SRC_CXSTRUCTS_STACKHASHMAP_H_

#  include "../cxconfig.h"
#  include <bitset>  //For std::bitset<N> and std::hash<K>
#  include <initializer_list>

//Memory footprint is still quite big
//Using std::bitset<> saves memory but is a bit slower
//Use set_rand(1) if you have your own perfect hash function -> will be overridden on collision
namespace cxstructs {

/**
 * StackHashMap is a hash map implemented entirely on the stack with an STL-like interface.<br>
 * It is designed for very fast lookup operations with a fixed size, making it ideal as a high-performance,<br>
 * fixed-capacity lookup table. However, it is slower during build-up, and maintaining a load factor<br>
 * below 90% is recommended to ensure no endless loops.<br>
 *
 * Due to its rehashing strategy to resolve collisions—multiplying hash values by a random number<br>
 * until a perfect hash is found—insert operations can potentially enter an endless loop if a perfect<br>
 * hash for all elements cannot be determined.<br>
 *
 * Performance Characteristics:
 * - Build-up is generally slower compared to other hash maps at equal load factors.<br>
 * - Lookup operations are approximately 2-3 times as fast as those in std::unordered_map.<br>
 * - Erase operations are significantly faster due to the nature of the implementation.<br>
 *
 * @tparam K Type of the keys.
 * @tparam V Type of the values.
 * @tparam N Size of the hash map. Must be a compile-time constant.
 * @tparam HashFunc Hash function to be used. Defaults to std::hash<K>.
 * @tparam size_type Type for representing sizes. Defaults to uint32_t.
 *
 * Note: StackHashMap currently only supports trivial types for both keys and values.
 */
template <typename K, typename V, size_t N, typename HashFunc = std::hash<K>,
          typename size_type = uint32_t>
class StackHashMap {
  using const_key_ref = const K&;
  using const_val_ref = const V&;
  using move_key_ref = K&&;
  using move_val_ref = V&&;
  using key_ref = K&;
  using val_ref = V&;

  struct Node {
    K key;
    V val;
  };

  HashFunc hash_func_;
  Node data_[N];
  bool register_[N] = {0};
  size_type size_ = 0;
  int rand_ = rand();

  [[nodiscard]] constexpr size_t impl_hash_func(const K& key) const noexcept {
    if constexpr ((N & (N - 1)) == 0) {  //Power of 2 optimization
      return hash_func_(key) & (N - 1);
    }
    return (hash_func_(key) * rand_) % N;
  }

  void add_node(size_t hash, const_key_ref key, auto val) noexcept {
    if (!register_[hash]) {
      if constexpr (std::is_trivially_copyable_v<K> && std::is_trivially_copyable_v<V>) {
        data_[hash].key = key;
        data_[hash].val = val;
      } else {
        new (&data_[hash].key) K(key);
        new (&data_[hash].val) V(val);
      }
      register_[hash] = true;
      size_++;
    } else {
      data_[hash].val = val;
    }
  }

  void insert_non_empty(const K& key, auto val) noexcept {
    Node org_data[N];
    bool org_register[N];
    std::memcpy(org_data, data_, N * sizeof(Node));
    std::memcpy(org_register, register_, N);

    while (true) {
      std::memset(register_, false, N);
      rand_ = rand();

      bool unique_mapping = true;
      for (int i = 0; i < N; i++) {
        if (!org_register[i]) continue;
        const size_t hash = impl_hash_func(org_data[i].key);
        if (register_[hash]) {  //New-found hash is not unique
          unique_mapping = false;
          break;
        }
        data_[hash].key = org_data[i].key;
        data_[hash].val = org_data[i].val;
        register_[hash] = true;
      }

      if (!unique_mapping) continue;

      const auto new_hash = impl_hash_func(key);
      if (register_[new_hash]) continue;

      // Check integrity
      bool integrity_check = true;

      for (int i = 0; i < N; i++) {
        if (!org_register[i]) continue;
        bool found_old_key = false;
        const auto& old_key = org_data[i].key;
        for (int j = 0; j < N; j++) {
          if (data_[j].key == old_key) {
            found_old_key = true;
            break;
          }
        }
        if (!found_old_key) {
          integrity_check = false;
          break;
        }
      }

      if (!integrity_check) continue;

      add_node(new_hash, key, val);
      break;
    }
  }

 public:
  StackHashMap() = default;

  StackHashMap(std::initializer_list<std::pair<K, V>> list) {
    for (const auto& elem : list) {
      insert(elem.first, elem.second);
    }
  }

  StackHashMap(const StackHashMap& other) {
    if constexpr (std::is_trivial_v<K> && std::is_trivial_v<V>) {
      std::memcpy(data_, other.data_, N * sizeof(Node));
    } else {
      for (size_t i = 0; i < N; ++i) {
        if (other.register_[i]) {
          new (&data_[i].key) K(other.data_[i].key);
          new (&data_[i].val) V(other.data_[i].val);
        }
      }
    }
    std::memcpy(register_, other.register_, N);
    size_ = other.size_;
    rand_ = other.rand_;
  }

  explicit StackHashMap(size_type elems) : size_(elems) {}

  ~StackHashMap() {
    if constexpr (!std::is_trivially_destructible_v<K> || !std::is_trivially_destructible_v<V>) {
      for (size_t i = 0; i < N; ++i) {
        if (register_[i]) {
          data_[i].key.~K();
          data_[i].val.~V();
        }
      }
    }
  }

  //Call can result in endless loop at high load factor
  void insert(const K& key, V&& val) noexcept {
    CX_ASSERT(size_ < N, "Trying to add to full StackHashMap");
    CX_STACK_ABORT_IMPL();

    const size_t hash = impl_hash_func(key);

    if (register_[hash] && data_[hash].key != key) {
      return insert_non_empty(key, val);
    }

    add_node(hash, key, val);
  }

  //Call can result in endless loop at high load factor
  void insert(const K& key, const V& val) noexcept {
    CX_ASSERT(size_ < N, "Trying to add to full StackHashMap");
    CX_STACK_ABORT_IMPL();

    const size_t hash = impl_hash_func(key);

    if (register_[hash] && data_[hash].key != key) {
      return insert_non_empty(key, val);
    }

    add_node(hash, key, val);
  }

  void insert(const std::pair<const K, V>& keyValue) noexcept {
    CX_ASSERT(size_ < N, "Trying to add to full StackHashMap");
    CX_STACK_ABORT_IMPL();

    const K& key = keyValue.first;
    const V& val = keyValue.second;

    const size_t hash = impl_hash_func(key);

    if (register_[hash] && data_[hash].key != key) {
      return insert_non_empty(key, val);
    }

    add_node(hash, key, val);
  }

  auto operator[](const K& key) noexcept -> V& {
    const size_t hash = impl_hash_func(key);

    if (!register_[hash]) {
      add_node(hash, key, V());
      return data_[hash].val;
    }

    if (data_[hash].key == key) {
      return data_[hash].val;
    }

    insert(key, V());
    return data_[impl_hash_func(key)].val;
  }

  [[nodiscard]] auto size() const noexcept -> size_type { return size_; }

  auto erase(const K& key) noexcept -> bool {
    const size_t hash = impl_hash_func(key);
    if (register_[hash]) {
      if constexpr (!std::is_trivial_v<K>) {
        data_[hash].key.~K();
      }
      if constexpr (!std::is_trivial_v<V>) {
        data_[hash].val.~V();
      }
      register_[hash] = false;
      size_--;
      return true;
    }
    return false;
  }

  [[nodiscard]] auto contains(const K& key) const noexcept -> bool {
    const size_t hash = impl_hash_func(key);
    return register_[hash] && data_[hash].key == key;
  }

  //The multiplier for the hash | Set to 1 if you supply a perfect hash function | Resets on collision
  void set_rand(int rand) noexcept { rand_ = rand; }

  void clear() noexcept {
    std::memset(register_, false, N);
    size_ = 0;
  }

  [[nodiscard]] auto empty() const noexcept -> bool { return size_ == 0; }

  [[nodiscard]] auto load_factor() const noexcept -> float { return (float)size_ / (float)N; }

  [[nodiscard]] auto get_hash(const K& key) const noexcept -> size_t { return impl_hash_func(key); }

  // Iterator support
  class KeyIterator {
   public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = K;
    using difference_type = std::ptrdiff_t;
    using pointer = K*;
    using reference = K&;

    explicit KeyIterator(Node* ptr, bool* reg, size_t size)
        : ptr_(ptr), reg_(reg), size_(size), index_(0) {
      if (!reg_[index_]) {
        ++(*this);
      }
    }

    reference operator*() const { return ptr_[index_].key; }

    pointer operator->() { return &(ptr_[index_].key); }

    // Prefix increment
    KeyIterator& operator++() {
      do {
        index_++;
      } while (index_ < size_ && !reg_[index_]);
      return *this;
    }

    // Postfix increment
    KeyIterator operator++(int) {
      KeyIterator tmp = *this;
      ++(*this);
      return tmp;
    }

    friend bool operator==(const KeyIterator& a, const KeyIterator& b) {
      return a.ptr_ + a.index_ == b.ptr_ + b.index_;
    }

    friend bool operator!=(const KeyIterator& a, const KeyIterator& b) { return !(a == b); }

   private:
    Node* ptr_;
    bool* reg_;
    size_t size_;
    size_t index_;
  };

  class ValueIterator {
   public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = V;
    using difference_type = std::ptrdiff_t;
    using pointer = V*;
    using reference = V&;

    explicit ValueIterator(Node* ptr, bool* reg, size_t size)
        : ptr_(ptr), reg_(reg), size_(size), index_(0) {
      if (!reg_[index_]) {
        ++(*this);
      }
    }

    reference operator*() const { return ptr_[index_].val; }

    pointer operator->() { return &(ptr_[index_].val); }

    // Prefix increment
    ValueIterator& operator++() {
      do {
        index_++;
      } while (index_ < size_ && !reg_[index_]);
      return *this;
    }

    // Postfix increment
    ValueIterator operator++(int) {
      ValueIterator tmp = *this;
      ++(*this);
      return tmp;
    }

    friend bool operator==(const ValueIterator& a, const ValueIterator& b) {
      return a.ptr_ + a.index_ == b.ptr_ + b.index_;
    }

    friend bool operator!=(const ValueIterator& a, const ValueIterator& b) { return !(a == b); }

   private:
    Node* ptr_;
    bool* reg_;
    size_t size_;
    size_t index_;
  };

  class PairIterator {
   public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = Node;
    using difference_type = std::ptrdiff_t;
    using pointer = Node*;
    using reference = Node&;

    explicit PairIterator(Node* ptr, bool* reg, size_t size) : ptr_(ptr), reg_(reg), size_(size) {
      if (!reg_[index_]) {
        ++(*this);
      }
    }

    reference operator*() const { return ptr_[index_]; }

    pointer operator->() { return &(ptr_[index_]); }

    // Prefix increment
    PairIterator& operator++() {
      do {
        index_++;
      } while (index_ < size_ && !reg_[index_]);
      return *this;
    }

    // Postfix increment
    PairIterator operator++(int) {
      PairIterator tmp = *this;
      ++(*this);
      return tmp;
    }

    friend bool operator==(const PairIterator& a, const PairIterator& b) {
      return a.ptr_ + a.index_ == b.ptr_ + b.index_;
    }

    friend bool operator!=(const PairIterator& a, const PairIterator& b) {
      return a.ptr_ + a.index_ != b.ptr_ + b.index_;
    }

   private:
    Node* ptr_;
    bool* reg_;
    size_t size_;
    size_t index_{0};
  };

  KeyIterator key_begin() { return KeyIterator(data_, register_, N); }

  KeyIterator key_end() { return KeyIterator(data_ + N, register_, N); }

  ValueIterator value_begin() { return ValueIterator(data_, register_, N); }

  ValueIterator value_end() { return ValueIterator(data_ + N, register_, N); }

  PairIterator begin() { return PairIterator(data_, register_, N); }

  PairIterator end() { return PairIterator(data_ + N, register_, N); }

#  ifdef CX_INCLUDE_TESTS
  static void TEST() {
    StackHashMap<int, int, 100> map1;

    map1.insert(1, 10);
    map1.insert(2, 20);
    map1.insert(3, 30);

    CX_ASSERT(map1.size() == 3, "Size has to be 3");

    CX_ASSERT(map1[3] == 30, "Key 3 is 30");

    map1.insert(101, 1010);
    CX_ASSERT(map1.size() == 4, "Size has to be 4");
    CX_ASSERT(map1[101] == 1010, "Key 101 has to be 1010 ");

    bool res = map1.erase(101);
    CX_ASSERT(res, "Value was deleted");
    CX_ASSERT(!map1.contains(101), "Key is not contained");
    CX_ASSERT(map1[101] == 0, "Key was deleted and remade");

    map1.clear();

    for (int i = 0; i < 50; i++) {
      map1.insert(i, i * 10);
      CX_ASSERT(map1[i] == i * 10, "");
    }

    for (int i = 0; i < 50; i++) {
      CX_ASSERT(map1[i] == i * 10, "");
    }

    StackHashMap<int, int, 64> map2;
    for (int i = 0; i < 50; i++) {
      map2.insert(i, i * 10);
      CX_ASSERT(map2[i] == i * 10, "");
    }

    for (auto& pair : map2) {
      pair.val = 10;
    }

    for (auto& pair : map2) {
      CX_ASSERT(pair.val == 10, "");
    }

    StackHashMap<std::string, int, 128> myMap2;

    myMap2.insert("hey", 100);
    myMap2.insert("blabla", 100);

    CX_ASSERT(myMap2["hey"] == 100, "");

    //cxstructs::StackHashMap<const char*, int, 7, cxstructs::Fnv1aHash> compMap;
    //compMap.insert("hello", 5);
    //compMap.insert("bye", 10);
    //compMap.insert("hihi", 15);

    //printf("%d\n", compMap["hello"]);
    //printf("%d\n", compMap["bye"]);
   // printf("%d\n", compMap["hihi"]);
  }
#  endif
};
}  // namespace cxstructs
#endif  //CXSTRUCTS_SRC_CXSTRUCTS_STACKHASHMAP_H_