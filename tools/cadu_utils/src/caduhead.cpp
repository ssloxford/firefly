#include <cxxopts.hpp>
#include <iostream>
#include <vector>

#include "libcadu/libcadu.h"
using namespace nonrandomised;

template <typename It>
class subrange {
private:
  It begin_iter;
  It end_iter;

public:
  subrange (It begin, It end): begin_iter{begin}, end_iter{end} {};

  auto begin() {
    return begin_iter;
  }

  auto end() {
    return end_iter;
  }
};

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
    // TODO: this exhibits UB once we get to the end of the buffer!
    std::copy_n
      ( std::istream_iterator<nonrandomised::CADU>(std::cin)
      , index
      , std::ostream_iterator<nonrandomised::CADU>(std::cout)
      );

    /*
    std::ranges::copy
      ( std::ranges::views::take
        ( subrange<std::istream_iterator<nonrandomised::CADU>>
          ( std::istream_iterator<nonrandomised::CADU>(std::cin)
          , std::istream_iterator<nonrandomised::CADU>()
          )
        , index
        )
      , std::ostream_iterator<nonrandomised::CADU>(std::cout)
      );
    */

    /*
    std::ranges::copy
      ( std::ranges::subrange
        ( std::istream_iterator<nonrandomised::CADU>(std::cin)
        , std::istream_iterator<nonrandomised::CADU>()
        )
        | std::ranges::views::take(index)
      , std::ostream_iterator<nonrandomised::CADU>(std::cout)
      );
    */

  } else {
    std::vector<nonrandomised::CADU> buffer;
    // Fill all the CADUs into a buffer
    std::copy
      ( std::istream_iterator<nonrandomised::CADU>(std::cin)
      , std::istream_iterator<nonrandomised::CADU>()
      , std::back_insert_iterator<std::vector<nonrandomised::CADU>>(buffer)
      );

    // Output from the buffer
    std::copy_n
      ( buffer.begin()
      , buffer.size() - index
      , std::ostream_iterator<nonrandomised::CADU>(std::cout)
      );
  }
}
