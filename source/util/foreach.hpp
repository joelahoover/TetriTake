#include <cstddef>
#include <tuple>
#include <utility>

// This came from http://codereview.stackexchange.com/a/67394
//
// Example usage:
//   for_each(std::make_tuple(1, 'k', true, 4.5f), [](auto x) {
//       std::cout << x << std::endl;
//   });
//

template <typename Tuple, typename F, std::size_t ...Indices>
void for_each_impl(Tuple&& tuple, F&& f, std::index_sequence<Indices...>) {
    using swallow = int[];
    (void)swallow{1,
        (f(std::get<Indices>(std::forward<Tuple>(tuple))), void(), int{})...
    };
}

template <typename Tuple, typename F>
void for_each(Tuple&& tuple, F&& f) {
    constexpr std::size_t N = std::tuple_size<std::remove_reference_t<Tuple>>::value;
    for_each_impl(std::forward<Tuple>(tuple), std::forward<F>(f),
                  std::make_index_sequence<N>{});
}


