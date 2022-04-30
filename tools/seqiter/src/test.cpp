#include <cassert>
#include <iostream>
#include <string>

#include "seqiter/seqiter.h"

using namespace std::literals;

int main(int, char **) {
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
