// Packs a byte stream into CCSDS packets

// TODO: make this work for multiple packets

#include <iostream>
#include <ranges>
#include <cxxopts.hpp>
#include <math.h>

#include "libccsds/libccsds.h"
#include "ccsds_constants.h"

std::ostream& operator<< (std::ostream& os, std::byte b) {
  return os << std::bitset<8>(std::to_integer<int>(b));
}

int main(int argc, char *argv[]) {
  cxxopts::Options options("ccsdspack", "Pack bytes from stdin into a CCSDS packet stream on stdout");
  options.add_options()
    (
      "n,version-number",
      "Set version number field - <int (0-"
        + std::to_string((int)pow(2, ccsds::VERSION_NUMBER_LEN)-1)
        + ")>",
      cxxopts::value<int>()->default_value("0")
    )
    (
      "t,type-flag",
      "Set packet type flag - \"telemetry\"|\"telecommand\"|<int (0-"
        + std::to_string((int)pow(2, ccsds::TYPE_FLAG_LEN)-1)
        + ")>",
      cxxopts::value<std::string>()->default_value("0")
    )
    (
      "f,sec-hdr-flag",
      "Set secondary header flag. 1 indicates the presence of a secondary header, 0 its absence - <int (0-"
        + std::to_string((int)pow(2, ccsds::SEC_HDR_FLAG_LEN)-1)
        + ")>",
      cxxopts::value<int>()->default_value("0")
    )
    (
      "a,app-id",
      "Set application process id - \"aqua_modis\"|<int (0-"
        + std::to_string((int)pow(2, ccsds::APP_ID_LEN)-1)
        + ")>",
      cxxopts::value<std::string>()->default_value("0")
    )
    (
      "s,seq-flags",
      "Set sequence flags, incidating whether the packets' data is a segment of a larger set - \"first\"|\"last\"|\"continuation\"|\"unsegmented\"|<int (0-"
        + std::to_string((int)pow(2, ccsds::SEQ_FLAGS_LEN)-1)
        + ")>",
      cxxopts::value<std::string>()->default_value("0")
    )
    (
      "c,seq-cnt-or-name",
      "Set packet sequence count or packet name - <int (0-"
        + std::to_string((int)pow(2, ccsds::SEQ_CNT_OR_NAME_LEN)-1)
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

  if (result["version-number"].as<int>() >= pow(2, ccsds::VERSION_NUMBER_LEN)) {
    std::cerr << "Error: version-number must be between 0 and " << pow(2, ccsds::VERSION_NUMBER_LEN)-1 << std::endl;
    valid = false;
  }

  int type;
  if (result["type-flag"].as<std::string>() == "telemetry") {
    type = 0;
  } else if (result["type-flag"].as<std::string>() == "telecommand") {
    type = 1;
  } else {
    try {
      type = std::stoi(result["type-flag"].as<std::string>());
      if (type >= pow(2, ccsds::TYPE_FLAG_LEN)) {
        std::cerr << "Error: type must be between 0 and " << pow(2, ccsds::TYPE_FLAG_LEN)-1 << std::endl;
        valid = false;
      }
    }
    catch(std::invalid_argument) {
      std::cerr << "Error: type-flag must be either \"telemetry\", \"telecommand\", or <int (0-" << pow(2, ccsds::TYPE_FLAG_LEN)-1 << ")>" << std::endl;
      valid = false;
    }
    catch(std::out_of_range) {
      std::cerr << "Error: type-flag out of range" << std::endl;
      valid = false;
    }
  }

  if (result["sec-hdr-flag"].as<int>() >= pow(2, ccsds::SEC_HDR_FLAG_LEN)) {
    std::cerr << "Error: sec-hdr-flag must be between 0 and " << pow(2, ccsds::SEC_HDR_FLAG_LEN)-1 << std::endl;
    valid = false;
  }

  int app_id;
  try {
    app_id = APP_IDs.at(result["app-id"].as<std::string>());
  } catch (std::out_of_range) {
    try {
      app_id = std::stoi(result["app-id"].as<std::string>());
      if (app_id >= pow(2, ccsds::APP_ID_LEN)) {
        std::cerr << "Error: app-id must be between 0 and " << pow(2, ccsds::APP_ID_LEN)-1 << std::endl;
        valid = false;
      }
    }
    catch(std::invalid_argument) {
      // TODO: create this dynamically at compile time
      std::cerr << "Error: app-id must be either \"aqua_modis\", or an int" << std::endl;
      valid = false;
    }
    catch(std::out_of_range) {
      std::cerr << "Error: app-id out of range" << std::endl;
      valid = false;
    }
  }

  int seq_flags;
  if (result["seq-flags"].as<std::string>() == "first") {
    seq_flags = 1;
  } else if (result["seq-flags"].as<std::string>() == "last") {
    seq_flags = 2;
  } else if (result["seq-flags"].as<std::string>() == "continuation") {
    seq_flags = 0;
  } else if (result["seq-flags"].as<std::string>() == "unsegmented") {
    seq_flags = 3;
  } else {
    try {
      seq_flags = std::stoi(result["seq-flags"].as<std::string>());
      if (seq_flags >= pow(2, ccsds::TYPE_FLAG_LEN)) {
        std::cerr << "Error: seq-flags must be between 0 and " << pow(2, ccsds::TYPE_FLAG_LEN)-1 << std::endl;
        valid = false;
      }
    }
    catch(std::invalid_argument) {
      std::cerr << "Error: seq-flags must be either \"telemetry\", \"telecommand\", or <int (0-" << pow(2, ccsds::TYPE_FLAG_LEN)-1 << ")>" << std::endl;
      valid = false;
    }
    catch(std::out_of_range) {
      std::cerr << "Error: seq-flags out of range" << std::endl;
      valid = false;
    }
  }

  if (result["seq-cnt-or-name"].as<int>() >= pow(2, ccsds::SEQ_CNT_OR_NAME_LEN)) {
    std::cerr << "Error: seq-cnt-or-name must be between 0 and " << pow(2, ccsds::SEQ_CNT_OR_NAME_LEN)-1 << std::endl;
    valid = false;
  }


  auto buffer = std::vector<std::byte> {};
  buffer.resize(ccsds::MAX_DATA_LEN);

  int n_read = 0;
  char c;
  while (std::cin.get(c)) {
    if (n_read >= ccsds::MAX_DATA_LEN) {
      break;
    }
    buffer[n_read] = std::byte(c);
    ++n_read;
  }

  buffer.resize(n_read);

  CCSDSPacket packet;
  packet.data() = buffer;
  packet.version_number() = result["version-number"].as<int>();
  packet.type() = type;
  packet.sec_hdr_flag() = result["sec-hdr-flag"].as<int>();
  packet.app_id() = app_id;
  packet.seq_flags() = seq_flags;
  packet.seq_cnt_or_name() = 16003;
  std::cout << packet;
}
