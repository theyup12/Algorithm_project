
///////////////////////////////////////////////////////////////////////////////
// balance_test.hpp
//
// Unit tests for the functionality declared in algorithms.hpp .
///////////////////////////////////////////////////////////////////////////////

#include <random>
#include <vector>

#include "gtest/gtest.h"

#include "algorithms.hpp"

template <typename T>
std::vector<T> random_vector(size_t size, T min, T max) {
  std::vector<T> result;
  auto gen = std::uniform_int_distribution(min, max);
  auto rng = std::minstd_rand(0);
  std::generate_n(std::back_inserter(result),
		  size,
		  [&](){ return gen(rng); });
  return result;
}

TEST(find_dip_trivial_cases, trivial_cases) {
  { // input too small to find a dip
    std::vector<int> empty,
                     one_element{5},
                     two_elements{5, 6};
    EXPECT_EQ(empty.end(), algorithms::find_dip(empty));
    EXPECT_EQ(one_element.end(), algorithms::find_dip(one_element));
    EXPECT_EQ(two_elements.end(), algorithms::find_dip(two_elements));
  }

  { // input only contains a dip
    std::vector<int> dip{8, 2, 8};
    EXPECT_EQ(dip.begin(), algorithms::find_dip(dip));
  }

  { // input is small and definitely does not contain a dip
    std::vector<int> increasing{1, 2, 3}, zeroes{0, 0, 0};
    EXPECT_EQ(increasing.end(), algorithms::find_dip(increasing));
    EXPECT_EQ(zeroes.end(), algorithms::find_dip(zeroes));
  }
}

TEST(find_dip_nontrivial_cases, nontrivial_cases) {
  { // dip using entirely negative ints
    std::vector<int> negatives{-10, -12, -10};
    EXPECT_EQ(negatives.begin(), algorithms::find_dip(negatives));
  }

  { // three dips, finds the last one
    std::vector<int> vec{5, 4, 5, 10, 8, 7, 8, 10, 9, 8, 9, 10};
    EXPECT_EQ(vec.begin() + 8, algorithms::find_dip(vec));
  }

  { // large vector, 1 million elements, all the same
    std::vector<int> big(1000000, 1);
    EXPECT_EQ(big.end(), algorithms::find_dip(big));
  }

  { // large vector, rotating between four different values
    std::vector<int> values{2, 4, 6, 8}, rotating;
    for (unsigned i = 0; i < 1000000; ++i) {
      rotating.push_back(values[i % 4]);
    }
    EXPECT_EQ(rotating.end(), algorithms::find_dip(rotating));
  }

  { // large vector, dip near the middle
    std::vector<int> big(1000000, 1);
    size_t i = big.size() / 2;
    big[i] = 8;
    big[i+1] = 7;
    big[i+2] = 8;
    EXPECT_EQ(big.begin() + i, algorithms::find_dip(big));
  }

  { // large vector, dip near the very end
    std::vector<int> big(1000000, 1);
    size_t i = big.size() - 4;
    big[i] = 5;
    big[i+1] = 2;
    big[i+2] = 5;
    EXPECT_EQ(big.begin() + i, algorithms::find_dip(big));
  }

  { // stress test: large pseudo-random vector
    auto big = random_vector<int>(10*1000*1000, -10, 10);
    ASSERT_EQ(10*1000*1000, big.size());
    algorithms::find_dip(big);
  }
}

