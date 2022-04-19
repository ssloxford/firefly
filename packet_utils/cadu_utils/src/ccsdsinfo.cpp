#include <iostream>
#include <cxxopts.hpp>

#include "libccsds/libccsds.h"

// TODO: select desired outputs through flags

int main(int argc, char *argv[]) {
  cxxopts::Options options("ccsdsinfo", "Displays the header contents of a CCSDS packet stream from stdin");
  options.add_options()
    ("h,help", "Print usage")
    ;

  auto result = options.parse(argc, argv);

  // Show help menu
  if (result.count("help")) {
    std::cerr << options.help() << '\n';
    exit(0);
  }

  CCSDSPacket packet;
  std::cerr << "version-number\ttype-flag\tsec-hdr-flag\tapp-id\tseq-flags\tseq-cnt-or-name\tdata-len" << '\n';
  while (std::cin >> packet) {
    std::cout << packet.version_number() << "\t\t" \
              << packet.type() << "\t\t" \
              << packet.sec_hdr_flag() << "\t\t" \
              << packet.app_id() << "\t" \
              << packet.seq_flags() << "\t\t" \
              << packet.seq_cnt_or_name() << "\t\t" \
              << packet.data_len() \
              << '\n';
  }
}
