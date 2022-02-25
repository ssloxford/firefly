#include <iostream>
#include "libcadu.h"

int main() {
  using namespace nonrandomised;
  CADU cadu;
  std::cin >> cadu;
  std::cerr << "version_number: " << cadu.get_mutable().version_number() << "\n";
  std::cerr << "scid: " << cadu.get_mutable().scid() << "\n";
  std::cerr << "vcid: " << cadu.get_mutable().vcid() << "\n";
  std::cerr << "vcdu_counter: " << cadu.get_mutable().vcdu_counter() << "\n";
  std::cerr << "replay_flag: " << cadu.get_mutable().replay_flag() << "\n";
  std::cerr << "vcdu_spare: " << cadu.get_mutable().vcdu_spare() << "\n";
  std::cerr << "m_pdu_spare: " << cadu.get_mutable().m_pdu_spare() << "\n";
  std::cerr << "first_header_pointer: " << cadu.get_mutable().first_header_pointer() << "\n";
  std::cout << cadu;
}
