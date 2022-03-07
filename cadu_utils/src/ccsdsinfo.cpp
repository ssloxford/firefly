#include <iostream>
#include "libccsds/libccsds.h"

int main() {
  CCSDSPacket packet;
  while (std::cin >> packet) {
    std::cout << "version_number: " << packet.version_number() << "\n";
    std::cout << "type: " << packet.type() << "\n";
    std::cout << "sec_hdr_flag: " << packet.sec_hdr_flag() << "\n";
    std::cout << "app_id: " << packet.app_id() << "\n";
    std::cout << "seq_flags: " << packet.seq_flags() << "\n";
    std::cout << "seq_cnt_or_name: " << packet.seq_cnt_or_name() << "\n";
    std::cout << "data_len: " << packet.data_len() << "\n";
    std::cout << std::endl;
  }
}
