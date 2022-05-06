#include <iostream>
#include <stdlib.h>    // rand
#include <algorithm>   // min
#include <cxxopts.hpp>

#include "libccsds/libccsds.h"
#include "libgiis/libgiis.h"

// TODO: select desired outputs through flags

int main(int argc, char *argv[]) {
  cxxopts::Options options("ccsdsinfo", "Displays the header contents of a CCSDS packet stream from stdin");
  options.add_options()
    ("v,verbose", "Warn on non-fatal decoding errors")
    ("h,help", "Print usage")
    ("m,mask", "Mask out the following channel with a uniform value (default 0, change using -V)", cxxopts::value<std::vector<int>>()->default_value("-1"))
    ("M,mask-value", "Value to which masked channels are set", cxxopts::value<int>()->default_value("0"))
    ("r,randomize", "Set the following channel to random values", cxxopts::value<std::vector<int>>()->default_value("-1"))
    ("R,random-max", "Set \"randomize\" channels to values in the range [0,M)", cxxopts::value<int>()->default_value("100"))
    ("c,cap", "Cap the following channel to a maximum of C", cxxopts::value<std::vector<int>>()->default_value("-1"))
    ("C,cap-max", "Cap the \"cap\" channels to a maximum of this value", cxxopts::value<int>()->default_value("100"))
    ;

  auto result = options.parse(argc, argv);

  auto mask = result["mask"].as<std::vector<int>>();
  int mask_value = result["mask-value"].as<int>();

  auto randomize = result["randomize"].as<std::vector<int>>();
  int random_max = result["random-max"].as<int>();

  auto cap = result["cap"].as<std::vector<int>>();
  int cap_max = result["cap-max"].as<int>();

  // Show help menu
  if (result.count("help")) {
    std::cerr << options.help() << '\n';
    exit(0);
  }

  CCSDSPacket<giis::SecondaryHeader, giis::DataField> packet;
  while (std::cin >> packet) {
    // Only set earth data IR fields to zero
    if (packet.data_field.src_ident_type() == 0 && packet.data_field.frame_data_count() != 0) {
      for (int ifov=1; ifov<=5; ifov++) {
        // Mask out the selected channels
        for (auto m : mask) {
          if (m >= 0) {
            packet.data_field.data_word(ifov, m) = mask_value;
          }
        }
        for (auto r : randomize) {
          if (r >= 0) {
            int rand_value = rand() % random_max;
            packet.data_field.data_word(ifov, r) = rand_value;
          }
        }
        for (auto c : cap) {
          if (c >= 0) {
            int data_word = packet.data_field.data_word(ifov, c);
            packet.data_field.data_word(ifov, c) = std::min(data_word, cap_max);
          }
        }
        // Mask out the infrared channels
        //packet.data_field.data_word(ifov, 21) = 0;
        //packet.data_field.data_word(ifov, 22) = 0;
      }
    }

    // TODO: when the segfaulting bug on operator >> is fixed, remove the flush
    std::cout << packet << std::flush;
  }
}
