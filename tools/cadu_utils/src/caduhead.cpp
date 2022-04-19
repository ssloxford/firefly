#include <cxxopts.hpp>
#include <iostream>
#include <vector>

#include "libcadu/libcadu.h"

int main(int argc, char *argv[]) {
  cxxopts::Options options("caduhead", "Output the first part of a CADU stream from stdin, in whole CADUs, up to a given index");
  options.add_options()
    (
      "n,index",
      "Output CADUs up to index arg, indexing from the start of the stream. A leading '-' indexes from the end of the stream - <int>",
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
  auto sign = true; // Set the sign to be from the front by default
  int index;

  if (index_str.empty()) {
    std::cerr << "Error: index string was empty\n";
    valid = false;
  } else {
    if (index_str[0] == '-') {
      sign = false;
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

  if (sign) {
    std::cerr << "positive\n";
    // Positive sign
    int n = 0;
    CADU cadu;
    while (nonrandomised::operator>>(std::cin, cadu)) {
      if (n < index) {
        nonrandomised::operator<<(std::cout, cadu);
        n++;
      } else {
        break;
      }
    }
  } else {
    std::cerr << "negative\n";
    // Negative sign
    // Fill all the CADUs into a buffer
    std::vector<CADU> buffer = {};
    while (nonrandomised::operator>>(std::cin, buffer.emplace_back())) {}

    // Calculate the new index
    auto front_index = buffer.size() - index;

    // Output from the buffer
    int n = 0;
    for (auto c : buffer) {
      if (n < front_index) {
        nonrandomised::operator<<(std::cout, c);
        n++;
      } else {
        break;
      }
    }
  }
}
