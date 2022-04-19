#include <cmath>
#include <cxxopts.hpp>
#include <iostream>
#include <vector>

#include "libcadu/libcadu.h"

int main(int argc, char *argv[]) {
  cxxopts::Options options("cadutail", "Output the last part of a CADU stream from stdin, in whole CADUs, from a given index");
  options.add_options()
    (
      "n,index",
      "Output CADUs from index arg, indexing from the end of the stream. A leading '+' indexes from the start of the stream - <int>",
      cxxopts::value<std::string>()->default_value("10")
    )
    ("h,help", "Print usage")
    ;

  auto result = options.parse(argc, argv);

  // Show help menu
  if (result.count("help")) {
    std::cerr << options.help() << '\n';
    exit(0);
  }

  bool valid = true;

  auto index_str = result["index"].as<std::string>();
  auto sign = false; // Set the sign to be from the back by default
  int index;

  if (index_str.empty()) {
    std::cerr << "Error: index string was empty\n";
    valid = false;
  } else {
    if (index_str[0] == '+') {
      sign = true;
    }

    try {
      index = std::abs(std::stoi(index_str));
    }
    catch(std::invalid_argument const& ex)
    {
        std::cerr << "Error: index invalid_argument: " << ex.what() << '\n';
        valid = false;
    }
    catch(std::out_of_range const& ex)
    {
        std::cerr << "Error: index out of range: " << ex.what() << '\n';
        valid = false;
    }
  }

  if (!valid) {
    std::cerr << "Quitting..." << '\n';
    exit(1);
  }

  // Set the index to be from the back by default, as in POSIX `tail`


  if (sign) {
    // Positive sign
    std::cerr << "positive\n";
    int n = 0;
    CADU cadu;
    while (nonrandomised::operator>>(std::cin, cadu)) {
      if (n < index) {
        n++;
      } else {
        // No need to increment n
        nonrandomised::operator<<(std::cout, cadu);
      }
    }
  } else {
    if (index != 0) {
      // Negative sign
      std::vector<CADU> buffer(index);
      buffer.reserve(index);

      // Read the last index elements into a ring buffer
      int n = 0;
      while (nonrandomised::operator>>(std::cin, buffer.at(n%index))) {
        n++;
      }

      // Read the last elements out of the buffer
      for (int i = 0; i < std::min(n, index); i++) {
        nonrandomised::operator<<(std::cout, buffer.at((n - std::min(n, index) + i)%index));
      }
    }
  }
}
