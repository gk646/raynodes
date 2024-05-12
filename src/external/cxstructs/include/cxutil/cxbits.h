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
#ifndef CXSTRUCTS_SRC_CXUTIL_CXBITS_H_
#  define CXSTRUCTS_SRC_CXUTIL_CXBITS_H_

#  include "../cxconfig.h"
#  include <cstdlib>
#  include <print>
#  include <type_traits>  //For type traits

//Assumes little endian for all operations

//// Integer Literals
//int: 42
//unsigned int: 42U
//long: 42L
//unsigned long: 42UL
//long long: 42LL
//unsigned long long: 42ULL
//octal: 052 // Equivalent to decimal 42
//hexadecimal: 0x2A // Equivalent to decimal 42
//binary: 0b101010 // Equivalent to decimal 42
//
//// Floating-Point Literals
//float: 3.14F
//double: 3.14
//long double: 3.14L
//
//// Character Literals
//char: 'A'
//wchar_t: L'A'
//char16_t: u'A'
//char32_t: U'A'
//
//// String Literals
//char string: "Hello"
//wchar_t string: L"Hello"
//char16_t string: u"Hello"
//char32_t string: U"Hello"
//UTF-8 string (C++20): u8"Hello"

namespace cxstructs {

enum Endianness { LITTLE_ENDIAN, BIG_ENDIAN };

// Type trait to check if both types are the same size and either both signed or both unsigned
template <typename T, typename U>
struct is_same_size_and_sign {
  static constexpr bool value =
      (sizeof(T) == sizeof(U)) && (std::is_signed_v<T> == std::is_signed_v<U>);
};

template <typename T, typename U, bool SameSizeAndSign>
struct ConcatType;

template <typename T, typename U>
struct ConcatType<T, U, true> {
  static constexpr size_t totalSize = sizeof(T) * 8 + sizeof(U) * 8;
  using type =
      std::conditional_t<totalSize <= 16, uint16_t,
                         std::conditional_t<totalSize <= 32, uint32_t,
                                            std::conditional_t<totalSize <= 64, uint64_t, void>>>;
};

template <typename T, typename U, size_t PadToBytes>
struct ConcatPaddingType {
  // Calculate the total size needed, considering padding.
  static constexpr size_t paddedSize = PadToBytes > sizeof(T) ? PadToBytes : sizeof(T);
  static constexpr size_t totalSize =
      paddedSize + (PadToBytes > sizeof(U) ? PadToBytes : sizeof(U));

