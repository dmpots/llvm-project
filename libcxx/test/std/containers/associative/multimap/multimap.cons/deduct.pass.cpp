//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// <map>
// UNSUPPORTED: c++03, c++11, c++14

// template<class InputIterator,
//          class Compare = less<iter-value-type<InputIterator>>,
//          class Allocator = allocator<iter-value-type<InputIterator>>>
// multimap(InputIterator, InputIterator,
//          Compare = Compare(), Allocator = Allocator())
//   -> multimap<iter-value-type<InputIterator>, Compare, Allocator>;
// template<class Key, class Compare = less<Key>, class Allocator = allocator<Key>>
// multimap(initializer_list<Key>, Compare = Compare(), Allocator = Allocator())
//   -> multimap<Key, Compare, Allocator>;
// template<class InputIterator, class Allocator>
// multimap(InputIterator, InputIterator, Allocator)
//   -> multimap<iter-value-type<InputIterator>, less<iter-value-type<InputIterator>>, Allocator>;
// template<class Key, class Allocator>
// multimap(initializer_list<Key>, Allocator)
//   -> multimap<Key, less<Key>, Allocator>;
//
// template<ranges::input_range R, class Compare = less<range-key-type<R>>,
//           class Allocator = allocator<range-to-alloc-type<R>>>
//   multimap(from_range_t, R&&, Compare = Compare(), Allocator = Allocator())
//     -> multimap<range-key-type<R>, range-mapped-type<R>, Compare, Allocator>; // C++23
//
// template<ranges::input_range R, class Allocator>
//   multimap(from_range_t, R&&, Allocator)
//     -> multimap<range-key-type<R>, range-mapped-type<R>, less<range-key-type<R>>, Allocator>; // C++23

#include <algorithm> // std::equal
#include <array>
#include <cassert>
#include <climits> // INT_MAX
#include <functional>
#include <map>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#include "deduction_guides_sfinae_checks.h"
#include "test_allocator.h"

using P  = std::pair<int, long>;
using PC = std::pair<const int, long>;

