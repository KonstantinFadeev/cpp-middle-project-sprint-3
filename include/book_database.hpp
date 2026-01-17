#pragma once

#include <flat_set>
#include <initializer_list>
#include <print>
#include <string>
#include <string_view>
#include <unordered_set>
#include <vector>

#include "book.hpp"
#include "concepts.hpp"
#include "heterogeneous_lookup.hpp"

namespace bookdb {

template <BookContainerLike BookContainer = std::vector<Book>>
class BookDatabase {
public:
    using value_type = typename BookContainer::value_type;
    using size_type = typename BookContainer::size_type;
    using difference_type = typename BookContainer::difference_type;
    using reference = typename BookContainer::reference;
    using const_reference = typename BookContainer::const_reference;
    using pointer = typename BookContainer::pointer;
    using const_pointer = typename BookContainer::const_pointer;
    using iterator = typename BookContainer::iterator;
    using const_iterator = typename BookContainer::const_iterator;

    using AuthorContainer = std::unordered_set<std::string, TransparentStringHash, TransparentStringEqual>;

    BookDatabase() = default;

    BookDatabase(std::initializer_list<Book> init) {
        for (const auto &book : init) {
            PushBack(book);
        }
    }

    void Clear() {
        books_.clear();
        authors_.clear();
    }

    iterator begin() { return books_.begin(); }

    const_iterator begin() const { return books_.begin(); }

    const_iterator cbegin() const { return books_.cbegin(); }

    iterator end() { return books_.end(); }

    const_iterator end() const { return books_.end(); }

    const_iterator cend() const { return books_.cend(); }

    size_type size() const { return books_.size(); }

    bool empty() const { return books_.empty(); }

    void PushBack(const Book &book) {
        authors_.insert(std::string(book.author));
        books_.push_back(book);
    }

    void PushBack(Book &&book) {
        authors_.insert(std::string(book.author));
        books_.push_back(std::move(book));
    }

    template <typename... Args>
    void EmplaceBack(Args &&...args) {
        books_.emplace_back(std::forward<Args>(args)...);
        authors_.insert(std::string(books_.back().author));
    }

    const BookContainer &GetBooks() const { return books_; }

    const AuthorContainer &GetAuthors() const { return authors_; }

private:
    BookContainer books_;
    AuthorContainer authors_;
};

using AuthorContainerFlat = std::flat_set<std::string, TransparentStringLess>;

template <BookContainerLike T>
AuthorContainerFlat extractAuthorsFlat(const BookDatabase<T> &db) {
    AuthorContainerFlat authors;
    for (const auto &book : db) {
        authors.insert(std::string(book.author));
    }
    return authors;
}

}  // namespace bookdb

namespace std {
template <>
struct formatter<bookdb::BookDatabase<std::vector<bookdb::Book>>> {
    template <typename FormatContext>
    auto format(const bookdb::BookDatabase<std::vector<bookdb::Book>> &db, FormatContext &fc) const {
        format_to(fc.out(), "BookDatabase (size = {}): ", db.size());

        format_to(fc.out(), "Books:\n");
        for (const auto &book : db.GetBooks()) {
            format_to(fc.out(), "- {}\n", book);
        }

        format_to(fc.out(), "Authors:\n");
        for (const auto &author : db.GetAuthors()) {
            format_to(fc.out(), "- {}\n", author);
        }
        return fc.out();
    }

    constexpr auto parse(format_parse_context &ctx) {
        return ctx.begin();  // Просто игнорируем пользовательский формат
    }
};
}  // namespace std
