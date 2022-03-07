#include <iostream>
#include "libcadu/libcadu.h"
#include "cadu_constants.h"

std::ostream& operator<< (std::ostream& os, std::byte b) {
  return os << std::bitset<8>(std::to_integer<int>(b));
}

int main() {
  // TODO: optionally turn on and off randomisation
  using namespace randomised;
  // auto buffer = std::make_unique_for_overwrite<char[]>(CADU_DATA_LEN);
  auto buffer = std::remove_cvref_t<decltype(std::declval<CADU>()->data())>();

  while (std::cin.read(reinterpret_cast<char*>(buffer.data()), CADU_DATA_LEN)) {
    std::cerr << "outputting frame" << std::endl;
    std::cerr << "buffer size " << CADU_DATA_LEN << ":" << std::endl;

    /*
    for (int i = 0; i < CADU_DATA_LEN; i++) {
      std::cerr << buffer[i] << " ";
    }
    */
   
    CADU cadu;
    cadu.get_mutable().version_number() = 1;
    cadu.get_mutable().scid() = SCID_AQUA;
    cadu.get_mutable().vcid() = VCID_AQUA_MODIS;
    // TODO: set sequence number

    // Convert char array to std::byte array
    cadu.get_mutable().data() = buffer;
    std::cout << cadu;
  }
}
