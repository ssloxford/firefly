#include <array>
#include <bitset>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <memory>

#include "proxy.h"

// TODO: Precalculate array of uint8_ts of sync pulse middle sections - 8x3
// As constexpr into a const buffer
// Currently we rely on the bit pattern being synchronised already

// TODO: shift to uint8_t where appropriate
// TODO: test bit outputs
// TODO: sort out get_mutable syntax and dirtying checksum
uint32_t SYNC_MARKER = 0x1acffc1d;

// Default encoding table, from generator polynomial x**8 + x**7 + x**5 + x**3 + 1
// From gov/nasa/gsfc/drl/rtstps/core/PnDecoder.java, originally from
// Gerald Grebowsky of GSFC in 1996

const uint8_t randomise_table[] = {
    0xff, 0x48, 0x0e, 0xc0, 0x9a, 0x0d, 0x70, 0xbc, 0x8e, 0x2c, 0x93,
    0xad, 0xa7, 0xb7, 0x46, 0xce, 0x5a, 0x97, 0x7d, 0xcc, 0x32, 0xa2,
    0xbf, 0x3e, 0x0a, 0x10, 0xf1, 0x88, 0x94, 0xcd, 0xea, 0xb1, 0xfe,
    0x90, 0x1d, 0x81, 0x34, 0x1a, 0xe1, 0x79, 0x1c, 0x59, 0x27, 0x5b,
    0x4f, 0x6e, 0x8d, 0x9c, 0xb5, 0x2e, 0xfb, 0x98, 0x65, 0x45, 0x7e,
    0x7c, 0x14, 0x21, 0xe3, 0x11, 0x29, 0x9b, 0xd5, 0x63, 0xfd, 0x20,
    0x3b, 0x02, 0x68, 0x35, 0xc2, 0xf2, 0x38, 0xb2, 0x4e, 0xb6, 0x9e,
    0xdd, 0x1b, 0x39, 0x6a, 0x5d, 0xf7, 0x30, 0xca, 0x8a, 0xfc, 0xf8,
    0x28, 0x43, 0xc6, 0x22, 0x53, 0x37, 0xaa, 0xc7, 0xfa, 0x40, 0x76,
    0x04, 0xd0, 0x6b, 0x85, 0xe4, 0x71, 0x64, 0x9d, 0x6d, 0x3d, 0xba,
    0x36, 0x72, 0xd4, 0xbb, 0xee, 0x61, 0x95, 0x15, 0xf9, 0xf0, 0x50,
    0x87, 0x8c, 0x44, 0xa6, 0x6f, 0x55, 0x8f, 0xf4, 0x80, 0xec, 0x09,
    0xa0, 0xd7, 0x0b, 0xc8, 0xe2, 0xc9, 0x3a, 0xda, 0x7b, 0x74, 0x6c,
    0xe5, 0xa9, 0x77, 0xdc, 0xc3, 0x2a, 0x2b, 0xf3, 0xe0, 0xa1, 0x0f,
    0x18, 0x89, 0x4c, 0xde, 0xab, 0x1f, 0xe9, 0x01, 0xd8, 0x13, 0x41,
    0xae, 0x17, 0x91, 0xc5, 0x92, 0x75, 0xb4, 0xf6, 0xe8, 0xd9, 0xcb,
    0x52, 0xef, 0xb9, 0x86, 0x54, 0x57, 0xe7, 0xc1, 0x42, 0x1e, 0x31,
    0x12, 0x99, 0xbd, 0x56, 0x3f, 0xd2, 0x03, 0xb0, 0x26, 0x83, 0x5c,
    0x2f, 0x23, 0x8b, 0x24, 0xeb, 0x69, 0xed, 0xd1, 0xb3, 0x96, 0xa5,
    0xdf, 0x73, 0x0c, 0xa8, 0xaf, 0xcf, 0x82, 0x84, 0x3c, 0x62, 0x25,
    0x33, 0x7a, 0xac, 0x7f, 0xa4, 0x07, 0x60, 0x4d, 0x06, 0xb8, 0x5e,
    0x47, 0x16, 0x49, 0xd6, 0xd3, 0xdb, 0xa3, 0x67, 0x2d, 0x4b, 0xbe,
    0xe6, 0x19, 0x51, 0x5f, 0x9f, 0x05, 0x08, 0x78, 0xc4, 0x4a, 0x66,
    0xf5, 0x58
};


  static unsigned char lookup[16] = {
    0x0, 0x8, 0x4, 0xc, 0x2, 0xa, 0x6, 0xe,
    0x1, 0x9, 0x5, 0xd, 0x3, 0xb, 0x7, 0xf, };

  uint8_t reverse(uint8_t n) {
    // Reverse the top and bottom nibble then swap them.
    return (lookup[n&0b1111] << 4) | lookup[n>>4];
  }



