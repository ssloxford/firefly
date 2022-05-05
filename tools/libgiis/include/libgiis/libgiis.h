#include <array>
#include <cstdint>
#include <climits>
#include <numeric>
#include <optional>
#include <string>

// Testing
#include <signal.h>

#include "getsetproxy/proxy.h"

// TODO: work out which bits are giis-specific, modis-specific, modis-aqua-specific, and shared with girs

namespace _libgiis_impl {
  // Implements an int array of word size wordlen (which currently must be 12)
  template <std::size_t N, std::size_t wordlen = 12>
  struct bitlen_int_array {
    // TODO: how to do this?
    static_assert(wordlen == 12); // A future implementation should genericise this
    static_assert(CHAR_BIT == 8);

  private:
    // TODO: ensure this works more generally when N*12 doesn't divide cleanly
    std::array<std::byte, (N*12)/8> data = {};

  public:
    auto size() const -> int {
      return N;
    }

    // TODO: what does this first ref mean?
    auto operator[](std::size_t pos) const -> int {
      // We assume that a byte is CHAR_BIT bits
      if (pos >= size()) {
        throw std::invalid_argument(
          std::string("data_word cannot exceed ") + std::to_string(size()));
      }

      // Extract the correct word from the array
      // Find the first bit
      const auto [index, offset] = std::div(pos*wordlen, CHAR_BIT);

      if (offset <= 4) {
        // word split across two bytes
        //std::cerr << "word split across two: " << index << "," << offset << "\n";
        //std::cerr << "index: " << index << '\n';
        //std::cerr << std::to_integer<int>(data.at(index)) << ":" << std::to_integer<int>(data.at(index+1)) << "\n";
        auto x = 
          ((std::to_integer<int>(data.at(index)) & ((1 << (CHAR_BIT-offset))-1)) << (wordlen-(CHAR_BIT-offset)))
          | (std::to_integer<int>(data.at(index+1)) >> CHAR_BIT-(wordlen-(CHAR_BIT-offset)));

        //std::cerr << "output: " << x << '\n';
        return x;

      } else {
        // word split across three bytes
        //std::cerr << "### WARNING split across three impossible\n";
        return
          (std::to_integer<int>(data[index]) & ((1 << (CHAR_BIT-offset))-1) << wordlen-(CHAR_BIT-offset))
          | std::to_integer<int>(data[index+1]) << 4-(CHAR_BIT-offset)
          | std::to_integer<int>(data[index+2]) >> CHAR_BIT-(wordlen-((CHAR_BIT-offset)+CHAR_BIT));
      }
    }

    auto operator[](std::size_t pos) {
      return Proxy{
        [this,pos]() -> decltype(auto) { return std::as_const(*this)[pos]; },
        // TODO: investigate why this is not mutated
        [this,pos](int const value) mutable {
          // We assume that a byte is CHAR_BIT bits
          if (pos >= size()) {
            throw std::invalid_argument(
              std::string("data_word cannot exceed ") + std::to_string(size()));
          }

          // Extract the correct word from the array
          const auto [index, offset] = std::div(pos*wordlen, CHAR_BIT);

          // Set the low bits at the index
          data[index] &= std::byte(~0 << (CHAR_BIT-offset));
          data[index] |= std::byte((value >> wordlen-(CHAR_BIT-offset)) & ((1 << (CHAR_BIT-offset))-1));

          if (offset <= 4) {
            // checksum split across two bytes, so set the entire first byte
            data[index+1] &= std::byte((1 << CHAR_BIT-(wordlen-(CHAR_BIT-offset))) -1); // Clear the high bits
            data[index+1] |= std::byte(value << CHAR_BIT-(wordlen-(CHAR_BIT-offset)));
          } else {
            // checksum split across three bytes
            data[index+1] = std::byte(value >> 4-(CHAR_BIT-offset));  // Entire second byte filled
            data[index+2] &= std::byte((1 << CHAR_BIT-(wordlen-((CHAR_BIT-offset)+CHAR_BIT)))-1); // Clear the high bits
            data[index+2] |= std::byte(value << CHAR_BIT-(wordlen-((CHAR_BIT-offset)+CHAR_BIT)));
          }
        }
      };
    }

 private:
   struct Iterator {
   private:
     int index = 0;
     _libgiis_impl::bitlen_int_array<N, wordlen> *parent;

   public:
     Iterator() = default;
     Iterator(int index, _libgiis_impl::bitlen_int_array<N, wordlen> *parent) : index{index}, parent{parent} {}

     auto operator*() const -> int { return (*parent)[index]; }

     Iterator& operator++() { index++; return *this; }

     Iterator operator++(int) { Iterator tmp = *this; ++(*this); return tmp; }

