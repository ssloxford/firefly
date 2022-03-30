#include <iostream>
#include <span>
#include "libcadu/libcadu.h"
#include "cadu_constants.h"

std::ostream& operator<< (std::ostream& os, std::byte b) {
  return os << std::bitset<8>(std::to_integer<int>(b));
}

int main() {
  // TODO: optionally turn on and off randomisation
  using namespace randomised;
  // auto buffer = std::make_unique_for_overwrite<char[]>(CADU_DATA_LEN);
  // auto buffer = std::remove_cvref_t<decltype(std::declval<CADU>()->data())>();

  int n_read = -1;
  char c;
  auto buffer = std::array<std::byte, CADU_DATA_LEN> {};

  while (n_read != 0) {
    n_read = 0;
    while (std::cin.get(c)) {
      if (n_read >= CADU_DATA_LEN) {
        break;
      }
      buffer[n_read] = std::byte(c);
      ++n_read;
    }

    std::cerr << "outputting frame" << std::endl;
    std::cerr << "buffer size " << n_read << "/" << CADU_DATA_LEN << std::endl;

    CADU cadu;
    cadu.get_mutable().version_number() = 1;
    cadu.get_mutable().scid() = SCID_AQUA;
    cadu.get_mutable().vcid() = VCID_AQUA_MODIS;
    cadu.get_mutable().vcdu_counter() = 6970478;
    // TODO: set sequence number
    // TODO: get_mutable().data_header_aligned() severely broken

    // Convert char array to std::byte array
    cadu.get_mutable().first_header_pointer() = CADU_DATA_LEN - n_read;
    // cadu.get_mutable().data_header_aligned() = std::;
    // cadu.get_mutable().data_header_aligned() = std::views::counted(buffer.begin(), n_read);
    cadu.get_mutable().data_header_aligned() = buffer;
    std::cout << cadu;
  }
}
