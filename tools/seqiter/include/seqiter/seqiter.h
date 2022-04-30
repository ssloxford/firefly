#include <algorithm>
#include <array>
#include <iterator>
#include <tuple>
#include <type_traits>

template <typename It>
struct it_pair {
  It _begin;
  It _end;

  it_pair(It _begin, It _end)
      : _begin{std::move(_begin)}, _end{std::move(_end)} {}

  it_pair(auto &x) : _begin{x.begin()}, _end{x.end()} {}

  auto begin() const { return _begin; }
  auto begin() { return _begin; }
  auto end() const { return _end; }
  auto end() { return _end; }

  friend auto operator==(it_pair const &, it_pair const &) -> bool = default;
};

it_pair(auto &x) -> it_pair<decltype(x.begin())>;

auto recursive_arrow(auto &&x) {
  if constexpr (std::is_pointer_v<
                    decltype(std::forward<decltype(x)>(x).operator->())>) {
    return std::forward<decltype(x)>(x).operator->();
  } else {
    return recursive_arrow(std::forward<decltype(x)>(x).operator->());
  }
}

template <typename Reference, typename... It>
struct sequential_iterators_t {
 private:
  using ranges_t = std::tuple<it_pair<It>...>;

  ranges_t ranges;
  std::size_t index = 0;

  template <typename Ret, typename F>
  static auto visit(sequential_iterators_t const &self, F &&f) -> Ret {
    static constexpr auto fs =
        []<std::size_t... Indices>(std::index_sequence<Indices...>) {
      return std::array<auto(*)(F &&, ranges_t const &)->Ret, sizeof...(It)>{
          [](F &&f, ranges_t const &ranges) -> Ret {
            return std::forward<F>(f)(std::get<Indices>(ranges));
          }...};
    }
    (std::index_sequence_for<It...>{});
    return fs[self.index](std::forward<F>(f), self.ranges);
  }

  template <typename Ret, typename F>
  static auto visit(sequential_iterators_t &self, F &&f) -> Ret {
    static constexpr auto fs =
        []<std::size_t... Indices>(std::index_sequence<Indices...>) {
      return std::array<auto(*)(F &&, ranges_t &)->Ret, sizeof...(It)>{
          [](F &&f, ranges_t &ranges) -> Ret {
            return std::forward<F>(f)(std::get<Indices>(ranges));
          }...};
    }
    (std::index_sequence_for<It...>{});
    return fs[self.index](std::forward<F>(f), self.ranges);
  }

  sequential_iterators_t(ranges_t ranges, std::size_t index)
      : ranges{std::move(ranges)}, index{index} {}

 public:
  using reference = Reference;
  using value_type = std::remove_cvref_t<reference>;
  using pointer = value_type *;  // TODO maybe this should be more general

  static_assert((std::is_same_v<value_type, typename std::iterator_traits<It>::value_type> && ...),
                "Iterators do not all return correct value_type");

  sequential_iterators_t(it_pair<It>... ranges)
      : ranges{std::move(ranges)...} {}

  auto operator*() const -> reference {
    return visit<reference>(*this,
                            [](auto &x) -> reference { return *x.begin(); });
  }

  auto operator*() -> reference {
    return visit<reference>(*this,
                            [](auto &x) -> reference { return *x.begin(); });
  }

  auto operator->() const -> pointer {
    return visit<pointer>(
        *this, [](auto &x) -> pointer { return recursive_arrow(x.begin()); });
  }

  auto operator->() -> pointer {
    return visit<pointer>(
        *this, [](auto &x) -> pointer { return recursive_arrow(x.begin()); });
  }

  auto operator++() -> sequential_iterators_t & {
    visit<void>(*this, [](auto &x) {
            ++(x._begin);
         });

    while (index < sizeof...(It) && visit<bool>(*this, [](auto &x) {
         return x.begin() == x.end();
        })) {
      index += 1;
    }
    return *this;
  }

  auto operator++(int) -> sequential_iterators_t {
    auto tmp = *this;
    this->operator++();
    return tmp;
  }

  auto operator+(int const x) const -> sequential_iterators_t {
    auto tmp = *this;
    for (int i=0; i<x; i++)
      tmp++;
    return tmp;
  }

  friend auto operator==(sequential_iterators_t const &lhs,
                         sequential_iterators_t const &rhs) -> bool {
    if (lhs.index == rhs.index) {
      static constexpr auto fs =
          []<std::size_t... Indices>(std::index_sequence<Indices...>) {
        return std::array<auto(*)(ranges_t const &, ranges_t const &)->bool,
                          sizeof...(It)>{
            [](ranges_t const &ranges_lhs, ranges_t const &ranges_rhs) -> bool {
              return std::get<Indices>(ranges_lhs) ==
                     std::get<Indices>(ranges_rhs);
            }...};
      }
      (std::index_sequence_for<It...>{});
      for (std::size_t index = lhs.index; index < sizeof...(It); index += 1) {
        if (!fs[index](lhs.ranges, rhs.ranges)) {
          return false;
        }
      }
      return true;
    } else {
      return false;
    }
  }

  auto begin() const -> sequential_iterators_t { return *this; }
  auto begin() -> sequential_iterators_t { return *this; }

  auto end() const -> sequential_iterators_t { return {ranges, sizeof...(It)}; }

  auto end() -> sequential_iterators_t { return {ranges, sizeof...(It)}; }
};

// Function TAD works better than CTAD
template <typename Reference, typename... It>
auto sequential_iterators(it_pair<It>... ranges) {
  return sequential_iterators_t<Reference, It...>{std::move(ranges)...};
}