TEST(longest_balanced_span_trivial_cases, trivial_cases) {
  // empty
  {
    std::vector<int> empty;
    EXPECT_FALSE(algorithms::longest_balanced_span(empty));
  }

  // only one element that is not zero
  {
    std::vector<int> five{5};
    EXPECT_FALSE(algorithms::longest_balanced_span(five));
  }

  // several elements that are not zero
  {
    std::vector<int> four{5, 2, -1, 8};
    EXPECT_FALSE(algorithms::longest_balanced_span(four));
  }

  // only one zero, that's the only span
  {
    std::vector<int> zero{0};
    auto got = algorithms::longest_balanced_span(zero);
    ASSERT_TRUE(got);
    EXPECT_EQ(algorithms::span(zero.begin(), zero.end()), *got);
  }

  { // four-element vector, zero at index 0
    std::vector<int> four{0, 2, -1, 8};
    auto got = algorithms::longest_balanced_span(four);
    ASSERT_TRUE(got);
    EXPECT_EQ(algorithms::span(four.begin(), four.begin() + 1), *got);
  }

  { // four-element vector, zero at index 1
    std::vector<int> four{5, 0, -1, 8};
    auto got = algorithms::longest_balanced_span(four);
    ASSERT_TRUE(got);
    EXPECT_EQ(algorithms::span(four.begin() + 1, four.begin() + 2), *got);
  }

  { // four-element vector, zero at index 2
    std::vector<int> four{5, 2, 0, 8};
    auto got = algorithms::longest_balanced_span(four);
    ASSERT_TRUE(got);
    EXPECT_EQ(algorithms::span(four.begin() + 2, four.begin() + 3), *got);
  }

  { // four-element vector, zero at index 3
    std::vector<int> four{5, 2, -1, 0};
    auto got = algorithms::longest_balanced_span(four);
    ASSERT_TRUE(got);
    EXPECT_EQ(algorithms::span(four.begin() + 3, four.begin() + 4), *got);
  }

  { // negatives and positives cancel
    std::vector<int> four{8, 5, -5, 7};
    auto got = algorithms::longest_balanced_span(four);
    ASSERT_TRUE(got);
    EXPECT_EQ(algorithms::span(four.begin() + 1, four.begin() + 3), *got);
  }

  { // two small negatives cancel a large positive
    std::vector<int> four{8, -2, -3, 5};
    auto got = algorithms::longest_balanced_span(four);
    ASSERT_TRUE(got);
    EXPECT_EQ(algorithms::span(four.begin() + 1, four.begin() + 4), *got);
  }
}

TEST(longest_balanced_span_nontrivial_cases, nontrivial_cases) {
  { // entire vector sums to zero
    std::vector<int> four{6, -2, -5, 1};
    auto got = algorithms::longest_balanced_span(four);
    ASSERT_TRUE(got);
    EXPECT_EQ(algorithms::span(four.begin(), four.end()), *got);
  }

  { // length-2 followed by length-1
    std::vector<int> six{4, 3, -3, 2, 0, 8};
    auto got = algorithms::longest_balanced_span(six);
    ASSERT_TRUE(got);
    EXPECT_EQ(algorithms::span(six.begin() + 1, six.begin() + 3), *got);
  }

  { // length-1 followed by length-2
    std::vector<int> six{4, 0, 2, -3, 3, 8};
    auto got = algorithms::longest_balanced_span(six);
    ASSERT_TRUE(got);
    EXPECT_EQ(algorithms::span(six.begin() + 3, six.begin() + 5), *got);
  }

  { // two length-2s, picks the LATER one
    std::vector<int> seven{3, 2, -2, 3, -4, 4, 3};
    auto got = algorithms::longest_balanced_span(seven);
    ASSERT_TRUE(got);
    EXPECT_EQ(algorithms::span(seven.begin() + 4, seven.begin() + 6), *got);
  }

  { // many length-3's, picks the LAST one
    std::vector<int> big;
    for (unsigned i = 0; i < 100; ++i) {
      big.push_back(8);
      big.push_back(-1);
      big.push_back(-1);
      big.push_back(2);
      big.push_back(7);
    }
    ASSERT_EQ(500, big.size());
    auto got = algorithms::longest_balanced_span(big);
    ASSERT_TRUE(got);
    EXPECT_EQ(algorithms::span(big.end() - 4, big.end() - 1), *got);
  }

  { // big vector of all zeros, picks everything
    std::vector<int> big(500, 0);
    ASSERT_EQ(500, big.size());
    auto got = algorithms::longest_balanced_span(big);
    ASSERT_TRUE(got);
    EXPECT_EQ(algorithms::span(big.begin(), big.end()), *got);
  }

  { // large pseudo-random vector
    auto big = random_vector<int>(500, -10, 10);
    ASSERT_EQ(500, big.size());
    auto got = algorithms::longest_balanced_span(big);
    EXPECT_TRUE(got);
  }
}