     Iterator operator+(int x) {
       Iterator tmp = *this;
       for (int i=0; i<x; i++)
         tmp++;
       return tmp;
     }

     friend auto operator== (const Iterator& a, const Iterator& b) -> bool = default;

     using value_type = int;
     using difference_type = std::ptrdiff_t;
     using pointer = void;
     using reference = int;
     using iterator_category = std::forward_iterator_tag;
   };

  public:
    auto begin() -> Iterator { return Iterator(0, this); }
    auto end() -> Iterator { return Iterator(size(), this); }

    auto begin() const -> Iterator { return Iterator(0, this); }
    auto end() const -> Iterator { return Iterator(size(), this); }
  };
}

namespace giis {
  // Secondary header fields
  constexpr int TIME_TAG_LEN = 64;
  constexpr int QUICK_LOOK_LEN = 1;
  constexpr int PKT_TYPE_LEN = 3;
  constexpr int SCAN_COUNT_LEN = 3;
  constexpr int MIR_SIDE_LEN = 1;

  // Data zone fields
  constexpr int SRC_INDENT_LEN = 12;
  constexpr int FPA_AEM_CONFIG_LEN = 10;
  constexpr int SCI_STATE_LEN = 1;
  constexpr int SCI_ABNORM_LEN = 1;
  constexpr int DATA_FIELD_DAY_ENGINEERING_LEN = 4980;
  constexpr int DATA_FIELD_NIGHT_LEN = 2052; // This may be off-by-one as night packet contains a fill 12-bit word
  constexpr int CHECKSUM_LEN = 12;

  struct SecondaryHeader {
  private:
    #pragma pack(push, 1)
    struct Impl {
      friend SecondaryHeader;
    private:
      std::array<std::byte, giis::TIME_TAG_LEN/8> _time_tag = {};
      uint8_t _mir_side : giis::MIR_SIDE_LEN = 0;
      uint8_t _scan_count : giis::SCAN_COUNT_LEN = 0;
      uint8_t _pkt_type : giis::PKT_TYPE_LEN = 0;
      uint8_t _quick_look : giis::QUICK_LOOK_LEN = 0;
    };
    #pragma pack(pop)
    static_assert(std::is_trivially_copyable_v<Impl>,
                  "giis::SecondaryHeader is not trivially copyable");
    static_assert(std::is_standard_layout_v<Impl>,
                  "giis::SecondaryHeader is not a standard layout type");
    static_assert(sizeof(Impl) == 9,
                  "giis::SecondaryHeader is not of size 9 as in the spec");

    Impl header;

  public:
    SecondaryHeader() = default;
    SecondaryHeader(const char* data, std::size_t len) {
      // std::cerr << "### invoking constructor" << '\n';
      if (len != sizeof(Impl)) {
        std::cerr << "size: " << len << '\n';
        std::cerr << "correct size: " << sizeof(Impl) << '\n';
        throw std::invalid_argument(
          "giis::SecondaryHeader - invalid size"
          //"giis::DataField requires data of size "s + std::to_string(giis::DATA_FIELD_DAY_ENGINEERING_LEN) + " or "s + std::to_string(DATA_FIELD_NIGHT_LEN)
        );
      }
      // Copy in the first fields
      // std::cerr << "copying in " << len << '\n';

      std::memcpy(&header, data, len);
    }
    
    auto begin() { return reinterpret_cast<std::byte*>(&header); }
    auto end() { return reinterpret_cast<std::byte*>(&header) + sizeof(Impl); }

    auto begin() const { return reinterpret_cast<std::byte const * const>(&header); }
    auto end() const { return reinterpret_cast<std::byte const * const>(&header) + sizeof(Impl); }

    // TODO: getters and setters for time_tag

    auto mir_side() const & {
      return static_cast<int>(header._mir_side);
    }

    auto mir_side() & {
      return Proxy{
        [this]() -> decltype(auto) { return std::as_const(*this).mir_side(); },
        [this](int x) { header._mir_side = x; }
      };
    }

    auto scan_count() const & {
      return static_cast<int>(header._scan_count);
    }

    auto scan_count() & {
      return Proxy{
        [this]() -> decltype(auto) { return std::as_const(*this).scan_count(); },
        [this](int x) { header._scan_count = x; }
      };
    }

    auto pkt_type() const & {
      return static_cast<int>(header._pkt_type);
    }

    auto pkt_type() & {
      return Proxy{
        [this]() -> decltype(auto) { return std::as_const(*this).pkt_type(); },
        [this](int x) { header._pkt_type = x; }
      };
    }

    auto quick_look() const & {
      return static_cast<int>(header._quick_look);
    }

    auto quick_look() & {
      return Proxy{
        [this]() -> decltype(auto) { return std::as_const(*this).quick_look(); },
        [this](int x) { header._quick_look = x; }
      };
    }