#pragma pack(push, 1)
struct VC_PDU {
private:
  uint16_t _scid_h : 6;
  uint16_t _version_number : 2;
  uint16_t _vcid : 6;
  uint16_t _scid_l : 2;
  uint32_t _vcdu_counter_h : 8;
  uint32_t _vcdu_counter_m : 8;
  uint32_t _vcdu_counter_l : 8;
  uint32_t _vcdu_spare : 7 = 0;
  uint32_t _replay_flag : 1;
  uint16_t _first_header_pointer_h : 3;
  uint16_t _m_pdu_spare : 5 = 0;
  uint16_t _first_header_pointer_l : 8;
  std::array<std::byte, 884> _data;


public:
  auto version_number() && {
    return (uint16_t) _version_number;
  }

  auto version_number() & {
    return Proxy{
      [this] { return std::move(*this).version_number(); },
      [this](uint16_t x) { _version_number = x; }
    };
  }

  auto scid() && {
    return (uint16_t) _scid_h << 2 | _scid_l;
  }

  auto scid() & {
    return Proxy{
      [this] { return std::move(*this).scid(); },
      [this](uint16_t x) { _scid_h = (x >> 2) & 0xff; _scid_l = x & 0x03; }
    };
  }

  auto vcid() && {
    return (uint16_t) _vcid;
  }

  auto vcid() & {
    return Proxy{
      [this] { return std::move(*this).vcid(); },
      [this](uint16_t x) { _vcid = x; }
    };
  }

  auto vcdu_counter() && {
    return (uint32_t) _vcdu_counter_h << 16 | _vcdu_counter_m << 8 | _vcdu_counter_l;
  }

  auto vcdu_counter() & {
    return Proxy{
      [this] { return std::move(*this).vcdu_counter(); },
      [this](uint32_t x) {
        _vcdu_counter_h = (x >> 16) & 0xff;
        _vcdu_counter_m = (x >> 8) & 0xff;
        _vcdu_counter_l = x & 0xff;
      }
    };
  }

  auto replay_flag() && {
    return (uint32_t) _replay_flag;
  }

  auto replay_flag() & {
    return Proxy{
      [this] { return std::move(*this).replay_flag(); },
      [this](uint32_t x) { _replay_flag= x; }
    };
  }

  auto vcdu_spare() && {
    return (uint32_t) _vcdu_spare;
  }

  auto vcdu_spare() & {
    return Proxy{
      [this] { return std::move(*this).vcdu_spare(); },
      [this](uint32_t x) { _vcdu_spare = x; }
    };
  }

  auto m_pdu_spare() && {
    return (uint16_t) _m_pdu_spare;
  }

  auto m_pdu_spare() & {
    return Proxy{
      [this] { return std::move(*this).m_pdu_spare(); },
      [this](uint16_t x) { _m_pdu_spare = x; }
    };
  }

  auto first_header_pointer() && {
    return (uint16_t) _first_header_pointer_h << 8 | _first_header_pointer_l;
  }

  auto first_header_pointer() & {
    return Proxy{
      [this] { return std::move(*this).first_header_pointer(); },
      [this](uint16_t x) { _first_header_pointer_h = x >> 8; _first_header_pointer_l= x & 0xff; }
    };
  }
};
#pragma pack(pop)
static_assert(std::is_trivially_copyable_v<VC_PDU>,
              "VC_PDU is not trivially copyable");
static_assert(std::is_standard_layout_v<VC_PDU>,
              "VC_PDU is not a standard layout type");


#pragma pack(push, 1)
struct CVCDU {
  VC_PDU vc_pdu;
  std::array<std::byte, 128> checksum;

  CVCDU() = default;
  CVCDU(VC_PDU const &vc_pdu) : vc_pdu{vc_pdu} {}
};
#pragma pack(pop)
static_assert(sizeof(CVCDU) == 1020,
              "CVCDU is not of size 1020 as in the spec");
static_assert(std::is_trivially_copyable_v<CVCDU>,
              "CVCDU is not trivially copyable");
static_assert(std::is_standard_layout_v<CVCDU>,
              "CVCDU is not a standard layout type");