TEST(telegraph_style_trivial_cases, trivial_cases) {

  // empty string: just append STOP.
  EXPECT_EQ("STOP.", algorithms::telegraph_style(""));

  // "STOP.": leave unchanged
  EXPECT_EQ("STOP.", algorithms::telegraph_style("STOP."));

  // lower case letters are converted to upper case
  EXPECT_EQ("ABCSTOP.", algorithms::telegraph_style("abcSTOP."));
  EXPECT_EQ("ABCDEFGHIJKLMNOPQRSTUVWXYZSTOP.",
	    algorithms::telegraph_style("abcdefghijklmnopqrstuvwxyzSTOP."));

  // punctuation coversion
  EXPECT_EQ("A.B STOP.", algorithms::telegraph_style("A!B "));
  EXPECT_EQ("A.B STOP.", algorithms::telegraph_style("A?B "));
  EXPECT_EQ("A.B STOP.", algorithms::telegraph_style("A;B "));
  EXPECT_EQ("...AB STOP.", algorithms::telegraph_style("!?;AB "));
  EXPECT_EQ("...AB STOP.", algorithms::telegraph_style(";?!AB "));
  EXPECT_EQ("AB ...STOP.", algorithms::telegraph_style("AB !?;"));
  EXPECT_EQ("AB ...STOP.", algorithms::telegraph_style("AB ;?!"));
  
  // upper case letters are preserved
  EXPECT_EQ("ABCSTOP.", algorithms::telegraph_style("ABC"));
  EXPECT_EQ("ABCDEFGHIJKLMNOPQRSTUVWXYZSTOP.",
	    algorithms::telegraph_style("ABCDEFGHIJKLMNOPQRSTUVWXYZ"));

  // digits are preserved
  EXPECT_EQ("123STOP.", algorithms::telegraph_style("123"));
  EXPECT_EQ("1234567890STOP.", algorithms::telegraph_style("1234567890"));

  // space is preserved
  EXPECT_EQ(" STOP.", algorithms::telegraph_style(" "));

  // periods are preserved
  EXPECT_EQ(".STOP.", algorithms::telegraph_style("."));
  EXPECT_EQ("...STOP.", algorithms::telegraph_style("..."));

  // other characters are removed
  EXPECT_EQ("XY STOP.", algorithms::telegraph_style("X`~@#$%^&*()-_=+Y "));
  EXPECT_EQ("XY STOP.", algorithms::telegraph_style("X\t\r\nY "));
  EXPECT_EQ("XY STOP.", algorithms::telegraph_style("X[]{}\\|:'\"<,>Y "));
}

TEST(telegraph_style_nontrivial_cases, nontrivial_cases) {

  // multiple spaces
  EXPECT_EQ(" ABSTOP.", algorithms::telegraph_style("    AB")); // at front
  EXPECT_EQ("AB STOP.", algorithms::telegraph_style("AB    ")); // at back
  EXPECT_EQ("A BSTOP.", algorithms::telegraph_style("A    B")); // at back
  EXPECT_EQ(" A B STOP.", algorithms::telegraph_style("    A    B    ")); // all three

  // lower-case stop counts
  EXPECT_EQ("STOP.", algorithms::telegraph_style("stop."));

  // finish the stop with converted punctuation
  EXPECT_EQ("STOP.", algorithms::telegraph_style("STOP;"));
  EXPECT_EQ("STOP.", algorithms::telegraph_style("STOP!"));
  EXPECT_EQ("STOP.", algorithms::telegraph_style("STOP?"));

  // finish the stop with removed characters
  EXPECT_EQ("STOP.", algorithms::telegraph_style("ST^$__OP."));

  // many blocks of spaces
  EXPECT_EQ(" A B C D E F STOP.",
	    algorithms::telegraph_style("  A       B   C       D  E F         "));
  
  // stress test: 10MB string
  {
    auto vect = random_vector<char>(10*1000*1000, ' ', '~');
    std::string big(vect.begin(), vect.end());
    algorithms::telegraph_style(big);
  }
}