    constexpr auto size() const -> std::size_t {
      return sizeof(Impl);
    }
  };

  class DataField {
  private:
    #pragma pack(push, 1)
    template <int data_field_bit_len>
    struct Impl {
      friend DataField;
    private:
      uint8_t _src_ident_l = 0;
      uint16_t _fpa_aem_config_l : 4 = 0;
      uint16_t _src_ident_h : giis::SRC_INDENT_LEN - 8 = 0;
      uint16_t _sci_abnorm : giis::SCI_ABNORM_LEN = 0;
      uint16_t _sci_state : giis::SCI_STATE_LEN = 0;
      uint16_t _fpa_aem_config_h : giis::FPA_AEM_CONFIG_LEN - 4 = 0;
      mutable _libgiis_impl::bitlen_int_array<(data_field_bit_len + giis::CHECKSUM_LEN)/12> checksummed_data = {};
      static_assert((data_field_bit_len + giis::CHECKSUM_LEN) % 8 == 0,
                    "giis::DataField::Impl must have checksummed_data be an integer number of bytes");
    };
    #pragma pack(pop)
    // TODO: work out how to static_assert the property for all instances of the template
    static_assert(std::is_trivially_copyable_v<Impl<4>>,
                  "giis::DataField is not trivially copyable");
    static_assert(std::is_standard_layout_v<Impl<4>>,
                  "giis::DataField is not a standard layout type");
    static_assert(sizeof(Impl<12>) == (24 + 12 + giis::CHECKSUM_LEN)/8,
                  "giis::DataField is not of correct size");

    std::optional<std::variant<
      Impl<giis::DATA_FIELD_DAY_ENGINEERING_LEN>,
      Impl<giis::DATA_FIELD_NIGHT_LEN>
      >> data_field = std::nullopt; // TODO: use std::monostate variant
    mutable bool dirty_checksum = true;

  public:
    DataField() = default;

    DataField(std::size_t len) {
      if (len == sizeof(Impl<giis::DATA_FIELD_DAY_ENGINEERING_LEN>)) {
        data_field = Impl<giis::DATA_FIELD_DAY_ENGINEERING_LEN>();
      } else if (len == sizeof(Impl<giis::DATA_FIELD_NIGHT_LEN>)) {
        data_field = Impl<giis::DATA_FIELD_NIGHT_LEN>();
      } else {
        std::cerr << "size: " << len << '\n';
        std::cerr << "correct size: " << sizeof(Impl<giis::DATA_FIELD_DAY_ENGINEERING_LEN>) << '\n';
        throw std::invalid_argument(
          "giis::DataField - invalid size"
          //"giis::DataField requires data of size "s + std::to_string(giis::DATA_FIELD_DAY_ENGINEERING_LEN) + " or "s + std::to_string(DATA_FIELD_NIGHT_LEN)
        );
      }
    }

    DataField(const char* data, std::size_t len) : DataField{len} {
      // Copy in the first fields
      std::memcpy(&data_field.value(), data, len);
    }

    friend auto operator>>(std::istream & is, DataField & data_field) -> std::istream & {
      // std::cerr << "Reading data field size: " << data_field.size() << '\n';
      is.read(reinterpret_cast<char*>(&data_field.data_field.value()), data_field.size());
      // std::cerr << "Read data field size: " << is.gcount() << '\n';
      return is;
    }

    auto resize(std::size_t len) {
      //throw std::invalid_argument("giis::DataField cannot be resized");
    }

    auto begin() { 
      recalculate_checksum();
      return std::visit(
        [](auto & v) -> std::byte* {
          return reinterpret_cast<std::byte*>(&v);
        },
        data_field.value()
      );
    }
    auto end() { 
      return std::visit(
        [](auto & v) -> std::byte* {
          return reinterpret_cast<std::byte*>(&v) + sizeof(v);
        },
        data_field.value()
      );
    }

    // TODO: fix const-ness in recalculate_checksum with mutable data fields
    /*
    auto begin() const {
      recalculate_checksum();
      return reinterpret_cast<std::byte const * const>(&data_field);
    }
    auto end() const { return reinterpret_cast<std::byte const * const>(&data_field) + sizeof(data_field); }
   
    // TODO: define methods for getting and setting the other fields with proxy
    */

  private:
    // Extract the 12 bit data word from checksummed_data at byte index, offset offset


    auto set_data_word(const int index, const int offset, const int value) {
      // std::cerr << "setting data word " << index << "," << offset << "\n";

    }

