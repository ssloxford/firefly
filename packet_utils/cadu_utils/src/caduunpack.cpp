// Given a stream of CADUs on stdin, extracts and outputs the stream of CCSDS packets on stdout

#include <cmath>
#include <iostream>
#include <cxxopts.hpp>

#include "libcadu/libcadu.h"


void report_discarded_bytes(const int discarded_cadus, const int discarded_bytes) {
  if (discarded_cadus == 1) {
    std::cerr << "First CADU contained no first header and was discarded" << '\n';
  } else if (discarded_cadus > 1) {
    std::cerr << "First " << discarded_cadus << " CADUs contained no first header and were discarded" << '\n';
  }

  if (discarded_bytes > 0) {
    if (discarded_cadus == 0) {
      std::cerr << "First CADU contained " << discarded_bytes << "bytes before first-header-pointer which were discarded" << '\n';
    } else {
      std::cerr << "The subsequent CADU contained " << discarded_bytes << "bytes before first-header-pointer which were discarded" << '\n';

    }
  }
}

int main(int argc, char *argv[]) {
  cxxopts::Options options("caduunpack", "Unpack a CADU stream from stdin to stdout");
  options.add_options()
    (
      "m,mode",
      "Choose between raw byte stream and CCSDS packet mode. raw unpacks all bytes from all the input CADUs.  ccsds discards prefixing bytes from a previous packet - raw|ccsds",
      cxxopts::value<std::string>()->default_value("raw")
    )
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
  if (mode != "raw" && mode != "ccsds") {
    std::cerr << "Error: mode must be either \"raw\", or \"ccsds\"" << '\n';
    valid = false;
  }

  if (!valid) {
    std::cerr << "Quitting..." << '\n';
    exit(1);
  }

  using namespace nonrandomised;
  CADU cadu;

  if (mode == "raw") {
    // Unpack all the bytes within the CADU
    while (std::cin >> cadu) {
      for (auto&& it : cadu->data()) {
        std::cout << static_cast<const uint8_t>(it);
      }
    }
  } else if (mode == "ccsds") {

    bool first_cadu = true;
    int discarded_bytes = 0;
    int discarded_cadus = 0;
    bool discarded_bytes_reported = false;
    while (std::cin >> cadu) {
      // TODO: count fill packets
      // TODO: count packets without data

      if (first_cadu && cadu->first_header_pointer() != 0) {
        // No valid bytes have been decoded yet
        if (cadu->first_header_pointer() == std::pow(2, cadu::FIRST_HEADER_POINTER_LEN)-1) {
          // This CADU contains no header. Skip
          discarded_cadus++;
        }
        else {
          // This CADU contains bytes before the first header
          // Report any discarded bytes
          discarded_bytes = cadu->first_header_pointer();
          if (!discarded_bytes_reported) {
            report_discarded_bytes(discarded_cadus, discarded_bytes);
            discarded_bytes_reported = true;
          }

          // Output the remaining bytes
          for (auto&& it : cadu->data_header_aligned()) {
            std::cout << static_cast<const uint8_t>(it);
          }
          first_cadu = false;
        }
      } else {
        // Report any discarded bytes
        if (!discarded_bytes_reported) {
          report_discarded_bytes(discarded_cadus, discarded_bytes);
          discarded_bytes_reported = true;
        }

        for (auto&& it : cadu->data()) {
          std::cout << static_cast<const uint8_t>(it);
        }
        first_cadu = false;
      }
    }

    // Report any remaining discarded bytes
    if (!discarded_bytes_reported) {
      report_discarded_bytes(discarded_cadus, discarded_bytes);
      discarded_bytes_reported = true;  // May as well
    }
    
  } else {
    throw std::invalid_argument("Error: invalid mode: " + mode);
  }
}

