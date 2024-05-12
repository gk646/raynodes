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
#ifndef CXSTRUCTS_SRC_CXSTRUCTS_STACKARRAY_H_
#  define CXSTRUCTS_SRC_CXSTRUCTS_STACKARRAY_H_

#  include "../cxconfig.h"
#  include <iterator>  // For std::forward_iterator_tag

//StackVector is a compile-time sized container very similar to a std::array
//However it differs in its interface being closer to a std::vector
//When keeping track of the size you can avoid a lot of the ugly syntax that comes when checking
//for empty slots in a std::array (or it might not even be possible)

//  [Basically you trade 8 bytes of additional memory (the size type will always be padded to 8) for clean syntax and a nice interface]
//    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// -> With the redesigned std::byte reducing the size of size_type can matter

// SUPPORTS non-trivial types!
// SUPPORTS Complex constructors!

namespace cxstructs {
template <typename T, size_t N, typename size_type = uint32_t>
class StackVector {
  alignas(T) std::byte data_[sizeof(T) * N];  // Stack-allocated array
  size_type size_ = 0;                        // Current number of elements in the array

 public:
  StackVector() : size_(0) {}

  StackVector(const std::initializer_list<T>& elems) {
    for (const auto& e : elems) {
      push_back(e);
    }
  }

  explicit StackVector(size_type count, const T& value = T()) : size_(count) {
    CX_ASSERT(count <= N, "Initial size exceeds maximum capacity");
    for (size_type i = 0; i < count; ++i) {
      new (reinterpret_cast<T*>(data_) + i) T(value);
    }
  }

  StackVector(const StackVector& other) : size_(other.size_) {
    for (size_type i = 0; i < other.size_; ++i) {
      new (reinterpret_cast<T*>(data_) + i) T(*(reinterpret_cast<const T*>(other.data_) + i));
    }
  }

  // Copy assignment operator
  StackVector& operator=(const StackVector& other) {
    if (this != &other) {
      clear();
      for (size_type i = 0; i < other.size_; ++i) {
        new (reinterpret_cast<T*>(data_) + i) T(*(reinterpret_cast<const T*>(other.data_) + i));
      }
      size_ = other.size_;
    }
    return *this;
  }

  // Move constructor
  StackVector(StackVector&& other) noexcept(std::is_nothrow_move_constructible_v<T>) : size_(other.size_) {
    for (size_type i = 0; i < other.size_; ++i) {
      new (reinterpret_cast<T*>(data_) + i) T(std::move(*(reinterpret_cast<T*>(other.data_) + i)));
    }
    other.size_ = 0;
  }

  // Move assignment operator
  StackVector& operator=(StackVector&& other) noexcept(std::is_nothrow_move_assignable_v<T>) {
    if (this != &other) {
      clear();
      for (size_type i = 0; i < other.size_; ++i) {
        new (reinterpret_cast<T*>(data_) + i) T(std::move(*(reinterpret_cast<T*>(other.data_) + i)));
      }
      size_ = other.size_;
      other.size_ = 0;
    }
    return *this;
  }

  ~StackVector() { clear(); }

  // Call can fail
  void push_back(const T& value) {
    CX_ASSERT(size_ < N, "Attempt to add to a full StackArray");
    CX_STACK_ABORT_IMPL();
    new (reinterpret_cast<T*>(data_) + size_) T(value);
    ++size_;
  }

  void push_back(T&& value) {
    CX_ASSERT(size_ < N, "Attempt to add to a full StackArray");
    CX_STACK_ABORT_IMPL();
    new (reinterpret_cast<T*>(data_) + size_) T(std::move(value));
    ++size_;
  }

  // Call cannot fail | Overwrites first elements and resets size on loop
  void push_back_loop(const T& value) {
    size_type index = size_ % N;
    if constexpr (!std::is_trivially_copyable_v<T>) {
      if (size_ >= N) {
        reinterpret_cast<T*>(data_ + index * sizeof(T))->~T();
      }
    }
    new (reinterpret_cast<T*>(data_) + size_) T(value);
    size_ = (size_ + 1) % N;
  }

  template <typename... Args>
  void emplace_back(Args&&... args) {
    CX_ASSERT(size_ < N, "Attempt to add to a full StackArray");
    CX_STACK_ABORT_IMPL();
    new (reinterpret_cast<T*>(data_) + size_) T(std::forward<Args>(args)...);
    ++size_;
  }

  auto operator[](size_type index) -> T& {
    CX_ASSERT(index < size_, "Index out of range");
    return *reinterpret_cast<T*>(data_ + index * sizeof(T));
  }

  auto operator[](size_type index) const -> const T& {
    CX_ASSERT(index < size_, "Index out of range");
    return *reinterpret_cast<const T*>(data_ + index * sizeof(T));
  }

  [[nodiscard]] auto size() const -> size_type { return size_; }

  [[nodiscard]] constexpr auto capacity() const -> size_type { return N; }

  [[nodiscard]] auto empty() const -> bool { return size_ == 0; }

  [[nodiscard]] auto full() const -> bool { return size_ == N; }

  [[nodiscard]] auto contains(const T& value) const -> bool {
    for (int i = 0; i < size_; ++i) {
      if (data_[i] == value) return true;
    }
    return false;
  }

  void clear() {
    for (size_type i = 0; i < size_; ++i) {
      reinterpret_cast<T*>(data_ + i * sizeof(T))->~T();
    }
    size_ = 0;
  }

