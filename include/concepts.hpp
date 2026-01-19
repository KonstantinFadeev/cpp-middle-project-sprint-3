#pragma once

#include <concepts>
#include <iterator>
#include <ranges>

#include "book.hpp"

namespace bookdb {

template <typename T>
concept BookContainerLike =
    std::ranges::range<T> && std::same_as<std::ranges::range_value_t<T>, Book> && requires(T container, Book book) {
        { container.size() } -> std::convertible_to<std::size_t>;
        { container.empty() } -> std::convertible_to<bool>;
        { container.begin() };
        { container.end() };
        { container.cbegin() };
        { container.cend() };
        { container.push_back(std::move(book)) } -> std::same_as<void>;
        {
            container.emplace_back(std::declval<std::string>(), std::declval<std::string_view>(), std::declval<int>(),
                                   std::declval<Genre>(), std::declval<double>(), std::declval<int>())
        };
        { container.clear() } -> std::same_as<void>;
        { container.back() } -> std::convertible_to<Book &>;
    };

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