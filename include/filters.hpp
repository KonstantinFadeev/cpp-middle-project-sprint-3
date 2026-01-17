#pragma once

#include <algorithm>
#include <functional>
#include <vector>

#include "book.hpp"
#include "concepts.hpp"

namespace bookdb {

inline auto YearBetween(int from, int to) {
    return [from, to](const Book &book) { return book.year >= from && book.year <= to; };
}

inline auto RatingAbove(double min_rating) {
    return [min_rating](const Book &book) { return book.rating >= min_rating; };
}

inline auto GenreIs(Genre genre) {
    return [genre](const Book &book) { return book.genre == genre; };
}

template <BookPredicate... Preds>
auto all_of(Preds... preds) {
    return [=](const Book &book) { return (... && preds(book)); };
}

template <BookPredicate... Preds>
auto any_of(Preds... preds) {
    return [=](const Book &book) { return (... || preds(book)); };
}

template <BookIterator Iter, BookSentinel<Iter> Sent, BookPredicate Pred>
std::vector<std::reference_wrapper<const Book>> filterBooks(Iter first, Sent last, Pred predicate) {
    std::vector<std::reference_wrapper<const Book>> result;
    for (auto it = first; it != last; ++it) {
        if (predicate(*it)) {
            result.emplace_back(std::cref(*it));
        }
    }
    return result;
}

}  // namespace bookdb