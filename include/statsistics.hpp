#pragma once

#include <algorithm>
#include <flat_map>
#include <functional>
#include <iterator>
#include <numeric>
#include <random>
#include <stdexcept>
#include <string_view>
#include <vector>

#include "book_database.hpp"

#include <print>

namespace bookdb {

template <BookContainerLike T, typename Comparator = TransparentStringLess>
auto buildAuthorHistogramFlat(const BookDatabase<T> &cont, Comparator comp = {}) {
    std::flat_map<std::string, int, Comparator> histogram(comp);
    for (const auto &book : cont) {
        histogram[std::string(book.author)]++;
    }
    return histogram;
}

template <BookIterator Iter, BookSentinel<Iter> Sent>
auto calculateGenreRatings(Iter first, Sent last) {
    std::flat_map<Genre, std::pair<double, int>> temp;
    for (auto it = first; it != last; ++it) {
        temp[it->genre].first += it->rating;
        temp[it->genre].second++;
    }

    std::flat_map<Genre, double> result;
    for (const auto &[genre, pair] : temp) {
        result[genre] = pair.first / pair.second;
    }
    return result;
}

template <BookContainerLike T>
double calculateAverageRating(const BookDatabase<T> &db) {
    if (db.empty()) {
        return 0.0;
    }
    double sum =
        std::accumulate(db.begin(), db.end(), 0.0, [](double acc, const Book &book) { return acc + book.rating; });
    return sum / db.size();
}

template <BookContainerLike T>
std::vector<std::reference_wrapper<const Book>> sampleRandomBooks(const BookDatabase<T> &db, size_t n) {
    std::vector<std::reference_wrapper<const Book>> result;
    if (n >= db.size()) {
        for (const auto &book : db) {
            result.emplace_back(std::cref(book));
        }
        return result;
    }

    std::vector<size_t> indices(db.size());
    std::iota(indices.begin(), indices.end(), 0);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(indices.begin(), indices.end(), gen);

    auto it = db.begin();
    for (size_t i = 0; i < n; ++i) {
        result.emplace_back(std::cref(*std::next(db.begin(), indices[i])));
    }
    return result;
}

template <BookContainerLike T, BookComparator Comp>
std::vector<std::reference_wrapper<const Book>> getTopNBy(BookDatabase<T> &db, size_t n, Comp comp) {
    std::sort(db.begin(), db.end(), comp);

    std::vector<std::reference_wrapper<const Book>> result;
    size_t count = std::min(n, db.size());
    auto it = db.end();
    for (size_t i = 0; i < count; ++i) {
        --it;
        result.emplace_back(std::cref(*it));
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
