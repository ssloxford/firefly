#include <iostream>
#include <cxxopts.hpp>

#include "libcadu/libcadu.h"

int main(int argc, char *argv[]) {
  cxxopts::Options options("cadurandomise", "Applies the randomisation polynomial to a CADU stream on stdin");
  options.add_options()
    ("h,help", "Print usage")
    ;

  auto result = options.parse(argc, argv);

  // Show help menu
  if (result.count("help")) {
    std::cerr << options.help() << '\n';
    exit(0);
  }

  std::copy
    ( std::istream_iterator<randomised::CADU>(std::cin)
    , std::istream_iterator<randomised::CADU>()
    , std::ostream_iterator<nonrandomised::CADU>(std::cout)
    );
}
