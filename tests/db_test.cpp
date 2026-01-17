#include <gtest/gtest.h>

#include <algorithm>

#include "book_database.hpp"
#include "comparators.hpp"
#include "filters.hpp"
#include "statsistics.hpp"

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

TEST(FiltersTest, YearBetween) {
    BookDatabase<> db;
    db.EmplaceBack("1984", "George Orwell", 1949, Genre::SciFi, 4.5, 100);
    db.EmplaceBack("The Great Gatsby", "F. Scott Fitzgerald", 1925, Genre::Fiction, 4.7, 120);
    db.EmplaceBack("To Kill a Mockingbird", "Harper Lee", 1960, Genre::Fiction, 4.8, 150);

    auto result = filterBooks(db.begin(), db.end(), YearBetween(1940, 1960));
    EXPECT_EQ(result.size(), 2);
}

TEST(FiltersTest, RatingAbove) {
    BookDatabase<> db;
    db.EmplaceBack("1984", "George Orwell", 1949, Genre::SciFi, 4.5, 100);
    db.EmplaceBack("Animal Farm", "George Orwell", 1945, Genre::Fiction, 4.4, 90);
    db.EmplaceBack("The Great Gatsby", "F. Scott Fitzgerald", 1925, Genre::Fiction, 4.7, 120);

    auto result = filterBooks(db.begin(), db.end(), RatingAbove(4.6));
    EXPECT_EQ(result.size(), 1);
    EXPECT_EQ(result[0].get().title, "The Great Gatsby");
}

TEST(FiltersTest, GenreIs) {
    BookDatabase<> db;
    db.EmplaceBack("1984", "George Orwell", 1949, Genre::SciFi, 4.5, 100);
    db.EmplaceBack("Animal Farm", "George Orwell", 1945, Genre::Fiction, 4.4, 90);
    db.EmplaceBack("Brave New World", "Aldous Huxley", 1932, Genre::SciFi, 4.3, 80);

    auto result = filterBooks(db.begin(), db.end(), GenreIs(Genre::SciFi));
    EXPECT_EQ(result.size(), 2);
}

TEST(FiltersTest, AllOf) {
    BookDatabase<> db;
    db.EmplaceBack("1984", "George Orwell", 1949, Genre::SciFi, 4.5, 100);
    db.EmplaceBack("Animal Farm", "George Orwell", 1945, Genre::Fiction, 4.4, 90);
    db.EmplaceBack("The Great Gatsby", "F. Scott Fitzgerald", 1925, Genre::Fiction, 4.7, 120);
    db.EmplaceBack("To Kill a Mockingbird", "Harper Lee", 1960, Genre::Fiction, 4.8, 150);

    auto result = filterBooks(db.begin(), db.end(), all_of(YearBetween(1940, 1960), RatingAbove(4.6)));
    EXPECT_EQ(result.size(), 1);
    EXPECT_EQ(result[0].get().title, "To Kill a Mockingbird");
}

TEST(FiltersTest, AnyOf) {
    BookDatabase<> db;
    db.EmplaceBack("1984", "George Orwell", 1949, Genre::SciFi, 4.5, 100);
    db.EmplaceBack("Animal Farm", "George Orwell", 1945, Genre::Fiction, 4.4, 90);
    db.EmplaceBack("The Great Gatsby", "F. Scott Fitzgerald", 1925, Genre::Fiction, 4.7, 120);

    auto result = filterBooks(db.begin(), db.end(), any_of(GenreIs(Genre::SciFi), RatingAbove(4.6)));
    EXPECT_EQ(result.size(), 2);
}

TEST(FiltersTest, EmptyDatabase) {
    BookDatabase<> db;
    auto result = filterBooks(db.begin(), db.end(), YearBetween(1900, 2000));
    EXPECT_EQ(result.size(), 0);
}

TEST(StatisticsTest, BuildAuthorHistogram) {
    BookDatabase<> db;
    db.EmplaceBack("1984", "George Orwell", 1949, Genre::SciFi, 4.5, 100);
    db.EmplaceBack("Animal Farm", "George Orwell", 1945, Genre::Fiction, 4.4, 90);
    db.EmplaceBack("Brave New World", "Aldous Huxley", 1932, Genre::SciFi, 4.3, 80);

    auto histogram = buildAuthorHistogramFlat(db);
    EXPECT_EQ(histogram.size(), 2);
    EXPECT_EQ(histogram.at("George Orwell"), 2);
    EXPECT_EQ(histogram.at("Aldous Huxley"), 1);
}

