#include <algorithm>
#include <iostream>
#include <cxxopts.hpp>

#include "libcadu/libcadu.h"

// TODO: select desired outputs through flags

std::ostream& operator<< (std::ostream& os, std::byte b) {
  std::hex(os);
  os << std::to_integer<int>(b);
  std::dec(os);
  return os;
}

void print_checksum(std::array<std::byte, 128> checksum, long unsigned int n_bytes) {
  for (int i=0; i<std::min(n_bytes, sizeof(checksum)); i++) {
    std::cout << checksum[i];
  }
}

int main(int argc, char *argv[]) {
  cxxopts::Options options("caduinfo", "Displays the header contents of a CADU stream from stdin");
  options.add_options()
    ("h,help", "Print usage")
    ;

  auto result = options.parse(argc, argv);

  // Show help menu
  if (result.count("help")) {
    std::cerr << options.help() << '\n';
    exit(0);
  }

  nonrandomised::CADU cadu;
  std::cerr << "version-number\tscid\tvcid\tvcdu-counter\treplay-flag\tvcdu-spare\tm-pdu-spare\tfirst-header-pointer\tchecksum" << '\n';
  while (std::cin >> cadu) {
    std::cout << cadu->version_number() << "\t\t"
              << cadu->scid() << "\t"
              << cadu->vcid() << "\t"
              << cadu->vcdu_counter() << "\t\t"
              << cadu->replay_flag() << "\t\t"
              << cadu->vcdu_spare() << "\t\t"
              << cadu->m_pdu_spare() << "\t\t"
              << cadu->first_header_pointer() << "\t\t\t";
    print_checksum(cadu->checksum(), 5);
    std::cout << '\n';
  }
}
