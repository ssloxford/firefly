#include <iostream>
#include <cxxopts.hpp>

#include "libccsds/libccsds.h"
#include "libgiis/libgiis.h"

// TODO: select desired outputs through flags

int main(int argc, char *argv[]) {
  cxxopts::Options options("ccsdsinfo", "Displays the header contents of a CCSDS packet stream from stdin");
  options.add_options()
    ("v,verbose", "Warn on non-fatal decoding errors")
    ("h,help", "Print usage")
    ;

  auto result = options.parse(argc, argv);

  // Show help menu
  if (result.count("help")) {
    std::cerr << options.help() << '\n';
    exit(0);
  }

  CCSDSPacket<giis::SecondaryHeader, giis::DataField> packet;
  // CCSDSPacket packet;
  // int n = 0;
  while (std::cin >> packet) {
    //n++;
    //std::cerr << "outputting packet" << n << "," << packet.data_len() << '\n';
    /*
    for (int ifov=1; ifov<=5; ifov++) {
      // Mask out the infrared channels
      packet.data_field.data_word(ifov, 21) = 0;
      packet.data_field.data_word(ifov, 22) = 0;
    }
    */
    std::cout << packet << std::flush;

    //std::cerr << "end of loop" << std::endl;
  }
}