TEST(StatisticsTest, CalculateGenreRatings) {
    BookDatabase<> db;
    db.EmplaceBack("1984", "George Orwell", 1949, Genre::SciFi, 4.0, 100);
    db.EmplaceBack("Brave New World", "Aldous Huxley", 1932, Genre::SciFi, 5.0, 80);
    db.EmplaceBack("Animal Farm", "George Orwell", 1945, Genre::Fiction, 4.5, 90);

    auto ratings = calculateGenreRatings(db.begin(), db.end());
    EXPECT_EQ(ratings.size(), 2);
    EXPECT_DOUBLE_EQ(ratings.at(Genre::SciFi), 4.5);
    EXPECT_DOUBLE_EQ(ratings.at(Genre::Fiction), 4.5);
}

TEST(StatisticsTest, CalculateAverageRating) {
    BookDatabase<> db;
    db.EmplaceBack("1984", "George Orwell", 1949, Genre::SciFi, 4.0, 100);
    db.EmplaceBack("Animal Farm", "George Orwell", 1945, Genre::Fiction, 5.0, 90);
    db.EmplaceBack("Brave New World", "Aldous Huxley", 1932, Genre::SciFi, 3.0, 80);

    double avg = calculateAverageRating(db);
    EXPECT_DOUBLE_EQ(avg, 4.0);
}

TEST(StatisticsTest, CalculateAverageRatingEmptyDatabase) {
    BookDatabase<> db;
    double avg = calculateAverageRating(db);
    EXPECT_DOUBLE_EQ(avg, 0.0);
}

TEST(StatisticsTest, GetTopNBy) {
    BookDatabase<> db;
    db.EmplaceBack("1984", "George Orwell", 1949, Genre::SciFi, 4.5, 100);
    db.EmplaceBack("Animal Farm", "George Orwell", 1945, Genre::Fiction, 4.4, 90);
    db.EmplaceBack("Brave New World", "Aldous Huxley", 1932, Genre::SciFi, 4.8, 80);
    db.EmplaceBack("The Great Gatsby", "F. Scott Fitzgerald", 1925, Genre::Fiction, 4.2, 120);

    auto top = getTopNBy(db, 2, comp::LessByRating{});
    EXPECT_EQ(top.size(), 2);
    EXPECT_EQ(top[0].get().rating, 4.8);
    EXPECT_EQ(top[1].get().rating, 4.5);
}

TEST(StatisticsTest, GetTopNByMoreThanAvailable) {
    BookDatabase<> db;
    db.EmplaceBack("1984", "George Orwell", 1949, Genre::SciFi, 4.5, 100);
    db.EmplaceBack("Animal Farm", "George Orwell", 1945, Genre::Fiction, 4.4, 90);

    auto top = getTopNBy(db, 10, comp::LessByRating{});
    EXPECT_EQ(top.size(), 2);
}

TEST(StatisticsTest, SampleRandomBooks) {
    BookDatabase<> db;
    db.EmplaceBack("1984", "George Orwell", 1949, Genre::SciFi, 4.5, 100);
    db.EmplaceBack("Animal Farm", "George Orwell", 1945, Genre::Fiction, 4.4, 90);
    db.EmplaceBack("Brave New World", "Aldous Huxley", 1932, Genre::SciFi, 4.3, 80);

    auto sample = sampleRandomBooks(db, 2);
    EXPECT_EQ(sample.size(), 2);
}

TEST(StatisticsTest, SampleRandomBooksMoreThanAvailable) {
    BookDatabase<> db;
    db.EmplaceBack("1984", "George Orwell", 1949, Genre::SciFi, 4.5, 100);

    auto sample = sampleRandomBooks(db, 10);
    EXPECT_EQ(sample.size(), 1);
}

TEST(BookTest, ConstructorWithGenreEnum) {
    Book book{"1984", "George Orwell", 1949, Genre::SciFi, 4.5, 100};
    EXPECT_EQ(book.title, "1984");
    EXPECT_EQ(book.author, "George Orwell");
    EXPECT_EQ(book.year, 1949);
    EXPECT_EQ(book.genre, Genre::SciFi);
    EXPECT_DOUBLE_EQ(book.rating, 4.5);
    EXPECT_EQ(book.read_count, 100);
}

TEST(BookTest, ConstructorWithGenreString) {
    Book book{"1984", "George Orwell", 1949, "SciFi", 4.5, 100};
    EXPECT_EQ(book.genre, Genre::SciFi);

    Book book2{"Unknown Title", "Unknown Author", 2000, "InvalidGenre", 3.0, 50};
    EXPECT_EQ(book2.genre, Genre::Unknown);
}

