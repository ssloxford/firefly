#include <iostream>
#include "libccsds.h"

std::ostream& operator<< (std::ostream& os, std::byte b) {
  return os << std::bitset<8>(std::to_integer<int>(b));
}

int main() {
  std::cerr << "Initialising packet" << std::endl;
  CCSDSPacket packet;
  if (! (std::cin >> packet)) {
    std::cerr << "stream extraction failed" << std::endl;
    return 1;
  } else {
    std::cerr << "version_number: " << packet.version_number() << "\n";
    std::cerr << "type: " << packet.type() << "\n";
    std::cerr << "sec_hdr_flag: " << packet.sec_hdr_flag() << "\n";
    std::cerr << "app_id: " << packet.app_id() << "\n";
    std::cerr << "seq_flags: " << packet.seq_flags() << "\n";
    std::cerr << "seq_cnt_or_name: " << packet.seq_cnt_or_name() << "\n";
    std::cerr << "data_len: " << packet.data_len() << "\n";
    std::cout << packet;
  }
}
