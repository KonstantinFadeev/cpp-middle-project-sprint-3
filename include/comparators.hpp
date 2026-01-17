#pragma once

#include "book.hpp"

namespace bookdb::comp {

struct LessByAuthor {
    constexpr bool operator()(const Book &lhs, const Book &rhs) const { return lhs.author < rhs.author; }
};

struct LessByPopularity {
    constexpr bool operator()(const Book &lhs, const Book &rhs) const { return lhs.read_count < rhs.read_count; }
};

struct LessByRating {
    constexpr bool operator()(const Book &lhs, const Book &rhs) const { return lhs.rating < rhs.rating; }
};

}  // namespace bookdb::comp