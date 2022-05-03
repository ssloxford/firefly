#include <array>
#include <bitset>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <memory>
#include <ranges>
#include <span>
#include <stdexcept>
#include <utility>
#include <variant>
#include <vector>

#include "getsetproxy/proxy.h"
#include "seqiter/seqiter.h"

// https://public.ccsds.org/Pubs/133x0b2e1.pdf

template <typename SecondaryHeader, typename DataField> struct CCSDSPacket;

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
}; // namespace ccsds

#pragma pack(push, 1)
struct CCSDSPrimaryHeader {
  template <typename SecondaryHeader, typename DataField>
  friend class CCSDSPacket;

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
    using difference_type = std::ptrdiff_t;
    using value_type = std::byte;
    using pointer = value_type *;
    using reference = value_type &;

    Iterator() = default;
    Iterator(pointer ptr) : m_ptr(ptr) {}

    reference operator*() const { return *m_ptr; }
    pointer operator->() { return m_ptr; }

    // Prefix increment
    Iterator &operator++() {
      m_ptr++;
      return *this;
    }

    // Postfix increment
    Iterator operator++(int) {
      Iterator tmp = *this;
      ++(*this);
      return tmp;
    }

    Iterator &operator+(int x) {
      m_ptr += x;
      return *this;
    }

    friend bool operator==(const Iterator &a, const Iterator &b) {
      return a.m_ptr == b.m_ptr;
    };
    friend bool operator!=(const Iterator &a, const Iterator &b) {
      return a.m_ptr != b.m_ptr;
    };

  private:
    pointer m_ptr;
  };

  Iterator begin() { return Iterator(reinterpret_cast<std::byte *>(this)); }
  Iterator end() {
    return Iterator(reinterpret_cast<std::byte *>(this) +
                    sizeof(CCSDSPrimaryHeader));
  }
};
#pragma pack(pop)
static_assert(std::is_trivially_copyable_v<CCSDSPrimaryHeader>,
              "CCSDSPrimaryHeader is not trivially copyable");
static_assert(std::is_standard_layout_v<CCSDSPrimaryHeader>,
              "CCSDSPrimaryHeader is not a standard layout type");
static_assert(sizeof(CCSDSPrimaryHeader) == 6,
              "CCSDSPrimaryHeader is not of size 6 as in the spec");

namespace ccsds {
constexpr int MIN_PACKET_LEN =
    sizeof(CCSDSPrimaryHeader) +
    1; // The data zone must contain at least one byte
constexpr int MAX_PACKET_LEN =
    sizeof(CCSDSPrimaryHeader) + (1 << DATA_LEN_LEN) + 1;
} // namespace ccsds

// The default data field, consisting just of a single vector
struct CCSDSDataField {
  template <typename T, typename U>
  friend class CCSDSPacket; // TODO: required? shouldn't be

private:
  std::vector<std::byte> _data = std::vector<std::byte>(0);

  auto size() const -> std::size_t { return _data.size(); }

public:
  CCSDSDataField() = default;

  // Maybe take a vector instead?
  // It's okay to fail if you're passed something that's the wrong sized
  CCSDSDataField(const char *data, std::size_t len) {
    _data.resize(len);
    std::memcpy(_data.data(), data, len);
  }

  // It's okay to fail if you're a fixed length field
  auto resize(std::size_t len) { _data.resize(len); }

  auto begin() { return _data.begin(); }
  auto end() { return _data.end(); }

  auto begin() const { return _data.begin(); }
  auto end() const { return _data.end(); }
};

struct NullSecondaryHeader {
  NullSecondaryHeader() = default;
  NullSecondaryHeader(const char *data, std::size_t len) {
    if (len != size()) {
      std::cerr << "size: " << len << '\n';
      std::cerr << "correct size: " << size() << '\n';
      throw std::invalid_argument("NullSecondaryHeader - invalid size");
    }
  }

public:
  auto begin() -> std::byte *const { return nullptr; }
  auto end() -> std::byte *const { return nullptr; }

  auto begin() const -> std::byte *const { return nullptr; }
  auto end() const -> std::byte *const { return nullptr; }

  constexpr auto size() const -> std::size_t { return 0; }
};

