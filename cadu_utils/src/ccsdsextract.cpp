#include <iostream>
#include "libccsds/libccsds.h"

std::ostream& operator<< (std::ostream& os, std::byte b) {
  return os << std::bitset<8>(std::to_integer<int>(b));
}

int main() {
  CCSDSPacket packet;
  while (std::cin >> packet) {
    std::cout << packet.data()().data;
  }
}
