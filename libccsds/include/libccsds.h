#include <array>
#include <bitset>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <span>
#include <memory>
#include <optional>
#include <utility>
#include <vector>

extern "C" {
#include "fec.h"
}

#include "proxy.h"

// https://public.ccsds.org/Pubs/133x0b2e1.pdf

struct CCSDSPacket;

#pragma pack(push, 1)
struct CCSDSPrimaryHeader{
  friend CCSDSPacket;
private:
  uint16_t _app_id_h : 3 = 0;
  uint16_t _sec_hdr_flag : 1 = 0;
  uint16_t _type : 1 = 0;
  uint16_t _version_number : 3 = 0;
  uint16_t _app_id_l : 8 = 0;
  uint16_t _seq_cnt_or_name_h : 6 = 0;
  uint16_t _seq_flags : 2 = 0;
  uint16_t _seq_cnt_or_name_l : 8 = 0;
  uint16_t _data_len_h : 8 = 0;
  uint16_t _data_len_l : 8 = 0;
};
#pragma pack(pop)
static_assert(std::is_trivially_copyable_v<CCSDSPrimaryHeader>,
              "CCSDSPrimaryHeader is not trivially copyable");
static_assert(std::is_standard_layout_v<CCSDSPrimaryHeader>,
              "CCSDSPrimaryHeader is not a standard layout type");
static_assert(sizeof(CCSDSPrimaryHeader) == 6,
              "CCSDSPrimaryHeader is not of size 6 as in the spec");

struct CCSDSDataField {
  // Can I tell this not to initialise yet?
  std::vector<std::byte> data; // TODO: initialise into secondary header and user data field

  auto resize(int len) {
    data.resize(len);
  }

  auto size() const -> size_t {
    return data.size();
  }
};


struct CCSDSPacket {
private:
  CCSDSPrimaryHeader primary_header;
  CCSDSDataField data_field;

public:
  auto version_number() const & {
    return static_cast<int>(primary_header._version_number);
  }

  auto version_number() & {
    return Proxy{
      [this]() -> decltype(auto) { return std::as_const(*this).version_number(); },
      [this](int x) { primary_header._version_number = x; }
    };
  }

  auto type() const & {
    return static_cast<int>(primary_header._type);
  }

  auto type() & {
    return Proxy{
      [this]() -> decltype(auto) { return std::as_const(*this).type(); },
      [this](int x) { primary_header._type = x; }
    };
  }

  auto sec_hdr_flag() const & {
    return static_cast<int>(primary_header._sec_hdr_flag);
  }

  auto sec_hdr_flag() & {
    return Proxy{
      [this]() -> decltype(auto) { return std::as_const(*this).sec_hdr_flag(); },
      [this](int x) { primary_header._sec_hdr_flag = x; }
    };
  }

  auto app_id() const & {
    return static_cast<int>(primary_header._app_id_h << 8 | primary_header._app_id_l);
  }

  auto app_id() & {
    return Proxy{
      [this]() -> decltype(auto) { return std::as_const(*this).app_id(); },
      [this](int x) { primary_header._app_id_h = (x >> 8) & 0x03; primary_header._app_id_l = x & 0xff; }
    };
  }

  auto seq_flags() const & {
    return static_cast<int>(primary_header._seq_flags);
  }

  auto seq_flags() & {
    return Proxy{
      [this]() -> decltype(auto) { return std::as_const(*this).seq_flags(); },
      [this](int x) { primary_header._seq_flags = x; }
    };
  }

  auto seq_cnt_or_name() const & {
    return static_cast<int>(primary_header._seq_cnt_or_name_h << 8 | primary_header._seq_cnt_or_name_l);
  }

  auto seq_cnt_or_name() & {
    return Proxy{
      [this]() -> decltype(auto) { return std::as_const(*this).seq_cnt_or_name(); },
      [this](int x) {
        primary_header._seq_cnt_or_name_h = (x >> 8) & 0x3f;
        primary_header._seq_cnt_or_name_l = x  & 0xff;
      }
    };
  }

  auto data_len() const & {
    return static_cast<int>(primary_header._data_len_h << 8 | primary_header._data_len_l);
  }

  auto data_len() & {
    return Proxy{
      [this]() -> decltype(auto) { return std::as_const(*this).data_len(); },
      [this](int x) {
        primary_header._data_len_h = (x >> 8) & 0xff;
        primary_header._data_len_l = (x) & 0xff;
      }
    };
  }

  CCSDSPacket() = default;
  CCSDSPacket(uint8_t const *const input) {
    // Memcpy the fixed length header
    std::memcpy(&primary_header, input, sizeof(CCSDSPrimaryHeader));
    auto len = data_len() + 1;   // Length 0 is used to mean a single byte
    data_field.resize(len);
    std::memcpy(&data_field.data, &(input[sizeof(CCSDSPrimaryHeader)]), len);
  }

  auto size() -> size_t {
    return sizeof(primary_header) + data_field.size();
  }

  friend auto operator<<(std::ostream & output, CCSDSPacket & packet) -> std::ostream &;
  friend auto operator>>(std::istream & input, CCSDSPacket & packet) -> std::istream &;
};


auto operator<<(std::ostream & output, CCSDSPacket & packet) -> std::ostream & {
  output.write(reinterpret_cast<char*>(&packet.primary_header), sizeof(CCSDSPrimaryHeader));
  output.write(reinterpret_cast<char*>(&packet.data_field), packet.data_field.size());
  return output;
}


auto operator>>(std::istream & input, CCSDSPacket & packet) -> std::istream & {
  std::array<char, sizeof(CCSDSPrimaryHeader)> header = {};
  input.read(header.data(), sizeof(CCSDSPrimaryHeader));
  std::memcpy(&packet.primary_header, header.data(), sizeof(CCSDSPrimaryHeader));

  auto data_len = packet.data_len() + 1;   // Length 0 is used to mean a single byte
  std::vector<char> data;
  data.resize(data_len);
  input.read(&data[0], data_len);
  packet.data_field.resize(data_len);
  std::memcpy(packet.data_field.data.data(), data.data(), data_len);
  return input;
}
