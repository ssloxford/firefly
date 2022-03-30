// Given a stream of CADUs on stdin, extracts and outputs the stream of CCSDS packets on stdout

#include <iostream>
#include "libcadu/libcadu.h"

std::ostream& operator<< (std::ostream& os, std::byte b) {
  return os << std::bitset<8>(std::to_integer<int>(b));
}

int main() {
  using namespace randomised;
  CADU cadu;

  bool first_cadu = true;
  while (std::cin >> cadu) {
    // TODO: count fill packets
    // TODO: count packets without data

    if (first_cadu) {
      if (cadu->first_header_pointer() != 0) {
        std::cerr << "First CADU contained " << cadu->first_header_pointer()
                  << " bytes from previous packet" << std::endl;
      }

      for (auto&& it : cadu->data_header_aligned()) {
        std::cout << static_cast<const uint8_t>(it);
      }

      first_cadu = false;
    } else {
      for (auto&& it : cadu->data()) {
        std::cout << static_cast<const uint8_t>(it);
      }
    }
  }
}