struct CADU;

namespace nonrandomised {
  auto operator<<(std::ostream & output, CADU & cadu) -> std::ostream &;
  auto operator>>(std::istream & input, CADU & cadu) -> std::istream &;
}

namespace randomised {
  auto operator<<(std::ostream & output, CADU & cadu) -> std::ostream &;
  auto operator>>(std::istream & input, CADU & cadu) -> std::istream &;
}

struct CADU {
private:
  struct Impl {
    const uint32_t sync = SYNC_MARKER;
    CVCDU cvcdu;

    Impl() = default;
    Impl(VC_PDU const &vc_pdu) : cvcdu{vc_pdu} {}
  };

  Impl impl;
  bool dirty_checksum = false;

  auto randomise() -> void {
    auto data = reinterpret_cast<uint8_t*>(&impl.cvcdu);
    for (int i = 0; i < sizeof(impl.cvcdu); i++) {
      data[i] ^= randomise_table[i];
    }
  }

public:
  auto reverse_endian() -> void {
    auto data = reinterpret_cast<uint8_t*>(&impl.cvcdu);
    for (int i = 0; i < sizeof(impl.cvcdu); i++) {
      data[i] = reverse(data[i]);
    }
  }

  // Constructor for everything without sync pulse
  CADU() = default;
  CADU(const CADU &cadu) : impl{cadu.impl.cvcdu.vc_pdu}, dirty_checksum{true} {}
  CADU(uint8_t const *const input) {std::memcpy(&impl.cvcdu, input, sizeof(CVCDU));}

  CADU(VC_PDU const &vc_pdu) : impl{vc_pdu}, dirty_checksum{true} {}

  // Reinitialise the CADU
  auto reinitialise(char const *const input) {
    std::memcpy(&impl.cvcdu, input, sizeof(CVCDU));
  }

  auto operator*() const -> VC_PDU const & { return impl.cvcdu.vc_pdu; }

  auto operator->() const -> VC_PDU const * { return &impl.cvcdu.vc_pdu; }

  auto get_mutable() -> VC_PDU & {
    dirty_checksum = true;
    return impl.cvcdu.vc_pdu;
  }

  void recalculate_checksum() {
    // TODO: recalculate checksum
  }
  friend auto nonrandomised::operator<<(std::ostream & output, CADU & cadu) -> std::ostream &;
  friend auto nonrandomised::operator>>(std::istream & input, CADU & cadu) -> std::istream &;
  friend auto randomised::operator<<(std::ostream & output, CADU & cadu) -> std::ostream &;
  friend auto randomised::operator>>(std::istream & input, CADU & cadu) -> std::istream &;};

namespace nonrandomised {
  auto operator<<(std::ostream & output, CADU & cadu) -> std::ostream & {
    if (cadu.dirty_checksum)
      cadu.recalculate_checksum();
    // TODO: check this is safe
    std::cerr << "outputting " << sizeof(cadu.impl) << " bytes\n";
    output.write(reinterpret_cast<char*>(&cadu.impl), sizeof(cadu.impl));
    return output;
  }

  auto operator>>(std::istream & input, CADU & cadu) -> std::istream & {
    uint32_t prefix_buffer = 0;
    bool found_header = false;

    for (uint8_t byte_buffer; input >> byte_buffer;) {
      // Update prefix buffer
      prefix_buffer <<= 8;
      prefix_buffer |= byte_buffer;

      // Check for matching prefix
      if (prefix_buffer == SYNC_MARKER) {
        found_header = true;
        break;
      }
    }

    if (found_header) {
      // We found the next frame
      std::cerr << "header found\n";
      // TODO: bytes on the stack instead
      auto bytes = std::make_unique_for_overwrite<char[]>(sizeof(CVCDU)/sizeof(char));
      // TODO: check this reads the right number
      input.read(bytes.get(), sizeof(CVCDU));
      cadu.reinitialise(bytes.get());
      std::cerr << "reinitialised " << sizeof(CVCDU) << " bytes\n";
    }
    return input;
  }  
}

namespace randomised {
  auto operator<<(std::ostream & output, CADU & cadu) -> std::ostream & {
    auto randomised_cadu = CADU(cadu);
    randomised_cadu.randomise();
    nonrandomised::operator<<(output, randomised_cadu);
    return output;
  }
  auto operator>>(std::istream & input, CADU & cadu) -> std::istream & {
    nonrandomised::operator>>(input, cadu);
    cadu.randomise();
    return input;
  }
}
