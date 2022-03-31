// caduinfo - given a CADU stream on stdin, prints information about its contents

#include <iostream>
#include "libcadu/libcadu.h"

std::ostream& operator<< (std::ostream& os, std::byte b) {
  std::hex(os);
  os << std::to_integer<int>(b);
  std::dec(os);
  return os;
}

void print_checksum(std::array<std::byte, 128> checksum) {
  for (int i=0; i<sizeof(checksum); i++) {
    std::cout << checksum[i] << " ";
  }
  std::cout << "\n";
}

int main() {
  using namespace nonrandomised;
  CADU cadu;
  while (std::cin >> cadu) {
    std::cout << "version_number: " << cadu->version_number() << std::endl;
    std::cout << "scid: " << cadu->scid() << std::endl;
    std::cout << "vcid: " << cadu->vcid() << std::endl;
    std::cout << "vcdu_counter: " << cadu->vcdu_counter() << std::endl;
    std::cout << "replay_flag: " << cadu->replay_flag() << std::endl;
    std::cout << "vcdu_spare: " << cadu->vcdu_spare() << std::endl;
    std::cout << "m_pdu_spare: " << cadu->m_pdu_spare() << std::endl;
    std::cout << "first_header_pointer: " << cadu->first_header_pointer() << std::endl;
    std::cout << "checksum: ";
    print_checksum(cadu->checksum());
    std::cout << std::endl;
  }
}
