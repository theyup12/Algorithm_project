///////////////////////////////////////////////////////////////////////////////
// algorithms_timing.cpp
//
// Example code showing how to run each algorithm while measuring
// elapsed times precisely. You should modify this program to gather
// all of your experimental data.
//
///////////////////////////////////////////////////////////////////////////////

#include <algorithm>
#include <cassert>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "timer.hpp"

#include "algorithms.hpp"

void print_bar() {
  std::cout << std::string(79, '-') << std::endl;
}

int main() {

  const size_t n = 2*1000; // 2,000

  assert(n > 0);

  std::vector<int> vec;
  std::string str;
  {
    std::mt19937 rng(0); // Use a hardcoded seed for reproducibility between runs.
    std::uniform_int_distribution<> randint(-100, +100),
      randchar(' ', '~');
    for (size_t i = 0; i < n; ++i) {
      vec.push_back(randint(rng));
      str.push_back(randchar(rng));
    }
  }
  assert(n == vec.size());
  assert(n == str.size());

  Timer timer;
  double elapsed;

  print_bar();
  std::cout << "n = " << n << std::endl;

  print_bar();
  std::cout << "find dip" << std::endl;
  {
    timer.reset();
    algorithms::find_dip(vec);
    elapsed = timer.elapsed();
  }
  std::cout << "elapsed time=" << elapsed << " seconds" << std::endl;

  print_bar();
  std::cout << "longest balanced span" << std::endl;
  {
    timer.reset();
    algorithms::longest_balanced_span(vec);
    elapsed = timer.elapsed();
  }
  std::cout << "elapsed time=" << elapsed << " seconds" << std::endl;

  print_bar();
  std::cout << "telegraph_style" << std::endl;
  {
    timer.reset();
    algorithms::telegraph_style(str);
    elapsed = timer.elapsed();
  }
  std::cout << "elapsed time=" << elapsed << " seconds" << std::endl;

  print_bar();

  return 0;
}