template <typename SecondaryHeader = NullSecondaryHeader,
          typename DataField = CCSDSDataField>
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
    if (sec_hdr_flag()) {
      return sequential_iterators<std::byte &>(it_pair{primary_header},
                                               it_pair{secondary_header},
                                               it_pair{data_field})
          .begin();
    } else {
      return sequential_iterators<std::byte &>(
                 it_pair{primary_header},
                 it_pair{secondary_header.begin(), secondary_header.begin()},
                 it_pair{data_field})
          .begin();
    }
  }

  auto end() {
    if (sec_hdr_flag()) {
      return sequential_iterators<std::byte &>(it_pair{primary_header},
                                               it_pair{secondary_header},
                                               it_pair{data_field})
          .end();
    } else {
      return sequential_iterators<std::byte &>(
                 it_pair{primary_header},
                 it_pair{secondary_header.begin(), secondary_header.begin()},
                 it_pair{data_field})
          .end();
    }
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
    return Proxy{[this]() -> decltype(auto) {
                   return std::as_const(*this).version_number();
                 },
                 [this](int x) { primary_header._version_number = x; }};
  }

  auto type() const & { return static_cast<int>(primary_header._type); }

  auto type() & {
    return Proxy{
        [this]() -> decltype(auto) { return std::as_const(*this).type(); },
        [this](int x) { primary_header._type = x; }};
  }

  auto sec_hdr_flag() const & {
    return static_cast<int>(primary_header._sec_hdr_flag);
  }

  auto sec_hdr_flag() & {
    return Proxy{[this]() -> decltype(auto) {
                   return std::as_const(*this).sec_hdr_flag();
                 },
                 [this](int x) { primary_header._sec_hdr_flag = x; }};
  }

  auto app_id() const & {
    return static_cast<int>(primary_header._app_id_h << 8 |
                            primary_header._app_id_l);
  }

  auto app_id() & {
    return Proxy{
        [this]() -> decltype(auto) { return std::as_const(*this).app_id(); },
        [this](int x) {
          primary_header._app_id_h = (x >> 8) & 0x03;
          primary_header._app_id_l = x & 0xff;
        }};
  }

  auto seq_flags() const & {
    return static_cast<int>(primary_header._seq_flags);
  }

  auto seq_flags() & {
    return Proxy{
        [this]() -> decltype(auto) { return std::as_const(*this).seq_flags(); },
        [this](int x) { primary_header._seq_flags = x; }};
  }

  auto seq_cnt_or_name() const & {
    return static_cast<int>(primary_header._seq_cnt_or_name_h << 8 |
                            primary_header._seq_cnt_or_name_l);
  }

  auto seq_cnt_or_name() & {
    return Proxy{[this]() -> decltype(auto) {
                   return std::as_const(*this).seq_cnt_or_name();
                 },
                 [this](int x) {
                   primary_header._seq_cnt_or_name_h = (x >> 8) & 0x3f;
                   primary_header._seq_cnt_or_name_l = x & 0xff;
                 }};
  }

  auto data_len() const & {
    return static_cast<int>(primary_header._data_len_h << 8 |
                            primary_header._data_len_l);
  }

  auto data_len() & {
    return Proxy{
        [this]() -> decltype(auto) { return std::as_const(*this).data_len(); },
        [this](std::size_t len) {
          // TODO: this needs to be changed in light of the new data length etc.
          // Fixed length secondary headers need to be accounted for
          data_field.resize(len + 1);
          dirty_length = false;
          primary_header._data_len_h = (len >> 8) & 0xff;
          primary_header._data_len_l = (len)&0xff;
        }};
  }

  auto data() const & -> auto const & { return data_field._data; }

  auto data() & {
    return Proxy{
        [this]() -> decltype(auto) { return std::as_const(*this).data(); },
        [this](std::span<std::byte> s) {
          // Data section must contain at least a single octet
          if (s.size() == 0) {
            throw(std::invalid_argument(
                "Data field must contain at least one byte"));
          }
          dirty_length = true;
          int size = std::min(s.size(), ccsds::MAX_DATA_LEN);
          data_field.resize(size);
          std::copy_n(s.begin(), size, data_field._data.begin());
        }};
  }

  CCSDSPacket() = default;
  CCSDSPacket(uint8_t const *const input) {
    // Memcpy the fixed length header
    std::memcpy(&primary_header, input, sizeof(CCSDSPrimaryHeader));

    secondary_header = SecondaryHeader(input + sizeof(CCSDSPrimaryHeader),
                                       secondary_header.size());

    // TODO: remove this horrible hack to get the header size
    auto size = SecondaryHeader();
    int sec_hdr_size = size.size();

    data_field = DataField(input + sizeof(CCSDSPrimaryHeader) + sec_hdr_size,
                           data_len() - sec_hdr_size +
                               1); // Length 0 is used to mean a single byte
  }

  auto size() -> std::size_t {
    return sizeof(primary_header) + data_field.size();
  }

  void recalculate_length() {
    if (dirty_length) {
      std::cerr << "recalculating length: " << data_field.size() << ","
                << secondary_header.size() << '\n';
      data_len() = data_field.size() + secondary_header.size() - 1;
      dirty_length = false;
    }
  }

  template <typename S, typename D>
  friend auto operator<<(std::ostream &output, CCSDSPacket<S, D> &packet)
      -> std::ostream &;
  template <typename S, typename D>
  friend auto operator>>(std::istream &input, CCSDSPacket<S, D> &packet)
      -> std::istream &;
};