TEST(BookTest, GenreFromString) {
    EXPECT_EQ(GenreFromString("Fiction"), Genre::Fiction);
    EXPECT_EQ(GenreFromString("NonFiction"), Genre::NonFiction);
    EXPECT_EQ(GenreFromString("SciFi"), Genre::SciFi);
    EXPECT_EQ(GenreFromString("Biography"), Genre::Biography);
    EXPECT_EQ(GenreFromString("Mystery"), Genre::Mystery);
    EXPECT_EQ(GenreFromString("InvalidGenre"), Genre::Unknown);
}

TEST(ComparatorsTest, LessByAuthor) {
    Book book1{"Title1", "Alice", 2000, Genre::Fiction, 4.0, 100};
    Book book2{"Title2", "Bob", 2001, Genre::Fiction, 4.5, 150};

    comp::LessByAuthor comparator;
    EXPECT_TRUE(comparator(book1, book2));
    EXPECT_FALSE(comparator(book2, book1));
}

TEST(ComparatorsTest, LessByPopularity) {
    Book book1{"Title1", "Author", 2000, Genre::Fiction, 4.0, 100};
    Book book2{"Title2", "Author", 2001, Genre::Fiction, 4.5, 150};

    comp::LessByPopularity comparator;
    EXPECT_TRUE(comparator(book1, book2));
    EXPECT_FALSE(comparator(book2, book1));
}

TEST(ComparatorsTest, LessByRating) {
    Book book1{"Title1", "Author", 2000, Genre::Fiction, 4.0, 100};
    Book book2{"Title2", "Author", 2001, Genre::Fiction, 4.5, 100};

    comp::LessByRating comparator;
    EXPECT_TRUE(comparator(book1, book2));
    EXPECT_FALSE(comparator(book2, book1));
}

TEST(HeterogeneousLookupTest, TransparentStringLess) {
    TransparentStringLess comp;
    EXPECT_TRUE(comp("apple", "banana"));
    EXPECT_FALSE(comp("banana", "apple"));
    EXPECT_FALSE(comp("apple", "apple"));
}

TEST(HeterogeneousLookupTest, TransparentStringEqual) {
    TransparentStringEqual eq;
    EXPECT_TRUE(eq("test", "test"));
    EXPECT_FALSE(eq("test", "other"));
}

TEST(HeterogeneousLookupTest, TransparentStringHash) {
    TransparentStringHash hasher;
    std::string str = "test";
    std::string_view sv = str;
    EXPECT_EQ(hasher(sv), hasher(str));
}

TEST(StatisticsTest, CalculateAverageRatingSpan) {
    std::vector<Book> books = {Book{"1984", "George Orwell", 1949, Genre::SciFi, 4.0, 100},
                               Book{"Animal Farm", "George Orwell", 1945, Genre::Fiction, 5.0, 90},
                               Book{"Brave New World", "Aldous Huxley", 1932, Genre::SciFi, 3.0, 80}};

    std::span<const Book> span(books);
    double avg = calculateAverageRatingSpan(span);
    EXPECT_DOUBLE_EQ(avg, 4.0);
}

TEST(FiltersTest, FilterBooksSpan) {
    std::vector<Book> books = {Book{"1984", "George Orwell", 1949, Genre::SciFi, 4.5, 100},
                               Book{"Animal Farm", "George Orwell", 1945, Genre::Fiction, 4.4, 90},
                               Book{"The Great Gatsby", "F. Scott Fitzgerald", 1925, Genre::Fiction, 4.7, 120}};

    std::span<const Book> span(books);
    auto result = filterBooksSpan(span, RatingAbove(4.6));
    EXPECT_EQ(result.size(), 1);
    EXPECT_EQ(result[0].get().title, "The Great Gatsby");
}

TEST(BookDatabaseTest, ExtractAuthorsFlat) {
    BookDatabase<> db;
    db.EmplaceBack("1984", "George Orwell", 1949, Genre::SciFi, 4.5, 100);
    db.EmplaceBack("Animal Farm", "George Orwell", 1945, Genre::Fiction, 4.4, 90);
    db.EmplaceBack("Brave New World", "Aldous Huxley", 1932, Genre::SciFi, 4.3, 80);

    auto authors = extractAuthorsFlat(db);
    EXPECT_EQ(authors.size(), 2);
    EXPECT_TRUE(authors.contains("George Orwell"));
    EXPECT_TRUE(authors.contains("Aldous Huxley"));
}
