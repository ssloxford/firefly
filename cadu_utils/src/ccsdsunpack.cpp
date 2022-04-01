#include <iostream>
#include <cxxopts.hpp>

#include "libccsds/libccsds.h"

std::ostream& operator<< (std::ostream& os, std::byte b) {
  return os << std::bitset<8>(std::to_integer<int>(b));
}

int main(int argc, char *argv[]) {
  cxxopts::Options options("ccsdsunpack", "Unpack a CCSDS packet stream from stdin to stdout");
  options.add_options()
    ("h,help", "Print usage")
    ;

  auto result = options.parse(argc, argv);

  // Show help menu
  if (result.count("help")) {
    std::cerr << options.help() << std::endl;
    exit(0);
  }

  CCSDSPacket packet;
  while (std::cin >> packet) {
    std::cout << packet.data().data;
  }
}
