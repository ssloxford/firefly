#include <iostream>
#include <cxxopts.hpp>

#include "libccsds/libccsds.h"

int main(int argc, char *argv[]) {
  cxxopts::Options options("ccsdsinfo", "Displays the header contents of a CCSDS packet stream from stdin");
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
    std::cout << "version-number: " << packet.version_number() << "\n";
    std::cout << "type-flag: " << packet.type() << "\n";
    std::cout << "sec-hdr-flag: " << packet.sec_hdr_flag() << "\n";
    std::cout << "app-id: " << packet.app_id() << "\n";
    std::cout << "seq-flags: " << packet.seq_flags() << "\n";
    std::cout << "seq-cnt-or-name: " << packet.seq_cnt_or_name() << "\n";
    std::cout << "data-len: " << packet.data_len() << "\n";
    std::cout << std::endl;
  }
}
