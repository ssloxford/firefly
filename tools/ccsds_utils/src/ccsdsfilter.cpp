#include <iostream>
#include <ranges>
#include <numeric>
#include <functional>
#include <tuple>
#include <vector>
#include <cxxopts.hpp>
#include <math.h>

#include "libccsds/libccsds.h"
#include "ccsds_constants.h"

std::ostream& operator<< (std::ostream& os, std::byte b) {
  return os << std::bitset<8>(std::to_integer<int>(b));
}

template <typename T>
auto get_result(cxxopts::OptionValue arg) {
  try {
    return arg.as<T>();
  } catch(cxxopts::option_has_no_value_exception) {
    return T{};
  }
}

/*
template <typename T, typename U>
auto validate_args(
  std::vector<T> &values,
  std::function<std::tuple<bool,U>(T)> validate) {
  return std::transform_reduce(
    values.begin(),
    values.end(),
    std::make_tuple(true, std::vector<U>{}),
    [](std::tuple<bool,U> t, std::tuple<bool,std::vector<U>> &acc) {
      return std::make_tuple<bool,std::vector<U>>(
        std::get<0>(t) & std::get<0>(acc),
        std::get<1>(acc).emplace_back(std::get<1>(t))
      );
    },
    // std::logical_and<bool>{},
    validate);
}
*/

