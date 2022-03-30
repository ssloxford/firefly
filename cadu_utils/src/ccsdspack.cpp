// Packs a byte stream into CCSDS packets

#include <iostream>
#include <ranges>

#include "libccsds/libccsds.h"
#include "ccsds_constants.h"

std::ostream& operator<< (std::ostream& os, std::byte b) {
  return os << std::bitset<8>(std::to_integer<int>(b));
}

int main() {
  auto buffer = std::vector<std::byte> {};
  buffer.resize(CCSDS_MAX_DATA_LEN);

  int n_read = 0;
  char c;
  while (std::cin.get(c)) {
    if (n_read >= CCSDS_MAX_DATA_LEN) {
      break;
    }
    buffer[n_read] = std::byte(c);
    ++n_read;
  }

  buffer.resize(n_read);

  CCSDSPacket packet;
  packet.data() = buffer;
  packet.sec_hdr_flag() = 1;
  packet.app_id() = APP_ID_AQUA_MODIS;
  packet.seq_flags() = 1;
  packet.seq_cnt_or_name() = 16003;
  std::cout << packet;
}
