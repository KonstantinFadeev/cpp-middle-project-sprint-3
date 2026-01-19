#pragma once

#include <algorithm>
#include <functional>
#include <span>
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
    return [... ps = std::move(preds)](const Book &book) { return (... && ps(book)); };
}

template <BookPredicate... Preds>
auto any_of(Preds... preds) {
    return [... ps = std::move(preds)](const Book &book) { return (... || ps(book)); };
}

template <BookIterator Iter, BookSentinel<Iter> Sent, BookPredicate Pred>
[[nodiscard]] std::vector<std::reference_wrapper<const Book>> filterBooks(Iter first, Sent last, Pred predicate) {
    std::vector<std::reference_wrapper<const Book>> result;
    std::for_each(first, last, [&result, &predicate](const Book &book) {
        if (predicate(book)) {
            result.emplace_back(std::cref(book));
        }
    });
    return result;
}

template <BookPredicate Pred>
[[nodiscard]] std::vector<std::reference_wrapper<const Book>> filterBooksSpan(std::span<const Book> books,
                                                                              Pred predicate) {
    return filterBooks(books.begin(), books.end(), predicate);
}

}  // namespace bookdb