int main(int argc, char *argv[]) {
  cxxopts::Options options("ccsdsfilter", "Filter CCSDS packets that match any given selector from stdin to stdout");
  options.add_options()
    (
      "n,version-number",
      "Select version number - <int (0-"
        + std::to_string((int)pow(2, ccsds::VERSION_NUMBER_LEN)-1)
        + ")>",
      cxxopts::value<std::vector<int>>()
    )
    (
      "N,except-version-number",
      "Select all except version number - <int (0-"
        + std::to_string((int)pow(2, ccsds::VERSION_NUMBER_LEN)-1)
        + ")>",
      cxxopts::value<std::vector<int>>()
    )
    (
      "t,type-flag",
      "Select packet type flag - \"telemetry\"|\"telecommand\"|<int (0-"
        + std::to_string((int)pow(2, ccsds::TYPE_FLAG_LEN)-1)
        + ")>",
      cxxopts::value<std::vector<std::string>>()
    )
    (
      "T,except-type-flag",
      "Select all except packet type flag - \"telemetry\"|\"telecommand\"|<int (0-"
        + std::to_string((int)pow(2, ccsds::TYPE_FLAG_LEN)-1)
        + ")>",
      cxxopts::value<std::vector<std::string>>()
    )
    (
      "f,sec-hdr-flag",
      "Select secondary header flag - <int (0-"
        + std::to_string((int)pow(2, ccsds::SEC_HDR_FLAG_LEN)-1)
        + ")>",
      cxxopts::value<std::vector<int>>()
    )
    (
      "F,except-sec-hdr-flag",
      "Select all except secondary header flag - <int (0-"
        + std::to_string((int)pow(2, ccsds::SEC_HDR_FLAG_LEN)-1)
        + ")>",
      cxxopts::value<std::vector<int>>()
    )
    (
      "a,app-id",
      "Select application process id - \"aqua_modis\"|<int (0-"
        + std::to_string((int)pow(2, ccsds::APP_ID_LEN)-1)
        + ")>",
      cxxopts::value<std::vector<std::string>>()
    )
    (
      "A,except-app-id",
      "Select all except application process id - \"aqua_modis\"|<int (0-"
        + std::to_string((int)pow(2, ccsds::APP_ID_LEN)-1)
        + ")>",
      cxxopts::value<std::vector<std::string>>()
    )
    (
      "s,seq-flags",
      "Select sequence flags - \"first\"|\"last\"|\"continuation\"|\"unsegmented\"|<int (0-"
        + std::to_string((int)pow(2, ccsds::SEQ_FLAGS_LEN)-1)
        + ")>",
      cxxopts::value<std::vector<std::string>>()
    )
    (
      "S,except-seq-flags",
      "Select all except sequence flags, incidating whether the packets' data is a segment of a larger set - \"first\"|\"last\"|\"continuation\"|\"unsegmented\"|<int (0-"
        + std::to_string((int)pow(2, ccsds::SEQ_FLAGS_LEN)-1)
        + ")>",
      cxxopts::value<std::vector<std::string>>()
    )
    (
      "c,seq-cnt-or-name",
      "Select packet sequence count or packet name - <int (0-"
        + std::to_string((int)pow(2, ccsds::SEQ_CNT_OR_NAME_LEN)-1)
        + ")>",
      cxxopts::value<std::vector<int>>()
    )
    (
      "C,except-seq-cnt-or-name",
      "Select packet sequence count or packet name - <int (0-"
        + std::to_string((int)pow(2, ccsds::SEQ_CNT_OR_NAME_LEN)-1)
        + ")>",
      cxxopts::value<std::vector<int>>()
    )
    ( "i,invert", "Invert selection" )
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

  auto version_numbers = get_result<std::vector<int>>(result["version-number"]);
  for (auto v : version_numbers) {
    if (v >= pow(2, ccsds::VERSION_NUMBER_LEN)) {
      std::cerr << "Error: version-number must be between 0 and " << pow(2, ccsds::VERSION_NUMBER_LEN)-1 << '\n';
      valid = false;
    }
  }

  auto except_version_numbers = get_result<std::vector<int>>(result["version-number"]);
  for (auto v : except_version_numbers) {
    if (v >= pow(2, ccsds::VERSION_NUMBER_LEN)) {
      std::cerr << "Error: except-version-number must be between 0 and " << pow(2, ccsds::VERSION_NUMBER_LEN)-1 << '\n';
      valid = false;
    }
  }

  std::vector<int> types = {};
  for (auto v : get_result<std::vector<std::string>>(result["type-flag"])) {
    if (v == "telemetry") {
      types.emplace_back(0);
    } else if (v == "telecommand") {
      types.emplace_back(1);
    } else {
      try {
        types.emplace_back(std::stoi(v));
        if (types.back() >= pow(2, ccsds::TYPE_FLAG_LEN)) {
          std::cerr << "Error: type-flag must be between 0 and " << pow(2, ccsds::TYPE_FLAG_LEN)-1 << '\n';
          valid = false;
        }
      }
      catch(std::invalid_argument) {
        std::cerr << "Error: type-flag must be either \"telemetry\", \"telecommand\", or <int (0-" << pow(2, ccsds::TYPE_FLAG_LEN)-1 << ")>" << '\n';
        valid = false;
      }
      catch(std::out_of_range) {
        std::cerr << "Error: type-flag out of range" << '\n';
        valid = false;
      }
    }
  }

  std::vector<int> except_types = {};
  for (auto v : get_result<std::vector<std::string>>(result["except-type-flag"])) {
    if (v == "telemetry") {
      except_types.emplace_back(0);
    } else if (v == "telecommand") {
      except_types.emplace_back(1);
    } else {
      try {
        except_types.emplace_back(std::stoi(v));
        if (except_types.back() >= pow(2, ccsds::TYPE_FLAG_LEN)) {
          std::cerr << "Error: except-type-flag must be between 0 and " << pow(2, ccsds::TYPE_FLAG_LEN)-1 << '\n';
          valid = false;
        }
      }
      catch(std::invalid_argument) {
        std::cerr << "Error: except-type-flag must be either \"telemetry\", \"telecommand\", or <int (0-" << pow(2, ccsds::TYPE_FLAG_LEN)-1 << ")>" << '\n';
        valid = false;
      }
      catch(std::out_of_range) {
        std::cerr << "Error: except-type-flag out of range" << '\n';
        valid = false;
      }
    }
  }

  auto sec_hdr_flags = get_result<std::vector<int>>(result["sec-hdr-flag"]);
  for (auto v : sec_hdr_flags) {
    if (v >= pow(2, ccsds::SEC_HDR_FLAG_LEN)) {
      std::cerr << "Error: sec-hdr-flag must be between 0 and " << pow(2, ccsds::VERSION_NUMBER_LEN)-1 << '\n';
      valid = false;
    }
  }

  auto except_sec_hdr_flags = get_result<std::vector<int>>(result["sec-hdr-flag"]);
  for (auto v : except_sec_hdr_flags) {
    if (v >= pow(2, ccsds::SEC_HDR_FLAG_LEN)) {
      std::cerr << "Error: except-sec-hdr-flag must be between 0 and " << pow(2, ccsds::VERSION_NUMBER_LEN)-1 << '\n';
      valid = false;
    }
  }

  std::vector<int> app_ids = {};
  for (auto v : get_result<std::vector<std::string>>(result["app-id"])) {
    try {
      app_ids.emplace_back(APP_IDs.at(v));
    } catch (std::out_of_range) {
      try {
        app_ids.emplace_back(std::stoi(v));
        if (app_ids.back() >= pow(2, ccsds::APP_ID_LEN)) {
          std::cerr << "Error: app-id must be between 0 and " << pow(2, ccsds::APP_ID_LEN)-1 << '\n';
          valid = false;
        }
      }
      catch(std::invalid_argument) {
        // TODO: create this dynamically at compile time
        std::cerr << "Error: app-id must be either \"aqua_modis\", or an int" << '\n';
        valid = false;
      }
      catch(std::out_of_range) {
        std::cerr << "Error: app-id out of range" << '\n';
        valid = false;
      }
    }
  }

  std::vector<int> except_app_ids = {};
  for (auto v : get_result<std::vector<std::string>>(result["except-app-id"])) {
    try {
      except_app_ids.emplace_back(APP_IDs.at(v));
    } catch (std::out_of_range) {
      try {
        except_app_ids.emplace_back(std::stoi(v));
        if (except_app_ids.back() >= pow(2, ccsds::APP_ID_LEN)) {
          std::cerr << "Error: except-app-id must be between 0 and " << pow(2, ccsds::APP_ID_LEN)-1 << '\n';
          valid = false;
        }
      }
      catch(std::invalid_argument) {
        // TODO: create this dynamically at compile time
        std::cerr << "Error: except-app-id must be either \"aqua_modis\", or an int" << '\n';
        valid = false;
      }
      catch(std::out_of_range) {
        std::cerr << "Error: except-app-id out of range" << '\n';
        valid = false;
      }
    }
  }

  std::vector<int> seq_flags = {};
  for (auto v : get_result<std::vector<std::string>>(result["seq-flags"])) {
    if (v == "first") {
      seq_flags.emplace_back(1);
    } else if (v == "last") {
      seq_flags.emplace_back(2);
    } else if (v == "continuation") {
      seq_flags.emplace_back(0);
    } else if (v == "unsegmented") {
      seq_flags.emplace_back(3);
    } else {
      try {
        seq_flags.emplace_back(std::stoi(v));
        if (seq_flags.back() >= pow(2, ccsds::TYPE_FLAG_LEN)) {
          std::cerr << "Error: seq-flags must be between 0 and " << pow(2, ccsds::TYPE_FLAG_LEN)-1 << '\n';
          valid = false;
        }
      }
      catch(std::invalid_argument) {
        std::cerr << "Error: seq-flags must be either \"telemetry\", \"telecommand\", or <int (0-" << pow(2, ccsds::TYPE_FLAG_LEN)-1 << ")>" << '\n';
        valid = false;
      }
      catch(std::out_of_range) {
        std::cerr << "Error: seq-flags out of range" << '\n';
        valid = false;
      }
    }
  }

  std::vector<int> except_seq_flags = {};
  for (auto v : get_result<std::vector<std::string>>(result["except-seq-flags"])) {
    if (v == "first") {
      except_seq_flags.emplace_back(1);
    } else if (v == "last") {
      except_seq_flags.emplace_back(2);
    } else if (v == "continuation") {
      except_seq_flags.emplace_back(0);
    } else if (v == "unsegmented") {
      except_seq_flags.emplace_back(3);
    } else {
      try {
        except_seq_flags.emplace_back(std::stoi(v));
        if (except_seq_flags.back() >= pow(2, ccsds::TYPE_FLAG_LEN)) {
          std::cerr << "Error: except-seq-flags must be between 0 and " << pow(2, ccsds::TYPE_FLAG_LEN)-1 << '\n';
          valid = false;
        }
      }
      catch(std::invalid_argument) {
        std::cerr << "Error: except-seq-flags must be either \"telemetry\", \"telecommand\", or <int (0-" << pow(2, ccsds::TYPE_FLAG_LEN)-1 << ")>" << '\n';
        valid = false;
      }
      catch(std::out_of_range) {
        std::cerr << "Error: except-seq-flags out of range" << '\n';
        valid = false;
      }
    }
  }

  auto seq_cnt_or_names = get_result<std::vector<int>>(result["seq-cnt_or-name"]);
  for (auto v : seq_cnt_or_names) {
    if (v >= pow(2, ccsds::SEQ_CNT_OR_NAME_LEN)) {
      std::cerr << "Error: seq-cnt_or-name must be between 0 and " << pow(2, ccsds::VERSION_NUMBER_LEN)-1 << '\n';
      valid = false;
    }
  }

  auto except_seq_cnt_or_names = get_result<std::vector<int>>(result["seq-cnt_or-name"]);
  for (auto v : except_seq_cnt_or_names) {
    if (v >= pow(2, ccsds::SEQ_CNT_OR_NAME_LEN)) {
      std::cerr << "Error: except-seq-cnt_or-name must be between 0 and " << pow(2, ccsds::VERSION_NUMBER_LEN)-1 << '\n';
      valid = false;
    }
  }

  /*
  CCSDSPacket packet;
  while (std::cin >> packet) {
    if (std::ranges::find(version_numbers, packet.version_number()) != version_numbers.end()) {
      std::cout << packet;
    }
  }
  */

/*
  if (result["seq-cnt-or-name"].as<int>() >= pow(2, ccsds::SEQ_CNT_OR_NAME_LEN)) {
    std::cerr << "Error: seq-cnt-or-name must be between 0 and " << pow(2, ccsds::SEQ_CNT_OR_NAME_LEN)-1 << '\n';
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
  */
}
