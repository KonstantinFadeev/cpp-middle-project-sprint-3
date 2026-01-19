#pragma once

#include <algorithm>
#include <execution>
#include <flat_map>
#include <functional>
#include <iterator>
#include <numeric>
#include <random>
#include <span>
#include <stdexcept>
#include <string_view>
#include <vector>

#include "book_database.hpp"

#include <print>

namespace bookdb {

template <BookContainerLike T, typename Comparator = TransparentStringLess>
[[nodiscard]] auto buildAuthorHistogramFlat(const BookDatabase<T> &cont, Comparator comp = {}) {
    std::flat_map<std::string, int, Comparator> histogram(comp);
    for (const auto &book : cont) {
        auto it = histogram.find(book.author);
        if (it != histogram.end()) {
            it->second++;
        } else {
            histogram.emplace(std::string(book.author), 1);
        }
    }
    return histogram;
}

template <BookIterator Iter, BookSentinel<Iter> Sent>
[[nodiscard]] auto calculateGenreRatings(Iter first, Sent last) {
    std::flat_map<Genre, std::pair<double, int>> temp;
    std::for_each(first, last, [&temp](const Book &book) {
        auto [map_it, inserted] = temp.try_emplace(book.genre, book.rating, 1);
        if (!inserted) {
            map_it->second.first += book.rating;
            map_it->second.second++;
        }
    });

    std::flat_map<Genre, double> result;
    for (const auto &[genre, pair] : temp) {
        result.insert({genre, pair.first / pair.second});
    }
    return result;
}

template <BookIterator Iter, BookSentinel<Iter> Sent>
double calculateAverageRating(Iter first, Sent last) {
    if (first == last) {
        return 0.0;
    }
    auto count = std::distance(first, last);
    double sum = std::transform_reduce(first, last, 0.0, std::plus<>{}, [](const Book &book) { return book.rating; });
    return sum / count;
}

template <BookContainerLike T>
double calculateAverageRating(const BookDatabase<T> &db) {
    return calculateAverageRating(db.begin(), db.end());
}

template <BookContainerLike T>
[[nodiscard]] std::vector<std::reference_wrapper<const Book>> sampleRandomBooks(const BookDatabase<T> &db, size_t n) {
    std::random_device rd;
    std::mt19937 gen(rd());

    size_t count = std::min(n, db.size());
    std::vector<std::reference_wrapper<const Book>> result;
    result.reserve(count);

    if (count == db.size()) {
        for (const auto &book : db) {
            result.emplace_back(std::cref(book));
        }
        return result;
    }

    std::vector<const Book *> all_ptrs;
    all_ptrs.reserve(db.size());
    for (const auto &book : db) {
        all_ptrs.push_back(&book);
    }

    std::vector<const Book *> sampled;
    sampled.reserve(count);
    std::sample(all_ptrs.begin(), all_ptrs.end(), std::back_inserter(sampled), count, gen);

    for (auto ptr : sampled) {
        result.emplace_back(std::cref(*ptr));
    }

    return result;
}

template <BookContainerLike T, BookComparator Comp>
[[nodiscard]] std::vector<std::reference_wrapper<const Book>> getTopNBy(BookDatabase<T> &db, size_t n, Comp comp) {
    size_t count = std::min(n, db.size());

    auto inverted_comp = [&comp](const Book &a, const Book &b) { return comp(b, a); };
    std::partial_sort(db.begin(), db.begin() + count, db.end(), inverted_comp);

    std::vector<std::reference_wrapper<const Book>> result;
    result.reserve(count);
    for (size_t i = 0; i < count; ++i) {
        result.emplace_back(std::cref(db.begin()[i]));
    }
    return result;
}

}  // namespace bookdb

namespace std {
template <typename K, typename V, typename C>
struct formatter<std::flat_map<K, V, C>> {
    template <typename FormatContext>
    auto format(const std::flat_map<K, V, C> &map, FormatContext &fc) const {
        format_to(fc.out(), "{{");
        bool first = true;
        for (const auto &[key, value] : map) {
            if (!first) {
                format_to(fc.out(), ", ");
            }
            first = false;
            format_to(fc.out(), "{}: {}", key, value);
        }
        format_to(fc.out(), "}}");
        return fc.out();
    }

    constexpr auto parse(format_parse_context &ctx) { return ctx.begin(); }
};
}  // namespace std
