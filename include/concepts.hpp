#pragma once

#include <concepts>
#include <iterator>
#include <ranges>

#include "book.hpp"

namespace bookdb {

template <typename T>
concept BookContainerLike = std::ranges::range<T> && std::same_as<std::ranges::range_value_t<T>, Book>;

template <typename T>
concept BookIterator = std::input_or_output_iterator<T> && requires(T it) {
    { *it } -> std::convertible_to<const Book &>;
};

template <typename S, typename I>
concept BookSentinel = std::sentinel_for<S, I> && BookIterator<I>;

template <typename P>
concept BookPredicate = requires(P pred, const Book &book) {
    { pred(book) } -> std::convertible_to<bool>;
};

template <typename C>
concept BookComparator = requires(C comp, const Book &a, const Book &b) {
    { comp(a, b) } -> std::convertible_to<bool>;
};

}  // namespace bookdb