  void resize(size_type new_size, const T& value = T()) {
    if (new_size > size_) {
      CX_ASSERT(new_size <= N, "Resize size exceeds maximum capacity");
      for (size_type i = size_; i < new_size; ++i) {
        new (reinterpret_cast<T*>(data_) + i) T(value);
      }
    } else {
      for (size_type i = new_size; i < size_; ++i) {
        reinterpret_cast<T*>(data_ + i * sizeof(T))->~T();
      }
    }
    size_ = new_size;
  }

  auto front() -> T& {
    if (size_ > 0) {
      return *reinterpret_cast<T*>(data_);
    }
    std::abort();
  }

  auto back() noexcept -> T& {
    if (size_ > 0) {
      return *reinterpret_cast<T*>(data_ + size_ * sizeof(T));
    }
    std::abort();
  }

  auto front() const -> T {
    if (size_ > 0) {
      return *reinterpret_cast<T*>(data_);
    }
    std::abort();
  }

  auto back() const -> T {
    if (size_ > 0) {
      return *reinterpret_cast<T*>(data_ + size_ * sizeof(T));
    }
    std::abort();
  }

  auto data() -> T* { return reinterpret_cast<T*>(data_); }

  auto cdata() -> const T* { return reinterpret_cast<const T*>(data_); }

  template <typename PtrType>
  class IteratorTemplate {
   public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = std::remove_const_t<PtrType>;
    using difference_type = std::ptrdiff_t;
    using pointer = PtrType*;
    using reference = PtrType&;

    explicit IteratorTemplate(pointer ptr) : ptr_(ptr) {}

    reference operator*() const { return *ptr_; }
    pointer operator->() const { return ptr_; }

    IteratorTemplate& operator++() {
      ptr_ = reinterpret_cast<pointer>(
          reinterpret_cast<std::conditional_t<std::is_const_v<PtrType>, const std::byte*, std::byte*>>(ptr_)
          + sizeof(T));
      return *this;
    }

    IteratorTemplate operator++(int) {
      IteratorTemplate tmp = *this;
      ++(*this);
      return tmp;
    }

    friend bool operator==(const IteratorTemplate& a, const IteratorTemplate& b) { return a.ptr_ == b.ptr_; }
    friend bool operator!=(const IteratorTemplate& a, const IteratorTemplate& b) { return a.ptr_ != b.ptr_; }

   private:
    pointer ptr_;
  };

  using Iterator = IteratorTemplate<T>;
  using ConstIterator = IteratorTemplate<const T>;

  Iterator erase(Iterator pos) {
    auto posPtr = &(*pos);
    size_type index = posPtr - &(*begin());

    std::move(posPtr + 1, &(*end()), posPtr);

    if constexpr (!std::is_trivially_destructible_v<T>) {
      data_[size_ - 1].~T();
    }

    --size_;

    return Iterator(reinterpret_cast<T*>(data_ + index * sizeof(T)));
  }


  Iterator begin() { return Iterator(reinterpret_cast<T*>(data_)); }

  Iterator end() { return Iterator(reinterpret_cast<T*>(data_ + size_ * sizeof(T))); }

  ConstIterator begin() const { return ConstIterator(reinterpret_cast<const T*>(data_)); }

  ConstIterator end() const { return ConstIterator(reinterpret_cast<const T*>(data_ + size_ * sizeof(T))); }
};
}  // namespace cxstructs

#  ifdef CX_INCLUDE_TESTS
#    include <string>
#    include <iostream>
namespace cxtests {
struct ComplexType {
  std::string name;
  int value;

  ComplexType(std::string n, int v) : name(std::move(n)), value(v) {
    std::cout << "ComplexType created: " << name << std::endl;
  }

  ComplexType(const ComplexType& other) : name(other.name), value(other.value) {
    std::cout << "ComplexType copied: " << name << std::endl;
  }

  ~ComplexType() { std::cout << "ComplexType destroyed: " << name << std::endl; }
};

// Test functions
static void testBasicOperations() {
  cxstructs::StackVector<int, 100> intVector;
  for (int i = 0; i < 10; i++) {
    intVector.push_back(i * 10);
  }
  CX_ASSERT(intVector.size() == 10, "Size should be 10 after adding 10 elements");

  intVector.clear();
  CX_ASSERT(intVector.size() == 0, "Size should be 0 after clear");
}

static void testComplexTypeOperations() {
  cxstructs::StackVector<ComplexType, 10> complexVector;
  complexVector.push_back(ComplexType("Item1", 100));
  complexVector.push_back(ComplexType("Item2", 200));

  CX_ASSERT(complexVector.size() == 2, "Size should be 2 after adding 2 complex elements");

  // Checking destructor calls on clear by listening to console output
  complexVector.clear();
  CX_ASSERT(complexVector.size() == 0, "Size should be 0 after clear");
}

static void testCopyConstructor() {
  cxstructs::StackVector<int, 5> original;
  for (int i = 0; i < 5; i++) {
    original.push_back(i);
  }

  cxstructs::StackVector<int, 5> copied = original;
  CX_ASSERT(copied.size() == 5, "Copied vector should have size 5");
  for (int i = 0; i < 5; i++) {
    CX_ASSERT(copied[i] == i, "Copied vector elements should match original");
  }
}

static void TEST_STACK_VECTOR() {
  cxstructs::StackVector<int, 100> arr;
  for (int i = 0; i < 10; i++) {
    arr.push_back(100);
  }
  CX_ASSERT(arr.size() == 10, "Size has to be 10");

  arr.clear();
  CX_ASSERT(arr.size() == 0, "Size has to be 0");

  testBasicOperations();
  testComplexTypeOperations();
  testCopyConstructor();
}
}  // namespace cxtests
#  endif
#endif  //CXSTRUCTS_SRC_CXSTRUCTS_STACKARRAY_H_