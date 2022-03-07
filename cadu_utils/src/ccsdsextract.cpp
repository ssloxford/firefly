#include <iostream>
#include "libccsds/libccsds.h"

int main() {
  CCSDSPacket packet;
  while (std::cin >> packet) {
    std::cout << packet.data();
  }
}
