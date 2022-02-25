#include <iostream>
#include "libcadu.h"

int main() {
  using namespace nonrandomised;
  CADU cadu;
  std::cin >> cadu;
  std::cerr << "version_number: " << cadu->version_number() << "\n";
  std::cerr << "scid: " << cadu->scid() << "\n";
  std::cerr << "vcid: " << cadu->vcid() << "\n";
  std::cerr << "vcdu_counter: " << cadu->vcdu_counter() << "\n";
  std::cerr << "replay_flag: " << cadu->replay_flag() << "\n";
  std::cerr << "vcdu_spare: " << cadu->vcdu_spare() << "\n";
  std::cerr << "m_pdu_spare: " << cadu->m_pdu_spare() << "\n";
  std::cerr << "first_header_pointer: " << cadu->first_header_pointer() << "\n";
  std::cout << cadu;
}