static_assert(std::copyable<decltype(CCSDSPacket{}.end())>);
static_assert(std::default_initializable<decltype(CCSDSPacket{}.end())>);
static_assert(std::semiregular<decltype(CCSDSPacket{}.end())>);
static_assert(std::sentinel_for<decltype(CCSDSPacket{}.end()),
                                decltype(CCSDSPacket{}.begin())>);

// TODO: ask why this isn't working
// An iterator over the items of It, static_cast to type to
template <typename It, typename to> class static_cast_iterator {
  using iterator_category = std::forward_iterator_tag;

private:
  It it;

public:
  static_cast_iterator(It it) : it{it} {}

  auto operator*() { return static_cast<to>(*it); }

  static_cast_iterator &operator++() {
    it++;
    return *this;
  }
  static_cast_iterator operator++(int) const {
    static_cast_iterator tmp = *this;
    ++(*this);
    return tmp;
  }

  static_cast_iterator operator+(int x) {
    static_cast_iterator tmp = *this;
    tmp.it += x;
    return tmp;
  }

  friend auto operator==(const static_cast_iterator &a,
                         const static_cast_iterator &b) {
    return a.it == b.it;
  }

  friend auto operator!=(const static_cast_iterator &a,
                         const static_cast_iterator &b) {
    return a.it != b.it;
  }
};

template <typename SecondaryHeader, typename DataField>
auto operator<<(std::ostream &output,
                CCSDSPacket<SecondaryHeader, DataField> &packet)
    -> std::ostream & {
  std::ranges::copy(std::ranges::views::transform(
                        packet, [](auto x) { return static_cast<char>(x); }),
                    std::ostreambuf_iterator<char>{output});

  return output;
}

template <typename It, typename F> class map_iterator {
private:
  It it;
  F f;

public:
  map_iterator(It it, F f) : it{std::move(it)}, f{std::move(f)} {}

  auto operator*() { return f(*it); }
  void operator->() = delete;

  auto operator++() -> map_iterator & {
    ++it;
    return *this;
  }
  auto operator++(int) -> map_iterator { return {it++, f}; }

  friend auto operator==(map_iterator const &, map_iterator const &)
      -> bool = default;

  using difference_type = std::ptrdiff_t;
  using value_type = decltype(*std::declval<map_iterator>());
  using pointer = void;
  using reference = void;
  using iterator_category = std::forward_iterator_tag;
};

// TODO: write copy_n_or_until_end

template <typename SecondaryHeader, typename DataField>
auto operator>>(std::istream &input,
                CCSDSPacket<SecondaryHeader, DataField> &packet)
    -> std::istream & {
  if (input.good()) {
    std::copy_n(map_iterator{std::istreambuf_iterator<char>{input},
                             [](auto x) { return static_cast<std::byte>(x); }},
                sizeof(packet.primary_header), packet.primary_header.begin());
    // std::cerr << "prev data_len(): " << packet.data_len() << '\n';

    // TODO: write a lazily evaluating sequential_iterator to work around the fact
    // that istreambuf_iterator only peeks to get the data
    char c;
    if (! input.get(c)) {
      return input;
    }

    auto sec_hdr_size =
        packet.sec_hdr_flag() ? packet.secondary_header.size() : 0;
    std::copy_n(map_iterator{std::istreambuf_iterator<char>{input},
                             [](auto x) { return static_cast<std::byte>(x); }},
                sec_hdr_size, packet.secondary_header.begin());
    if (packet.sec_hdr_flag()) {
      // std::cerr << "first byte of sec_hdr: " << static_cast<int>(*packet.secondary_header.begin()) << '\n';
    }

    // TODO: fix this like above
    if (! input.get(c)) {
      return input;
    }
    
    auto data_len = packet.data_len() - sec_hdr_size +
                    1; // Length 0 is used to mean a single byte
    //std::cerr << "Inputting packet.  data_len: " << data_len << '\n';
    // std::cerr << "data_len(): " << packet.data_len() << '\n';
    packet.data_field = DataField{data_len};
    input >> packet.data_field;
  }

  return input;
}