  public:
    auto data_word(int const pos) const & -> int const {
      const auto checksummed_data_size = std::visit(
        [](auto const & v) -> int {
          return static_cast<int>(v.checksummed_data.size());
        },
        data_field.value()
      );
      
      if (pos >= checksummed_data_size - 2) {
        // TODO: replace with idiomatic bounds checking
        throw std::invalid_argument("giis::DataField - out of bounds access");
      }

      return std::visit(
        [pos](auto const & v) -> int { return v.checksummed_data[pos]; },
        data_field.value());
    }

    auto data_word(int const pos) & {
      return Proxy{
        [this,pos]() -> decltype(auto) { return std::as_const(*this).data_word(pos); },
        [this,pos](int value) { 
          const auto checksummed_data_size = std::visit(
            [](auto const & v) -> int {
              return static_cast<int>(v.checksummed_data.size());
            },
            data_field.value()
          );

          if (pos >= checksummed_data_size - 2) {
            // TODO: replace with idiomatic bounds checking
            throw std::invalid_argument("giis::DataField - out of bounds access");
          }

          dirty_checksum = true;

          std::visit(
            [pos,value](auto & v) { v.checksummed_data[pos] = value; },
            data_field.value());
        }
      };
    }

  private:
    auto data_word_offset(int const ifov, int const band, int const det, int const sample, bool const high) const -> int const {
      // spec: MODIS_UG.pdf p183-
      // TODO: bounds checking
      // TODO: ensure that only the second packet in the group can accees high ifovs, same with lows

      int ifov_offset = ((ifov-1) % 5)*83;  // 83 is number of samples per ifov column
      int band_offset = 0;
      if (band == 1 || band == 2) {
        band_offset = (band-1)*16 + (sample-1)*4;
      } else if (band >= 3 && band <= 7) {
        band_offset = 32 + (band-3)*4 + (sample-1)*2;
      } else if (band >= 8 && band <= 12) {
        band_offset = 52 + (band-8);
      } else if (band == 13 || band == 14) {
        band_offset = 57 + (band-13)*2;
        if (high)
          band_offset++;
      } else if (band >= 15 && band <= 36) {
        band_offset = 61 + (band-15);
      }

      // std::cerr << "calculated offset: " << ifov_offset + band_offset << '\n';
      return ifov_offset + band_offset;
    }

  public:
    auto data_word(int const ifov, int const band, int const det=1, int const sample=1, bool const high=false) const & -> int {
      return data_word(data_word_offset(ifov, band, det, sample, high));
    }

    auto data_word(int const ifov, int const band, int const det=1, int const sample=1, bool const high=false) & {
      return data_word(data_word_offset(ifov, band, det, sample, high));
    }

    auto checksum() const & {
      return std::visit(
        [](auto const & v) -> int {
          return v.checksummed_data[v.checksummed_data.size() - 1];
        },
        data_field.value());
    }

      //std::cerr << "first bit location: " << dv.quot << "," << dv.rem << std::endl;
      /*
      std::cerr << "checksummed_data size (bytes): " << checksummed_data_size << std::endl;
      std::cerr << "checksummed_data size (dozens): " << (checksummed_data_size*8)/12 << std::endl;
      std::cerr << "checksummed_data size (bits): " << checksummed_data_size*8 << std::endl;

      std::cerr << "value: " << value << std::endl;
      */

    auto checksum() & {
      return Proxy{
        [this]() -> decltype(auto) { return std::as_const(*this).checksum(); },
        [this](int value) {
          std::visit(
            [value](auto & v) { v.checksummed_data[v.checksummed_data.size() - 1] = value; },
            data_field.value());

          dirty_checksum = false;
        }
      };
    }

  public:
    // TODO: make private
    auto calculate_checksum() const -> int const {
      // This implements "formatter """exclusive xor""" " from the spec MODIS_UG.pdf
      // Derived from ocssw-src/src/pdsmerge/pdsinfo.c
      return std::visit(
        [](auto const & v) -> int {
          auto checksum = std::reduce(
            v.checksummed_data.begin(),
            v.checksummed_data.begin() + (v.checksummed_data.size() - 1)
          );
          checksum >>= 4;
          checksum &= 0xFFF;
          return checksum;
        },
        data_field.value()
      );
    }

  public:
    void recalculate_checksum() {
      checksum() = calculate_checksum();
      dirty_checksum = false;
    }

    auto validate_checksum() const -> bool const {
      return std::visit(
        [](auto const & v) -> int {
          return std::accumulate(
            v.checksummed_data.begin(),
            v.checksummed_data.end(),
            0,
            // WHY is there no std::logical_xor ???
            [](int a, int b) { return a^b; }
          );
        },
        data_field.value()
      ) == 0;
    }

    auto size() const -> int const {
      return std::visit(
        [](auto const & v) -> int {
          return sizeof(v);
        },
        data_field.value()
      );
    }
  };
}
