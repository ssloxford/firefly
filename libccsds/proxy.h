#include <iostream>
#include <optional>
#include <type_traits>

template <typename Get, typename Set> class Proxy {
  private:
    Get get;
    Set set;

    using type = std::invoke_result_t<Get>;

  public:
    Proxy(Get get, Set set) : get{std::move(get)}, set{std::move(set)} {}

    operator type() { return get(); }

    void operator=(type x) { set(x); }

    Proxy(Proxy const &) = delete;
    Proxy(Proxy const &&) = delete;
    Proxy& operator=(Proxy const &) = delete;
    Proxy& operator=(Proxy &&) = delete;
};
