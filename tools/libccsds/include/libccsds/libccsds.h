#include <array>
#include <bitset>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <span>
#include <memory>
#include <optional>
#include <utility>
#include <vector>
#include <variant>

#include "seqiter/seqiter.h"
#include "getsetproxy/proxy.h"

// https://public.ccsds.org/Pubs/133x0b2e1.pdf

template <typename SecondaryHeader, typename DataField>
struct CCSDSPacket;

namespace ccsds {
  constexpr int VERSION_NUMBER_LEN = 3;
  constexpr int TYPE_FLAG_LEN = 1;
  constexpr int SEC_HDR_FLAG_LEN = 1;
  constexpr int APP_ID_LEN = 11;
  constexpr int SEQ_FLAGS_LEN = 2;
  constexpr int SEQ_CNT_OR_NAME_LEN = 14;
  constexpr int DATA_LEN_LEN = 16;

  // 0 denotes a data section of a single byte
  static constexpr std::size_t MAX_DATA_LEN = 1 << ccsds::DATA_LEN_LEN;
};

#pragma pack(push, 1)
struct CCSDSPrimaryHeader{
  template<typename SecondaryHeader, typename DataField> friend class CCSDSPacket;
private:
  uint16_t _app_id_h : ccsds::APP_ID_LEN - 8 = 0;
  uint16_t _sec_hdr_flag : ccsds::SEC_HDR_FLAG_LEN = 0;
  uint16_t _type : ccsds::TYPE_FLAG_LEN = 0;
  uint16_t _version_number : ccsds::VERSION_NUMBER_LEN = 0;
  uint16_t _app_id_l : 8 = 0;
  uint16_t _seq_cnt_or_name_h : ccsds::SEQ_CNT_OR_NAME_LEN - 8 = 0;
  uint16_t _seq_flags : ccsds::SEQ_FLAGS_LEN = 0;
  uint16_t _seq_cnt_or_name_l : 8 = 0;
  uint16_t _data_len_h : ccsds::DATA_LEN_LEN - 8 = 0;
  uint16_t _data_len_l : 8 = 0;

public:
  struct Iterator {
    using iterator_category = std::forward_iterator_tag;
    using difference_type   = std::ptrdiff_t;
    using value_type        = std::byte;
    using pointer           = value_type*;
    using reference         = value_type&;

    Iterator(pointer ptr) : m_ptr(ptr) {}

    reference operator*() const { return *m_ptr; }
    pointer operator->() { return m_ptr; }

    // Prefix increment
    Iterator& operator++() { m_ptr++; return *this; }  

    // Postfix increment
    Iterator operator++(int) { Iterator tmp = *this; ++(*this); return tmp; }

    Iterator& operator+(int x) { m_ptr += x; return *this; }

    friend bool operator== (const Iterator& a, const Iterator& b) { return a.m_ptr == b.m_ptr; };
    friend bool operator!= (const Iterator& a, const Iterator& b) { return a.m_ptr != b.m_ptr; };
  private:
    pointer m_ptr;

  };

  Iterator begin() { return Iterator(reinterpret_cast<std::byte*>(this)); }
  Iterator end() { return Iterator(reinterpret_cast<std::byte*>(this) + sizeof(CCSDSPrimaryHeader)); }
};
#pragma pack(pop)
static_assert(std::is_trivially_copyable_v<CCSDSPrimaryHeader>,
              "CCSDSPrimaryHeader is not trivially copyable");
static_assert(std::is_standard_layout_v<CCSDSPrimaryHeader>,
              "CCSDSPrimaryHeader is not a standard layout type");
static_assert(sizeof(CCSDSPrimaryHeader) == 6,
              "CCSDSPrimaryHeader is not of size 6 as in the spec");

namespace ccsds {
  constexpr int MIN_PACKET_LEN = sizeof(CCSDSPrimaryHeader) + 1; // The data zone must contain at least one byte
  constexpr int MAX_PACKET_LEN = sizeof(CCSDSPrimaryHeader) + (1 << DATA_LEN_LEN) + 1;
}

