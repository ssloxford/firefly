#include <bitset>
#include <iostream>
#include <stdexcept>
#include <sstream>
#include "fec.h"

// Locates CADU frames and updates the checksum according to the CCSDS specification
// Reed-Solomon checksum algorithm
// https://public.ccsds.org/Lists/Products/Attachments/27/CAMBR_Reed_Solomon_Encoder.pdf

// 0x1acffc1d;
const auto prefix = std::bitset<32>("00011010110011111111110000011101");


// Writes file from input to stdout, bit-level offset by offset
// Searches for CCSDS frame headers, derandomises the contents, and outputs the result
// NB: this code mangles the header of a sync frame if it appears within the first 1024 bytes
// of a different sync frame
void denoise(std::stringstream &input) {
    auto byte_buffer = std::bitset<8>();
    auto prefix_buffer = std::bitset<32>();
    bool in_frame = false;
    int frame_counter = 0;

    data = std::vector<std::array<char, 992> = {};
    
    for (char ch; input.get(ch);) {
        // Read in next byte
        byte_buffer = std::bitset<8>(ch);

        // Check for matching prefix for previous byte
        if ((prefix_buffer ^ prefix).none()) {
            in_frame = true;
        }

        // Update prefix buffer
        prefix_buffer <<= 8;
        for (size_t i=0; i<8; i++)
            prefix_buffer.set(i, byte_buffer[i]);

        // Read the relevant data structures into a thing

        // Derandomise the current byte
        if (in_frame) {
            if (frame_counter < 1020) {
                byte_buffer ^= table[frame_counter % 256];
                frame_counter++;
            } else {
                in_frame = false;
                frame_counter = 0;
            }
        }

        // Output the byte buffer
        std::cout << static_cast<unsigned char>(byte_buffer.to_ulong());
    }
}


int main() {
  // Read stdin into buffer inpu
  std::stringstream input;
  input << std::cin.rdbuf();
  denoise(input);
}
