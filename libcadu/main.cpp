#include <iostream>
#include "libcadu.h"

std::ostream& operator<< (std::ostream& os, std::byte b) {
  return os << std::bitset<8>(std::to_integer<int>(b));
}

void print_checksum(std::array<std::byte, 128> checksum) {
  for (int i=0; i<sizeof(checksum); i++) {
    std::cerr << checksum[i] << " ";
  }
  std::cerr << "\n";
}

int main() {
  using namespace randomised;
  CADU cadu;
  std::cin >> cadu;
  std::cerr << "version_number: " << cadu->version_number() << "\n";
  std::cerr << "scid: " << cadu->scid() << "\n";
  std::cerr << "vcid: " << cadu->vcid() << "\n";
  std::cerr << "vcdu_counter: " << cadu->vcdu_counter() << "\n";
  std::cerr << "replay_flag: " << cadu->replay_flag() << "\n";
  std::cerr << "vcdu_spare: " << cadu->vcdu_spare() << "\n";
  std::cerr << "m_pdu_spare: " << cadu->m_pdu_spare() << "\n";
  std::cerr << "first_header_pointer: " << cadu->first_header_pointer() << "\n";
  std::cerr << "checksum: " << "\n";
  print_checksum(cadu->checksum());

  auto buffer = std::array<std::byte, 128> {};
  cadu.calculate_checksum(buffer);

  std::cerr << "valid checksum before recalculation: " << cadu.validate_checksum() << "\n";
  cadu.recalculate_checksum();
  std::cerr << "valid checksum after recalculation: " << cadu.validate_checksum() << "\n";
  std::cerr << "checksum: " << "\n";
  print_checksum(cadu->checksum());
  std::cout << cadu;
}