// The default data field, consisting just of a single vector
struct CCSDSDataField {
  template<typename T, typename U> friend class CCSDSPacket; // TODO: required? shouldn't be

private:
  std::vector<std::byte> _data = std::vector<std::byte>(0);

  auto size() const -> std::size_t {
    return _data.size();
  }

public:
  CCSDSDataField() = default;

  // Maybe take a vector instead?
  // It's okay to fail if you're passed something that's the wrong sized
  CCSDSDataField(const char* data, std::size_t len) {
    _data.resize(len);
    std::memcpy(_data.data(), data, len);
  }

  // It's okay to fail if you're a fixed length field
  auto resize(std::size_t len) {
    _data.resize(len);
  }
  
  auto begin() { return _data.begin(); }
  auto end() { return _data.end(); }

  auto begin() const { return _data.begin(); }
  auto end() const { return _data.end(); }
};

struct NullSecondaryHeader {
  auto begin() -> std::byte* const { return nullptr; }
  auto end() -> std::byte* const { return nullptr; }

  auto begin() const -> std::byte* const { return nullptr; }
  auto end() const -> std::byte* const { return nullptr; }

  constexpr auto size() const -> std::size_t {
    return 0;
  }
};


template <typename SecondaryHeader = NullSecondaryHeader, typename DataField = CCSDSDataField>
struct CCSDSPacket {
private:
  CCSDSPrimaryHeader primary_header;

public:
  SecondaryHeader secondary_header;
  DataField data_field;

private:
  bool dirty_length = true;

public:
  auto begin() {
    recalculate_length();
    return sequential_iterators<std::byte &>(
      it_pair{primary_header},
      it_pair{secondary_header},
      it_pair{data_field}
    ).begin();
  }

  auto end() {
    return sequential_iterators<std::byte &>(
      it_pair{primary_header},
      it_pair{secondary_header},
      it_pair{data_field}
    ).end();
  }

  /*
  auto begin() const {
    recalculate_length();
    return sequential_iterators<std::byte const &>(
      it_pair{primary_header},
      it_pair{secondary_header},
      it_pair{data_field}
    ).begin();
  }

  auto end() const {
    return sequential_iterators<std::byte const &>(
      it_pair{primary_header},
      it_pair{secondary_header},
      it_pair{data_field}
      ).end();
  }
  */

  // TODO: ask JT why const iterators aren't working

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
      [this](std::size_t len) {
        // TODO: this needs to be changed in light of the new data length etc.
        // Fixed length secondary headers need to be accounted for
        data_field.resize(len+1);
        dirty_length = false;
        primary_header._data_len_h = (len >> 8) & 0xff;
        primary_header._data_len_l = (len) & 0xff;
      }
    };
  }

  auto data() const & -> auto const & {
    return data_field._data;
  }

  auto data() & {
    return Proxy{
      [this]() -> decltype(auto) { return std::as_const(*this).data(); },
      [this](std::span<std::byte> s) { 
        // Data section must contain at least a single octet
        if (s.size() == 0) {
          throw (std::invalid_argument("Data field must contain at least one byte"));
        }
        dirty_length = true;
        int size = std::min(s.size(), ccsds::MAX_DATA_LEN);
        data_field.resize(size);
        std::copy_n(s.begin(), size, data_field._data.begin());
      }
    };
  }

  CCSDSPacket() = default;
  CCSDSPacket(uint8_t const *const input) {
    // Memcpy the fixed length header
    std::memcpy(&primary_header, input, sizeof(CCSDSPrimaryHeader));
    auto len = data_len() + 1;   // Length 0 is used to mean a single byte
    data_field.resize(len);
    // TODO: copy in secondary header?
    std::memcpy(&data_field._data, &(input[sizeof(CCSDSPrimaryHeader)]), len);
  }

  auto size() -> std::size_t {
    return sizeof(primary_header) + data_field.size();
  }

  void recalculate_length() {
    if (dirty_length) {
      data_len() = data_field.size() + secondary_header.size() - 1;
      dirty_length = false;
    }
  }

  template <typename S, typename D>
  friend auto operator<<(std::ostream & output, CCSDSPacket<S, D> & packet) -> std::ostream &;
  template <typename S, typename D>
  friend auto operator>>(std::istream & input, CCSDSPacket<S, D> & packet) -> std::istream &;
};

