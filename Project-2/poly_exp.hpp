
///////////////////////////////////////////////////////////////////////////////
// poly_exp.hpp
//
// Definitions for two algorithms that solve the Maximum Subarray Problem,
// and one algorithm that solves the Subset Sum Problem.
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <algorithm>
#include <functional>
#include <optional>
#include <vector>

namespace subarray {

// A summed_span represents a non-empty range of indices inside of a vector of
// ints, stored in a begin iterator and end iterator. The class also stores
// the sum of the ints in that range.
//
// Just like in the rest of the C++ standard library, the range includes all
// elements in [begin, end), or in other words the range includes begin, and all
// elements up to BUT NOT INCLUDING end itself.
class summed_span {
public:
  using iterator = std::vector<int>::const_iterator;

private:
  iterator begin_, end_;
  int sum_;

public:

  // Constructor, given the begin iterator, end iterator, and sum of elements
  // in the range. begin must come before end. sum must be the total of all
  // elements in the range. O(1) time.
  summed_span(iterator begin, iterator end, int sum)
  : begin_(begin), end_(end), sum_(sum) {
    assert(begin < end);
  }

  // Constructor, given only the begin and end iterators. The sum is computed
  // in O(n) time.
  summed_span(iterator begin, iterator end)
  : summed_span(begin, end, std::accumulate(begin, end, 0)) {}

  // Equality tests, two spans are equal when each of their iterators are equal.
  bool operator== (const summed_span& rhs) const {
    return (begin_ == rhs.begin_) && (end_ == rhs.end_);
  }

  // Accessors.
  const iterator& begin() const { return begin_; }
  const iterator& end  () const { return end_  ; }
  int sum() const { return sum_; }

  // Compute the number of elements in the span.
  size_t size() const { return end_ - begin_; }

  // Stream insertion operator, so this class is printable.
  friend std::ostream& operator<<(std::ostream& stream, const summed_span& rhs) {
    stream << "summed_span, size=" << rhs.size() << ", sum=" << rhs.sum();
    return stream;
  }
};

// Compute the maximum subarray of input; i.e. the non-empty contiguous span of
// elements with the maximum sum. input must be nonempty. This function uses an
// exhaustive search algorithm that takes O(n^3) time.
summed_span max_subarray_exh(const std::vector<int>& input) {

  assert(!input.empty());

  // TODO: Rewrite the body of this function so that it actually works. That
  // includes rewriting the return statement. After you do that, delete this
  // comment.
  //  return summed_span(input.begin(), input.begin() + 1);
  return summed_span(input.begin(), input.end());
}

// Compute the maximum subarray using a decrease-by-half algorithm that takes
// O(n log n) time.
summed_span max_subarray_dbh(const std::vector<int>& input) {

  assert(!input.empty());

  // TODO: Rewrite the body of this function so that it actually works. That
  // includes rewriting the return statement. After you do that, delete this
  // comment.
  //  return summed_span(input.begin(), input.begin() + 1);
  return summed_span(input.begin(), input.end());
}

// Solve the subset sum problem: return a non-empty subset of input that adds
// up to exactly target. If no such subset exists, return an empty optional.
// input must not be empty, and must contain fewer than 64 elements.
// Note that the returned subset must never be empty, even if target == 0.
// This uses an exhaustive search algorithm that takes exponential O(n * 2^n)
// time.
std::optional<std::vector<int>>
subset_sum_exh(const std::vector<int>& input, int target) {

  assert(!input.empty());
  assert(input.size() < 64);

  // TODO: Rewrite the body of this function so that it actually works. That
  // includes rewriting the return statement. After you do that, delete this
  // comment.
  //return std::make_optional<std::vector<int>>();
  return std::nullopt;
 }

}
