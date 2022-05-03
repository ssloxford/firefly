#include <cassert>
#include <iostream>
#include <string>

#include "seqiter/seqiter.h"

using namespace std::literals;

bool test1() {
  auto in1 = std::array{3,1,4,1,5};
  auto in2 = std::array{9,2,6,5};
  auto in3 = std::array{3,5,8};
  auto in = sequential_iterators<int const &>(it_pair{in1}, it_pair{in2}, it_pair{in3});
  auto out = std::array<int, 12>{};
  std::copy(in, in.end(), out.begin());
  return out == std::array{3,1,4,1,5,9,2,6,5,3,5,8};
}

int main(int, char **) {
  assert(test1);

  auto s = "hello world\n"s;
  auto seq = sequential_iterators<char const &>(it_pair{s.begin(), s.end()},
                                                it_pair{s});
  auto mut_seq =
      sequential_iterators<char &>(it_pair{s.begin(), s.end()}, it_pair{s});
  for (auto x : seq) {
    std::cout << x;
  }
  for (auto &x : mut_seq) {
    x = 'q';
  }
  for (auto x : seq) {
    std::cout << x;
  }
  assert(seq.begin() == seq.begin());
  assert(seq.begin() != ++seq.begin());

  auto const s2 = "hello world\n"s;
  auto mut_seq2 = sequential_iterators<char const &>(
      it_pair{s2.begin(), s2.end()}, it_pair{s2});
  for (auto x : mut_seq2) {
    std::cout << x;
  }

  return 0;
}