  // Select the smallest standard integer type that fits the total size.
  // Fallback to void for sizes > 64 bits (indicating an error).
  using type =
      std::conditional_t<totalSize <= 2, uint16_t,
                         std::conditional_t<totalSize <= 4, uint32_t,
                                            std::conditional_t<totalSize <= 8, uint64_t, void>>>;
};

/**Concatenates two numbers padding each number to at least PadToBytes
 *
 * @tparam T integral type
 * @tparam U integral type
 * @tparam PadToBytes minimal size (numbers smaller than this will be padded)
 * @param first will be in the first bits
 * @param second  will be in the second bits
 * @return a number consisting out of the bits of T and U
 */

template <typename T, typename U, size_t PadToBytes>
inline auto bits_concat_ex(T first, U second) {
  static_assert(std::is_integral_v<T> && std::is_integral_v<U>,
                "Only integer types are supported.");

  using ResultType = typename ConcatPaddingType<T, U, PadToBytes>::type;
  static_assert(!std::is_same_v<ResultType, void>, "Resulting type is too large to represent.");

  constexpr size_t shiftAmount = PadToBytes > sizeof(T) ? PadToBytes * 8 : sizeof(T) * 8;

  return (static_cast<ResultType>(second) << shiftAmount) | first;
}

/**Concatenates two equal sized numbers
 *
 * @tparam T number type
 * @tparam U
 * @param first
 * @param second
 * @return
 */
template <typename T>
constexpr auto bits_concat(T first, T second) {
  static_assert(std::is_integral_v<T>, "Only integer types are supported.");
  constexpr auto same = is_same_size_and_sign<T, T>::value;
  constexpr bool fits = !std::is_same_v<typename ConcatType<T, T, same>::type, void>;
  static_assert(fits, "Resulting type is too large to represent.");

  using ResultType = typename ConcatType<T, T, is_same_size_and_sign<T, T>::value>::type;
  return (static_cast<ResultType>(second) << (sizeof(T) * 8)) | static_cast<ResultType>(first);
}

/**Prints the bits of the given num
 *
 * @tparam T
 * @param num must be integral
 * @return void - prints to stdout (flushes on result)
 */
template <typename T>
inline void bits_print(T num) {
  static_assert(std::is_integral_v<T>, "Only integral types are supported.");

  constexpr size_t numBits = sizeof(T) * 8;  // Total bits in type T
  char bitRepresentation[numBits + 1];       // +1 for null terminator
  bitRepresentation[numBits] = '\0';         // Null-terminate the string

  for (size_t i = 0; i < numBits; ++i) {
    bitRepresentation[(numBits - 1) - i] = (num & (T(1) << i)) ? '1' : '0';
  }

  fputs(bitRepresentation, stdout);
  fputc('\n', stdout);
  fflush(stdout);
}

/**Prints the bytes of the given num in the given format
 *
 * @tparam T
 * @param num must be integral
 * @param endian  endian value (defaults to little-endian)
 */
template <typename T>
inline void bits_print_bytes(const T& num, Endianness endian = LITTLE_ENDIAN) {
  static_assert(std::is_integral_v<T>, "Only integral types are supported.");

  const size_t numBytes = sizeof(T);
  const auto* bytePointer = reinterpret_cast<const unsigned char*>(&num);

  if (endian == LITTLE_ENDIAN) {
    for (size_t i = 0; i < numBytes; ++i) {
      std::print("{:02X} ", bytePointer[i]);
    }
  } else {
    for (size_t i = numBytes; i > 0; --i) {
      std::print("{:02X} ", bytePointer[i - 1]);
    }
  }
  fputc('\n', stdout);
  fflush(stdout);
}
/**
 * Retrieves a number from within a given number.
 * The given range is [off] - [off + sizeof(R)]
 * @tparam R return type
 * @tparam T must be integral
 * @param num the given number to extract from
 * @param off a right offset in bits
 * @return a number of type r created from the specified bit range
 */
template <typename R, typename T>
constexpr auto bits_get(T num, uint8_t off = 0) -> R {
  static_assert(std::is_integral_v<T>, "Only integer type are supported.");
  return static_cast<R>(num >> off);
}

#  ifdef CX_INCLUDE_TESTS
static void TEST_BITS() {
  CX_ASSERT(bits_concat(0x1, 0x01) == 4294967297, "");
  CX_ASSERT(bits_concat(0x0, 0x01) == 4294967296, "");
  CX_ASSERT(bits_concat(static_cast<uint8_t>(0x1), static_cast<uint8_t>(0x01)) == 0x101,
            "Concatenation of uint8_t failed.");
  CX_ASSERT(bits_concat(static_cast<uint16_t>(0x0), static_cast<uint16_t>(0x01)) == 0x10000,
            "Concatenation of uint16_t failed.");
  CX_ASSERT(bits_concat(static_cast<uint32_t>(0x1), static_cast<uint32_t>(0x2)) == 0x200000001,
            "Concatenation of uint32_t failed.");
  uint16_t num1 = 1;
  uint16_t num2 = 2;
  CX_ASSERT(bits_concat(num1, num2) == 0x20001,
            "Concatenation of uint16_t with different values failed.");

  CX_ASSERT(bits_concat(static_cast<uint8_t>(0xFF), static_cast<uint8_t>(0xFF)) == 0xFFFF,
            "Concatenation of max uint8_t failed.");

  // CX_ASSERT(concat(static_cast<int16_t>(-1), static_cast<int16_t>(-2)) == static_cast<int32_t>(0xFFFFFEFF), "Concatenation of int16_t failed.");
  //CX_ASSERT(concat(static_cast<int8_t>(-1), static_cast<int8_t>(-1)) == static_cast<int16_t>(-1), "Concatenation of max int8_t failed.");

  bits_print(bits_concat(0b0001, 0b0001));
  CX_ASSERT(bits_get<uint8_t>(bits_concat(0b0001, 0b0001), 0)
                == bits_get<uint8_t>(bits_concat(0b0001, 0b0001), 32),
            "");
  uint16_t bla = 1;
  uint16_t a = 257;

  bits_print(bits_get<uint8_t>(513));
  bits_print(a);

  auto res = bits_concat_ex<uint16_t, uint16_t, 4>(1, 1);
  bits_print(res);
}
#  endif

}  // namespace cxstructs

#endif  //CXSTRUCTS_SRC_CXUTIL_CXBITS_H_
