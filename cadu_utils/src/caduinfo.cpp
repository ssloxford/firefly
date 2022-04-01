#include <iostream>
#include <cxxopts.hpp>
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

int main(int argc, char *argv[]) {
  cxxopts::Options options("caduinfo", "Displays the header contents of a CADU stream from stdin");
  options.add_options()
    ("h,help", "Print usage")
    ;

  auto result = options.parse(argc, argv);

  // Show help menu
  if (result.count("help")) {
    std::cerr << options.help() << std::endl;
    exit(0);
  }

  using namespace nonrandomised;
  CADU cadu;
  while (std::cin >> cadu) {
    std::cout << "version-number: " << cadu->version_number() << std::endl;
    std::cout << "scid: " << cadu->scid() << std::endl;
    std::cout << "vcid: " << cadu->vcid() << std::endl;
    std::cout << "vcdu-counter: " << cadu->vcdu_counter() << std::endl;
    std::cout << "replay-flag: " << cadu->replay_flag() << std::endl;
    std::cout << "vcdu-spare: " << cadu->vcdu_spare() << std::endl;
    std::cout << "m-pdu-spare: " << cadu->m_pdu_spare() << std::endl;
    std::cout << "first-header-pointer: " << cadu->first_header_pointer() << std::endl;
    std::cout << "checksum: ";
    print_checksum(cadu->checksum());
    std::cout << std::endl;
  }
}