// An iterator over the items of It, static_cast to type to
template <typename It, typename to>
class static_cast_iterator {
  using iterator_category = std::forward_iterator_tag;

private:
  It it;

public:
  static_cast_iterator(It it) : it{it} {}

  auto operator*() {
    return *this;
  }

  static_cast_iterator& operator=(auto const & value) {
    it = static_cast<to>(value);
  }

  static_cast_iterator& operator++() { it++; return *this; }
  static_cast_iterator operator++(int) const { static_cast_iterator tmp = *this; ++(*this); return tmp; }

  static_cast_iterator operator+(int x) {
    static_cast_iterator tmp = *this;
    tmp.it += x;
    return tmp;
  }


  /*
  friend auto operator== (const static_cast_iterator& a, const static_cast_iterator& b) {
    return a.it == b.it;
  }

  friend auto operator!= (const static_cast_iterator& a, const static_cast_iterator& b) {
    return a.it != b.it;
  }
  */
};

template <typename SecondaryHeader, typename DataField>
auto operator<<(std::ostream & output, CCSDSPacket<SecondaryHeader, DataField> & packet) -> std::ostream & {
  // TODO: this is now broken
  packet.recalculate_length();

  output.write(reinterpret_cast<char*>(&packet.primary_header), sizeof(CCSDSPrimaryHeader));

  for (std::byte b : packet.secondary_header) {
    char c = static_cast<char>(b);
    output.write(&c, 1);
  }

  // TODO: finish static_cast iterator
  /*
  if (packet.sec_hdr_flag()) {
    std::ranges::copy(
      packet.secondary_header,
      static_cast_iterator<std::ostream_iterator<char>, char>(std::ostream_iterator<char>(output)));
  }
  */

  for (std::byte b : packet.data_field) {
    char c = static_cast<char>(b);
    output.write(&c, 1);
  }

  return output;
}

template <typename SecondaryHeader, typename DataField>
auto operator>>(std::istream & input, CCSDSPacket<SecondaryHeader, DataField> & packet) -> std::istream & {
  std::array<char, sizeof(CCSDSPrimaryHeader)> header = {};
  if (! input.read(header.data(), sizeof(CCSDSPrimaryHeader))) {
    return input;
  }
  std::memcpy(&packet.primary_header, header.data(), sizeof(CCSDSPrimaryHeader));

  int sec_hdr_size = 0;
  if (packet.sec_hdr_flag()) {
    auto secondary_header = SecondaryHeader();
    sec_hdr_size = secondary_header.size();
    std::array<char, secondary_header.size()> sec_header = {};
    if (! input.read(sec_header.data(), secondary_header.size())) {
      return input;
    }
    packet.secondary_header = secondary_header;

    // TODO: do something like this copy_n instead: it'll be nicer
    /*
    auto secondary_header = SecondaryHeader();
    std::copy_n(
      std::istreambuf_iterator(input),
      secondary_header.size(),
      secondary_header.begin());
    */
  }

  auto data_len = packet.data_len() - sec_hdr_size + 1;   // Length 0 is used to mean a single byte
  std::cerr << "creating data_len " << data_len << '\n';
  std::vector<char> data;
  data.resize(data_len);
  if (! input.read(&data[0], data_len)) {
    return input;
  }

  auto data_field = DataField(data.data(), data_len);
  packet.data_field = data_field;

  return input;
}
