#include <algorithm>
#include <iostream>
#include <cxxopts.hpp>
#include <math.h>

#include "libcadu/libcadu.h"
#include "cadu_constants.h"
#include "libccsds/libccsds.h"

std::ostream& operator<< (std::ostream& os, std::byte b) {
  return os << std::bitset<8>(std::to_integer<int>(b));
}

int main(int argc, char *argv[]) {
  cxxopts::Options options("cadupack", "Pack bytes from stdin into a CADU stream on stdout");
  options.add_options()
    (
      "m,mode",
      "Choose between raw byte stream and CCSDS packet mode - raw|ccsds",
      cxxopts::value<std::string>()->default_value("raw")
    )
    (
      "n,version-number", 
      "Set version number field - <int (0-"
        + std::to_string((int)pow(2, cadu::VERSION_NUMBER_LEN)-1)
        + ")>",
      cxxopts::value<int>()->default_value("0")
    )
    (
      "s,scid",
      "Set spacecraft ID field - terra|aqua|<int (0-"
        + std::to_string((int)pow(2, cadu::SCID_LEN)-1)
        + ")>",
      cxxopts::value<std::string>()->default_value("0")
    )
    (
      "i,vcid",
      "Set virtual channel (instrument) field - aqua_gbad|aqua_ceres_10|aqua_ceres_15|aqua_amsu_20|aqua_amsu_25|aqua_modis|aqua_airs|aqua_amsr|aqua_hsb|<int (0-"
        + std::to_string((int)pow(2, cadu::VCID_LEN)-1)
        + ")>",
      cxxopts::value<std::string>()->default_value("0")
    )
    (
      "c,vcdu-counter",
      "Set VCDU starting counter, incrementing for each consecutive CADU - <int (0-"
        + std::to_string((int)pow(2, cadu::VCDU_COUNTER_LEN)-1)
        + ")>",
      cxxopts::value<int>()->default_value("0")
    )
    (
      "r,replay-flag",
      "Set replay flag - <int(0-"
        + std::to_string((int)pow(2, cadu::REPLAY_FLAG_LEN)-1)
        + ")>",
      cxxopts::value<int>()->default_value("0")
    )
    (
      "vcdu-spare",
      "Set virtual channel data unit spare bits - <int (0-"
        + std::to_string((int)pow(2, cadu::VCDU_SPARE_LEN)-1)
        + ")>",
      cxxopts::value<int>()->default_value("0")
    )
    (
      "m-pdu-spare",
      "Set multiplexing protocol data unit spare bits - <int (0-"
        + std::to_string((int)pow(2, cadu::M_PDU_SPARE_LEN)-1)
        + ")>",
      cxxopts::value<int>()->default_value("0")
    )
    (
      "p,first-header-pointer",
      "Sets the pointer to the first CCSDS header within the data. In raw mode, this sets the pointer for all CADUs. In ccsds mode, this sets the pointer for the first CADU only - <int (0-"
        + std::to_string((int)pow(2, cadu::FIRST_HEADER_POINTER_LEN)-1)
        + ")>",
      cxxopts::value<int>()->default_value("0")
    )
    ("h,help", "Print usage")
    ;

  auto result = options.parse(argc, argv);

  // Show help menu
  if (result.count("help")) {
    std::cerr << options.help() << std::endl;
    exit(0);
  }

  // Validate arguments
  bool valid = true;

  auto mode = result["mode"].as<std::string>();
  if (mode != "raw" && mode != "ccsds") {
    std::cerr << "Error: mode must be either \"raw\", or \"ccsds\"" << std::endl;
    valid = false;
  }

  if (result["version-number"].as<int>() >= pow(2, cadu::VERSION_NUMBER_LEN)) {
    std::cerr << "Error: version-number must be between 0 and " << pow(2, cadu::VERSION_NUMBER_LEN)-1 << std::endl;
    valid = false;
  }

  int scid;
  try{
    scid = SCIDs.at(result["scid"].as<std::string>());
  }
  catch(std::out_of_range) {
    try {
      scid = std::stoi(result["scid"].as<std::string>());
      if (scid >= pow(2, cadu::SCID_LEN)) {
        std::cerr << "Error: scid must be between 0 and " << pow(2, cadu::SCID_LEN)-1 << std::endl;
        valid = false;
      }
    }
    catch(std::invalid_argument) {
      // TODO: create this dynamically at compile time
      std::cerr << "Error: scid must be either \"terra\", \"aqua\", or an int" << std::endl;
      valid = false;
    }
    catch(std::out_of_range) {
      std::cerr << "Error: scid out of range" << std::endl;
      valid = false;
    }
  }

  int vcid;
  try {
    vcid = VCIDs.at(result["vcid"].as<std::string>());
  }
  catch(std::out_of_range) {
    // There was no VCID with this name
    try {
      // Attempt to use the ID as an integer
      vcid = std::stoi(result["vcid"].as<std::string>());
      if (vcid >= pow(2, cadu::VCID_LEN)) {
        std::cerr << "Error: vcid must be between 0 and " << pow(2, cadu::VCID_LEN)-1 << std::endl;
        valid = false;
      }
    }
    catch(std::invalid_argument) {
      // TODO: create this dynamically at compile time
      std::cerr << "Error: vcid must be either \"aqua_gbad\", \"aqua_ceres_10\", \"aqua_ceres_15\", \"aqua_amsu_20\", \"aqua_amsu_25\", \"aqua_modis\", \"aqua_airs\", \"aqua_amsr\", \"aqua_hsb\", or an int" << std::endl;
      valid = false;
    }
    catch(std::out_of_range) {
      std::cerr << "Error: vcid out of range" << std::endl;
      valid = false;
    }
  }

  if (result["vcdu-counter"].as<int>() >= pow(2, cadu::VCDU_COUNTER_LEN)) {
    std::cerr << "Error: vcdu-counter must be between 0 and " << pow(2, cadu::VCDU_COUNTER_LEN)-1 << std::endl;
    valid = false;
  }

  if (result["replay-flag"].as<int>() >= pow(2, cadu::REPLAY_FLAG_LEN)) {
    std::cerr << "Error: replay-flag must be between 0 and " << pow(2, cadu::REPLAY_FLAG_LEN)-1 << std::endl;
    valid = false;
  }

  if (result["vcdu-spare"].as<int>() >= pow(2, cadu::VCDU_SPARE_LEN)) {
    std::cerr << "Error: vcdu-spare must be between 0 and " << pow(2, cadu::VCDU_SPARE_LEN)-1 << std::endl;
    valid = false;
  }

  if (result["m-pdu-spare"].as<int>() >= pow(2, cadu::M_PDU_SPARE_LEN)) {
    std::cerr << "Error: m-pdu-spare must be between 0 and " << pow(2, cadu::M_PDU_SPARE_LEN)-1 << std::endl;
    valid = false;
  }

  if (result["first-header-pointer"].as<int>() >= pow(2, cadu::FIRST_HEADER_POINTER_LEN)) {
    std::cerr << "Error: first-header-pointer must be between 0 and " << pow(2, cadu::FIRST_HEADER_POINTER_LEN)-1 << std::endl;
    valid = false;
  }

  if (!valid) {
    std::cerr << "Quitting..." << std::endl;
    exit(1);
  }

  // Turning on randomisation is out of scope
  // Users should use the `cadurandomise` program in their pipeline instead
  using namespace nonrandomised;

  auto buffer = std::remove_cvref_t<decltype(std::declval<CADU>()->data())>();
  auto vcdu_counter = result["vcdu-counter"].as<int>();

  CADU cadu;
  cadu.get_mutable().version_number() = result["version-number"].as<int>();
  cadu.get_mutable().scid() = scid;
  cadu.get_mutable().vcid() = vcid;
  cadu.get_mutable().vcdu_counter() = vcdu_counter;
  cadu.get_mutable().replay_flag() = result["replay-flag"].as<int>();
  cadu.get_mutable().vcdu_spare() = result["vcdu-spare"].as<int>();
  cadu.get_mutable().m_pdu_spare() = result["m-pdu-spare"].as<int>();
  cadu.get_mutable().first_header_pointer() = result["first-header-pointer"].as<int>();

  if (mode == "raw") {
    while (std::cin.read(reinterpret_cast<char*>(buffer.data()), cadu::DATA_LEN) || std::cin.gcount() > 0) {
      if (std::cin.gcount() < cadu::DATA_LEN) {
        // If insufficient characters read, pad with zeros
        std::fill(buffer.begin() + std::cin.gcount(), buffer.end(), std::byte{0});
      }

      cadu.get_mutable().data() = buffer;
      std::cout << cadu;

      ++vcdu_counter; // TODO: roll over correctly
    }
  } else if (mode == "ccsds") {
    CCSDSPacket packet;
    int next_header_pointer = result["first-header-pointer"].as<int>();

    cadu.get_mutable().first_header_pointer() = next_header_pointer;

    std::cerr << "ccsds mode" << std::endl;

    // TODO: zero out unused parts of the CADU
    // TODO: next_header_pointer -> next_header_offset

    while (std::cin >> packet) {
      /*
      for (auto x: packet) {
        std::cerr << x;
        std::cerr << "\n";
      }
      std::terminate();
      */
      
      std::cerr << "found packet of size " << packet.size() << std::endl;
      int packet_offset = 0; // The number of bytes of the packet that have been copied in so far
      while (packet_offset != packet.size()) {
        if (packet.size() - packet_offset <= cadu::DATA_LEN - next_header_pointer) {
          // The entire remaining packet fits within the CADU
          std::cerr << "packet fits within the CADU" << std::endl;
          std::copy(
            packet.begin() + packet_offset,
            packet.end(),
            buffer.begin() + next_header_pointer);

          next_header_pointer += packet.size();
          packet_offset = packet.size();
        } else {
          std::cerr << "packet does not fit within the CADU" << std::endl;
          if (cadu::DATA_LEN - next_header_pointer > 0) {
            // There is still space in the CADU

            std::copy_n(
              packet.begin() + packet_offset,
              cadu::DATA_LEN - next_header_pointer,
              buffer.begin() + next_header_pointer);

            packet_offset += cadu::DATA_LEN - next_header_pointer;
          }

          // The CADU is now entirely full - output it
          cadu.get_mutable().data() = buffer;
          std::cout << cadu;
          std::cerr << "outputting full cadu" << std::endl;

          // Create a "new" cadu
          next_header_pointer = 0;
          if (packet.size() - packet_offset > cadu::DATA_LEN) {
            // The packet is going to fill up the entire next CADU as well
            // so there will be no first header
            cadu.get_mutable().first_header_pointer() = std::pow(2, cadu::FIRST_HEADER_POINTER_LEN)-1;
          } else {
            // The packet will be contained in the next CADU
            // so the next header will be in the next CADU
            // TODO: this may not be true for the final CADU
            cadu.get_mutable().first_header_pointer() = packet.size() - packet_offset;
          }
        }
      }
    }

    if (next_header_pointer > 0) {
      // Buffer with zeros
      std::fill(buffer.begin() + next_header_pointer, buffer.end(), std::byte{0});

      // Output the cadu
      cadu.get_mutable().data() = buffer;
      std::cout << cadu;
      std::cerr << "outputting partial cadu" << std::endl;
    }
    
  } else {
    throw std::invalid_argument("Error: invalid mode: " + mode);
  }
}
