#include <bitset>
#include <iostream>
#include <stdexcept>
#include <sstream>


template <int size>
size_t find_offset(std::stringstream &input, std::bitset<size> prefix) {
  auto window = std::bitset<size>();  // Creates a bitset with all bits set to zero

  for (char ch; input.get(ch);) {
    // Load the buffer
    auto buffer = std::bitset<8>(ch);

    // Slide the window through the buffer
    for (size_t i=0; i<8; i++) {
      window.set(0, buffer[7-i]);
      if ((window ^ prefix).none()) {
        // Matching set found
        input.seekg(0);
        return (i+1)%8;
      }
      window <<= 1;
    }
  };
  input.seekg(0);
  throw "No matching bitset found";
}


// Writes file from input to stdout, bit-level offset by offset
void write_with_offset(std::stringstream &input, size_t offset) {
  auto in_buffer = std::bitset<8>();
  auto out_buffer = std::bitset<8>();

  if (offset == 0) {
    for (char ch; input.get(ch); std::cout << ch);
  } else {
    for (char ch; input.get(ch);) {
      in_buffer = std::bitset<8>(ch);

      // Read the correct number of bits into in_buffer
      for (int i=0; i<offset; i++) {
        out_buffer.set(offset-1-i, in_buffer[7-i]);
      }

      // Write out the out_buffer
      std::cout << static_cast<unsigned char>(out_buffer.to_ulong());

      // Swap out_buffer into in_buffer and bit shift
      out_buffer = in_buffer;
      out_buffer <<= offset;
    }
    // Write out final out buffer
    std::cout << static_cast<unsigned char>(out_buffer.to_ulong());
  }
}


int main() {
  // Read stdin into buffer input
  std::stringstream input;
  input << std::cin.rdbuf();
  
  // 0x1acffc1d;
  auto prefix = std::bitset<32>("00011010110011111111110000011101");

  // Find bit offset of prefix
  size_t offset;
  try {
    offset = find_offset<32>(input, prefix);
  } catch (char const* c) {
    std::cerr << c << "\n";
    exit(1);
  }

  std::cerr << "Offsetting file by " << offset << " bits\n";
  write_with_offset(input, offset);
}
