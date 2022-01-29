#include <bitset>
#include <iostream>
#include <stdexcept>
#include <sstream>


template <int size>
uint32_t find_offset(std::stringstream &input, std::bitset<size> prefix) {
  auto window = std::bitset<size>();  // Creates a bitset with all bits set to zero

  uint32_t count = 0;

  for (char ch; input.get(ch);) {
    // Load the buffer
    auto buffer = std::bitset<8>(ch);

    // Slide the window through the buffer
    for (size_t i=0; i<8; i++) {
      window.set(0, buffer[7-i]);
      if ((window ^ prefix).none()) {
        // Matching set found
        count++;
      }
      window <<= 1;
    }
  };
  input.seekg(0);
  return count;
}

int main() {
  // Read stdin into buffer input
  std::stringstream input;
  input << std::cin.rdbuf();
  
  // 0x1acffc1d;
  auto prefix = std::bitset<32>("00011010110011111111110000011101");

  // Find bit offset of prefix
  uint32_t count = find_offset<32>(input, prefix);

  std::cout << count << std::endl;
}
