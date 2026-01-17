#include <gtest/gtest.h>

#include <algorithm>

#include "book_database.hpp"
#include "comparators.hpp"

using namespace bookdb;

TEST(BookDatabaseTest, EmplaceBackAndSize) {
    BookDatabase<> db;
    EXPECT_EQ(db.size(), 0);
    EXPECT_TRUE(db.empty());

    db.EmplaceBack("1984", "George Orwell", 1949, Genre::SciFi, 4.5, 100);
    EXPECT_EQ(db.size(), 1);
    EXPECT_FALSE(db.empty());

    db.EmplaceBack("Animal Farm", "George Orwell", 1945, Genre::Fiction, 4.4, 90);
    EXPECT_EQ(db.size(), 2);
}

TEST(BookDatabaseTest, UniqueAuthors) {
    BookDatabase<> db;

    db.EmplaceBack("1984", "George Orwell", 1949, Genre::SciFi, 4.5, 100);
    db.EmplaceBack("Animal Farm", "George Orwell", 1945, Genre::Fiction, 4.4, 90);
    db.EmplaceBack("Brave New World", "Aldous Huxley", 1932, Genre::SciFi, 4.3, 80);

    const auto &authors = db.GetAuthors();
    EXPECT_EQ(authors.size(), 2);
    EXPECT_TRUE(authors.contains("George Orwell"));
    EXPECT_TRUE(authors.contains("Aldous Huxley"));
}

TEST(BookDatabaseTest, InitializerListConstructor) {
    BookDatabase<> db = {Book{"1984", "George Orwell", 1949, Genre::SciFi, 4.5, 100},
                         Book{"The Great Gatsby", "F. Scott Fitzgerald", 1925, Genre::Fiction, 4.7, 120},
                         Book{"To Kill a Mockingbird", "Harper Lee", 1960, Genre::Fiction, 4.8, 150}};

    EXPECT_EQ(db.size(), 3);
    EXPECT_EQ(db.GetAuthors().size(), 3);
}

TEST(BookDatabaseTest, IteratorAccess) {
    BookDatabase<> db;
    db.EmplaceBack("1984", "George Orwell", 1949, Genre::SciFi, 4.5, 100);
    db.EmplaceBack("Animal Farm", "George Orwell", 1945, Genre::Fiction, 4.4, 90);

    int count = 0;
    for (const auto &book : db) {
        EXPECT_FALSE(book.title.empty());
        EXPECT_FALSE(book.author.empty());
        count++;
    }
    EXPECT_EQ(count, 2);
}

TEST(BookDatabaseTest, SortWithComparators) {
    BookDatabase<> db;
    db.EmplaceBack("1984", "George Orwell", 1949, Genre::SciFi, 4.5, 100);
    db.EmplaceBack("Animal Farm", "George Orwell", 1945, Genre::Fiction, 4.4, 90);
    db.EmplaceBack("Brave New World", "Aldous Huxley", 1932, Genre::SciFi, 4.3, 80);

    std::sort(db.begin(), db.end(), comp::LessByRating{});
    EXPECT_EQ(db.begin()->rating, 4.3);

    std::sort(db.begin(), db.end(), comp::LessByPopularity{});
    EXPECT_EQ(db.begin()->read_count, 80);
}

TEST(BookDatabaseTest, PushBackWithMove) {
    BookDatabase<> db;
    Book book{"1984", "George Orwell", 1949, Genre::SciFi, 4.5, 100};
    db.PushBack(std::move(book));

    EXPECT_EQ(db.size(), 1);
    EXPECT_EQ(db.begin()->title, "1984");
}

TEST(BookDatabaseTest, Clear) {
    BookDatabase<> db;
    db.EmplaceBack("1984", "George Orwell", 1949, Genre::SciFi, 4.5, 100);
    db.EmplaceBack("Animal Farm", "George Orwell", 1945, Genre::Fiction, 4.4, 90);

    EXPECT_EQ(db.size(), 2);
    EXPECT_EQ(db.GetAuthors().size(), 1);

    db.Clear();

    EXPECT_EQ(db.size(), 0);
    EXPECT_EQ(db.GetAuthors().size(), 0);
    EXPECT_TRUE(db.empty());
}