int main(int, char**) {
  {
    const P arr[] = {{1, 1L}, {2, 2L}, {1, 1L}, {INT_MAX, 1L}, {3, 1L}};
    std::multimap m(std::begin(arr), std::end(arr));

    ASSERT_SAME_TYPE(decltype(m), std::multimap<int, long>);
    const PC expected_m[] = {{1, 1L}, {1, 1L}, {2, 2L}, {3, 1L}, {INT_MAX, 1L}};
    assert(std::equal(m.begin(), m.end(), std::begin(expected_m), std::end(expected_m)));
  }

  {
    const P arr[] = {{1, 1L}, {2, 2L}, {1, 1L}, {INT_MAX, 1L}, {3, 1L}};
    std::multimap m(std::begin(arr), std::end(arr), std::greater<int>());

    ASSERT_SAME_TYPE(decltype(m), std::multimap<int, long, std::greater<int>>);
    const PC expected_m[] = {{INT_MAX, 1L}, {3, 1L}, {2, 2L}, {1, 1L}, {1, 1L}};
    assert(std::equal(m.begin(), m.end(), std::begin(expected_m), std::end(expected_m)));
  }

  {
    const P arr[] = {{1, 1L}, {2, 2L}, {1, 1L}, {INT_MAX, 1L}, {3, 1L}};
    std::multimap m(std::begin(arr), std::end(arr), std::greater<int>(), test_allocator<PC>(0, 42));

    ASSERT_SAME_TYPE(decltype(m), std::multimap<int, long, std::greater<int>, test_allocator<PC>>);
    const PC expected_m[] = {{INT_MAX, 1L}, {3, 1L}, {2, 2L}, {1, 1L}, {1, 1L}};
    assert(std::equal(m.begin(), m.end(), std::begin(expected_m), std::end(expected_m)));
    assert(m.get_allocator().get_id() == 42);
  }

  {
    std::multimap<int, long> source;
    std::multimap m(source);
    ASSERT_SAME_TYPE(decltype(m), decltype(source));
    assert(m.size() == 0);
  }

  {
    std::multimap<int, long> source;
    std::multimap m{source}; // braces instead of parens
    ASSERT_SAME_TYPE(decltype(m), decltype(source));
    assert(m.size() == 0);
  }

  {
    std::multimap<int, long> source;
    std::multimap m(source, std::map<int, long>::allocator_type());
    ASSERT_SAME_TYPE(decltype(m), decltype(source));
    assert(m.size() == 0);
  }

  {
    std::multimap m{P{1, 1L}, P{2, 2L}, P{1, 1L}, P{INT_MAX, 1L}, P{3, 1L}};

    ASSERT_SAME_TYPE(decltype(m), std::multimap<int, long>);
    const PC expected_m[] = {{1, 1L}, {1, 1L}, {2, 2L}, {3, 1L}, {INT_MAX, 1L}};
    assert(std::equal(m.begin(), m.end(), std::begin(expected_m), std::end(expected_m)));
  }

  {
    std::multimap m({P{1, 1L}, P{2, 2L}, P{1, 1L}, P{INT_MAX, 1L}, P{3, 1L}}, std::greater<int>());

    ASSERT_SAME_TYPE(decltype(m), std::multimap<int, long, std::greater<int>>);
    const PC expected_m[] = {{INT_MAX, 1L}, {3, 1L}, {2, 2L}, {1, 1L}, {1, 1L}};
    assert(std::equal(m.begin(), m.end(), std::begin(expected_m), std::end(expected_m)));
  }

  {
    std::multimap m(
        {P{1, 1L}, P{2, 2L}, P{1, 1L}, P{INT_MAX, 1L}, P{3, 1L}}, std::greater<int>(), test_allocator<PC>(0, 43));

    ASSERT_SAME_TYPE(decltype(m), std::multimap<int, long, std::greater<int>, test_allocator<PC>>);
    const PC expected_m[] = {{INT_MAX, 1L}, {3, 1L}, {2, 2L}, {1, 1L}, {1, 1L}};
    assert(std::equal(m.begin(), m.end(), std::begin(expected_m), std::end(expected_m)));
    assert(m.get_allocator().get_id() == 43);
  }

  {
    const P arr[] = {{1, 1L}, {2, 2L}, {1, 1L}, {INT_MAX, 1L}, {3, 1L}};
    std::multimap m(std::begin(arr), std::end(arr), test_allocator<PC>(0, 44));

    ASSERT_SAME_TYPE(decltype(m), std::multimap<int, long, std::less<int>, test_allocator<PC>>);
    const PC expected_m[] = {{1, 1L}, {1, 1L}, {2, 2L}, {3, 1L}, {INT_MAX, 1L}};
    assert(std::equal(m.begin(), m.end(), std::begin(expected_m), std::end(expected_m)));
    assert(m.get_allocator().get_id() == 44);
  }

  {
    std::multimap m({P{1, 1L}, P{2, 2L}, P{1, 1L}, P{INT_MAX, 1L}, P{3, 1L}}, test_allocator<PC>(0, 45));

    ASSERT_SAME_TYPE(decltype(m), std::multimap<int, long, std::less<int>, test_allocator<PC>>);
    const PC expected_m[] = {{1, 1L}, {1, 1L}, {2, 2L}, {3, 1L}, {INT_MAX, 1L}};
    assert(std::equal(m.begin(), m.end(), std::begin(expected_m), std::end(expected_m)));
    assert(m.get_allocator().get_id() == 45);
  }

  {
    // Examples from LWG3025
    std::multimap m{std::pair{1, 1}, {2, 2}, {3, 3}};
    ASSERT_SAME_TYPE(decltype(m), std::multimap<int, int>);

    std::multimap m2{m.begin(), m.end()};
    ASSERT_SAME_TYPE(decltype(m2), std::multimap<int, int>);
  }

  {
    // Examples from LWG3531
    std::multimap m1{{std::pair{1, 2}, {3, 4}}, std::less<int>()};
    ASSERT_SAME_TYPE(decltype(m1), std::multimap<int, int>);

    using value_type = std::pair<const int, int>;
    std::multimap m2{{value_type{1, 2}, {3, 4}}, std::less<int>()};
    ASSERT_SAME_TYPE(decltype(m2), std::multimap<int, int>);
  }

#if TEST_STD_VER >= 23
  {
    using Range       = std::array<P, 0>;
    using Comp        = std::greater<int>;
    using DefaultComp = std::less<int>;
    using Alloc       = test_allocator<PC>;

    { // (from_range, range)
      std::multimap c(std::from_range, Range());
      static_assert(std::is_same_v<decltype(c), std::multimap<int, long>>);
    }

    { // (from_range, range, comp)
      std::multimap c(std::from_range, Range(), Comp());
      static_assert(std::is_same_v<decltype(c), std::multimap<int, long, Comp>>);
    }

    { // (from_range, range, comp, alloc)
      std::multimap c(std::from_range, Range(), Comp(), Alloc());
      static_assert(std::is_same_v<decltype(c), std::multimap<int, long, Comp, Alloc>>);
    }

    { // (from_range, range, alloc)
      std::multimap c(std::from_range, Range(), Alloc());
      static_assert(std::is_same_v<decltype(c), std::multimap<int, long, DefaultComp, Alloc>>);
    }
  }
  {
    std::vector<std::pair<const int, float>> pair_vec = {{1, 1.1f}, {2, 2.2f}, {3, 3.3f}};
    std::multimap mm1(pair_vec.begin(), pair_vec.end());
    ASSERT_SAME_TYPE(decltype(mm1), std::multimap<int, float>);

    std::vector<std::tuple<int, double>> tuple_vec = {{10, 1.1}, {20, 2.2}, {30, 3.3}};
    std::multimap mm2(tuple_vec.begin(), tuple_vec.end());
    ASSERT_SAME_TYPE(decltype(mm2), std::multimap<int, double>);

    std::vector<std::array<long, 2>> array_vec = {{100L, 101L}, {200L, 201L}, {300L, 301L}};
    std::multimap mm3(array_vec.begin(), array_vec.end());
    ASSERT_SAME_TYPE(decltype(mm3), std::multimap<long, long>);

    // Check deduction with non-const key in input pair
    std::vector<std::pair<int, char>> non_const_key_pair_vec = {{5, 'a'}, {6, 'b'}};
    std::multimap mm4(non_const_key_pair_vec.begin(), non_const_key_pair_vec.end());
    ASSERT_SAME_TYPE(decltype(mm4), std::multimap<int, char>);
  }
#endif

  AssociativeContainerDeductionGuidesSfinaeAway<std::multimap, std::multimap<int, long>>();

  return 0;
}
