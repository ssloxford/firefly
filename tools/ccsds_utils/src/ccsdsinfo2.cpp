#include <iostream>
#include <cxxopts.hpp>

#include "libccsds/libccsds.h"
#include "libgiis/libgiis.h"

// TODO: select desired outputs through flags

int main(int argc, char *argv[]) {
  cxxopts::Options options("ccsdsinfo", "Displays the header contents of a CCSDS packet stream from stdin");
  options.add_options()
    (
      "m,mode",

      "Select the bit packing structure of the secondary header and data fields - none|modis",
      cxxopts::value<std::string>()->default_value("none")
    )
    ("v,verbose", "Warn on non-fatal decoding errors")
    ("h,help", "Print usage")
    ;

  auto result = options.parse(argc, argv);

  // Show help menu
  if (result.count("help")) {
    std::cerr << options.help() << '\n';
    exit(0);
  }

  // Validate arguments
  bool valid = true;

  auto mode = result["mode"].as<std::string>();
  if (mode != "none" && mode != "modis") {
    std::cerr << "Error: mode must be either \"none\", or \"modis\"" << '\n';
    valid = false;
  }

  if (!valid) {
    std::cerr << "Quitting..." << '\n';
    exit(1);
  }

  if (mode == "none") {
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

  } else if (mode == "modis") {
    CCSDSPacket<giis::SecondaryHeader, giis::DataField> packet;
    std::cerr << "version-number\ttype-flag\tsec-hdr-flag\tapp-id\tseq-flags\tseq-cnt-or-name\tdata-len" << '\n';
    bool done = false;
    while (!done) {
      try {
        if (! std::cin >> packet) {
          done = true;
          continue;
        }
      } catch (const std::invalid_argument& e) {
        if (result.count("verbose") {
          std::cerr << e.what();
        }
        continue;
      }
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
}
