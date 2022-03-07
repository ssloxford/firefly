#include <type_traits>
#include <utility>

template <typename Get, typename Set> class Proxy {
  private:
    Get get;
    Set set;

    using type = std::invoke_result_t<Get>;

  public:
    Proxy(Get get, Set set) : get{std::move(get)}, set{std::move(set)} {}

    operator type() { return get(); }

    void operator=(std::remove_cvref_t<type> x) { set(x); }

    auto begin() & -> decltype(auto) {
      using std::begin;
      return begin(this.get());
    }

    auto begin() && -> decltype(auto) {
      using std::begin;
      return begin(this->get());
    }

    auto end() & -> decltype(auto) {
      using std::end;
      return end(this.get());
    }

    auto end() && -> decltype(auto) {
      using std::end;
      return end(this->proxy.get());
    }

    Proxy(Proxy const &) = delete;
    Proxy(Proxy const &&) = delete;
    Proxy& operator=(Proxy const &) = delete;
    Proxy& operator=(Proxy &&) = delete;
};
