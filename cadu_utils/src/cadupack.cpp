#include <iostream>
#include <cxxopts.hpp>
#include <math.h>

#include "libcadu/libcadu.h"
#include "cadu_constants.h"

std::ostream& operator<< (std::ostream& os, std::byte b) {
  return os << std::bitset<8>(std::to_integer<int>(b));
}

// TODO: implement CCSDS mode

int main(int argc, char *argv[]) {
  cxxopts::Options options("cadupack", "Pack bytes from stdin into a CADU");
  options.add_options()
    (
      "n,version_number", 
      "Set version number field - <int (0-"
        + std::to_string((int)pow(2, VERSION_NUMBER_LEN)-1)
        + ")>",
      cxxopts::value<int>()->default_value("0")
    )
    (
      "s,scid",
      "Set spacecraft ID field - terra|aqua|<int (0-"
        + std::to_string((int)pow(2, SCID_LEN)-1)
        + ")>",
      cxxopts::value<std::string>()->default_value("0")
    )
    (
      "i,vcid",
      "Set virtual channel (instrument) field - aqua_gbad|aqua_ceres_10|aqua_ceres_15|aqua_amsu_20|aqua_amsu_25|aqua_modis|aqua_airs|aqua_amsr|aqua_hsb|<int (0-"
        + std::to_string((int)pow(2, VCID_LEN)-1)
        + ")>",
      cxxopts::value<std::string>()->default_value("0")
    )
    (
      "c,vcdu_counter",
      "Set VCDU starting counter, incrementing for each consecutive CADU - <int (0-"
        + std::to_string((int)pow(2, VCDU_COUNTER_LEN)-1)
        + ")>",
      cxxopts::value<int>()->default_value("0")
    )
    (
      "r,replay_flag",
      "Set replay flag - <int(0-"
        + std::to_string((int)pow(2, REPLAY_FLAG_LEN)-1)
        + ")>",
      cxxopts::value<int>()->default_value("0")
    )
    (
      "vcdu_spare",
      "Set virtual channel data unit spare bits - <int (0-"
        + std::to_string((int)pow(2, VCDU_SPARE_LEN)-1)
        + ")>",
      cxxopts::value<int>()->default_value("0")
    )
    (
      "m_pdu_spare",
      "Set multiplexing protocol data unit spare bits - <int (0-"
        + std::to_string((int)pow(2, M_PDU_SPARE_LEN)-1)
        + ")>",
      cxxopts::value<int>()->default_value("0")
    )
    (
      "p,first_header_pointer",
      "Sets the pointer to the first CCSDS header within the data - <int (0-"
        + std::to_string((int)pow(2, FIRST_HEADER_POINTER_LEN)-1)
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

  if (result["version_number"].as<int>() >= pow(2, VERSION_NUMBER_LEN)) {
    std::cerr << "Error: version_number must be between 0 and " << pow(2, VERSION_NUMBER_LEN)-1 << std::endl;
    valid = false;
  }

  int scid;
  try{
    scid = SCIDs.at(result["scid"].as<std::string>());
  }
  catch(std::out_of_range) {
    try {
      scid = std::stoi(result["scid"].as<std::string>());
      if (scid >= pow(2, SCID_LEN)) {
        std::cerr << "Error: scid must be between 0 and " << pow(2, SCID_LEN)-1 << std::endl;
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
      if (vcid >= pow(2, VCID_LEN)) {
        std::cerr << "Error: vcid must be between 0 and " << pow(2, VCID_LEN)-1 << std::endl;
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

  if (result["vcdu_counter"].as<int>() >= pow(2, VCDU_COUNTER_LEN)) {
    std::cerr << "Error: vcdu_counter must be between 0 and " << pow(2, VCDU_COUNTER_LEN)-1 << std::endl;
    valid = false;
  }

  if (result["replay_flag"].as<int>() >= pow(2, REPLAY_FLAG_LEN)) {
    std::cerr << "Error: replay_flag must be between 0 and " << pow(2, REPLAY_FLAG_LEN)-1 << std::endl;
    valid = false;
  }

  if (result["vcdu_spare"].as<int>() >= pow(2, VCDU_SPARE_LEN)) {
    std::cerr << "Error: vcdu_spare must be between 0 and " << pow(2, VCDU_SPARE_LEN)-1 << std::endl;
    valid = false;
  }

  if (result["m_pdu_spare"].as<int>() >= pow(2, M_PDU_SPARE_LEN)) {
    std::cerr << "Error: m_pdu_spare must be between 0 and " << pow(2, M_PDU_SPARE_LEN)-1 << std::endl;
    valid = false;
  }

  if (result["first_header_pointer"].as<int>() >= pow(2, FIRST_HEADER_POINTER_LEN)) {
    std::cerr << "Error: first_header_pointer must be between 0 and " << pow(2, FIRST_HEADER_POINTER_LEN)-1 << std::endl;
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
  auto vcdu_counter = result["vcdu_counter"].as<int>();

  while (std::cin.read(reinterpret_cast<char*>(buffer.data()), CADU_DATA_LEN) || std::cin.gcount() > 0) {
    std::cerr << "Characters extracted: " << std::cin.gcount() << std::endl;
    if (std::cin.gcount() < CADU_DATA_LEN) {
      // If insufficient characters read, pad with zeros
      std::fill(buffer.begin() + std::cin.gcount(), buffer.end(), std::byte{0});
    }
    CADU cadu;
    cadu.get_mutable().version_number() = result["version_number"].as<int>();
    cadu.get_mutable().scid() = scid;
    cadu.get_mutable().vcid() = vcid;
    cadu.get_mutable().vcdu_counter() = vcdu_counter;
    cadu.get_mutable().replay_flag() = result["replay_flag"].as<int>();
    cadu.get_mutable().vcdu_spare() = result["vcdu_spare"].as<int>();
    cadu.get_mutable().m_pdu_spare() = result["m_pdu_spare"].as<int>();
    cadu.get_mutable().first_header_pointer() = result["first_header_pointer"].as<int>();
    cadu.get_mutable().data() = buffer;
    std::cout << cadu;

    ++vcdu_counter; // TODO: roll over correctly
  }
}
