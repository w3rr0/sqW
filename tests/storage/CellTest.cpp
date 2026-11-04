#include <gtest/gtest.h>
#include "storage/Cell.hpp"
#include <set>
#include <string>
#include <stdexcept>
#include <unordered_map>
#include <unordered_set>

TEST(CellTest, DefaultConstructorIsNULL) {
    Cell cell;
    EXPECT_EQ(cell.getType(), Cell::NULL_TYPE);
}

TEST(CellTest, IntConstructor) {
    Cell cell(42);
    EXPECT_EQ(cell.getType(), Cell::INT);
    EXPECT_EQ(cell.as<int>(), 42);
}

TEST(CellTest, DoubleConstructor) {
    Cell cell(3.14);
    EXPECT_EQ(cell.getType(), Cell::DOUBLE);
    EXPECT_DOUBLE_EQ(cell.as<double>(), 3.14);
}

TEST(CellTest, StringConstructor) {
    Cell cell(std::string("hello"));
    EXPECT_EQ(cell.getType(), Cell::TEXT);
    EXPECT_EQ(cell.as<std::string>(), "hello");
}

TEST(CellTest, BoolConstructor) {
    Cell cell(true);
    EXPECT_EQ(cell.getType(), Cell::BOOL);
    EXPECT_TRUE(cell.as<bool>());
}

TEST(CellTest, EqualityOperator) {
    Cell c1(10);
    Cell c2(10);
    Cell c3(20);
    Cell c4("test");

    EXPECT_TRUE(c1 == c2);
    EXPECT_FALSE(c1 == c3);
    EXPECT_FALSE(c1 == c4);
}

TEST(CellTest, LessThanOperator) {
    Cell c1(10);
    Cell c2(20);
    
    EXPECT_TRUE(c1 < c2);
    EXPECT_FALSE(c2 < c1);
}

TEST(CellTest, InvalidTypeAccessThrows) {
    Cell cell(42);
    EXPECT_THROW(cell.as<std::string>(), std::bad_variant_access);
}

TEST(CellTest, NullComparisonThrows) {
    Cell c1(10);
    Cell cNull;
    
    EXPECT_THROW(c1.checkNullComparison(cNull), std::invalid_argument);
}

TEST(CellTest, NullEqualsNull) {
    EXPECT_TRUE(Cell() == Cell());
}

TEST(CellTest, NullIsNotEqualToValue) {
    EXPECT_FALSE(Cell() == Cell(0));
    EXPECT_FALSE(Cell(0) == Cell());
}

TEST(CellTest, NullIsLessThanAnyValue) {
    EXPECT_TRUE(Cell() < Cell(0));
    EXPECT_FALSE(Cell(0) < Cell());
    EXPECT_FALSE(Cell() < Cell());
}

TEST(CellTest, IntAndDoubleAreComparable) {
    EXPECT_TRUE(Cell(1) == Cell(1.0));
    EXPECT_FALSE(Cell(1) < Cell(1.0));
    EXPECT_FALSE(Cell(1.0) < Cell(1));

    EXPECT_TRUE(Cell(1) < Cell(2.0));
    EXPECT_TRUE(Cell(2.0) < Cell(3));
}

TEST(CellTest, DifferentTypesAreNotEqual) {
    EXPECT_FALSE(Cell(1) == Cell(std::string("1")));
    EXPECT_TRUE(Cell(1) != Cell(std::string("1")));
}

TEST(CellTest, OrderingDifferentTypesThrows) {
    EXPECT_THROW({
        bool result = Cell(1) < Cell(std::string("1"));
        (void)result;
    }, std::invalid_argument);
}

TEST(CellTest, DerivedOperatorsFollowTotalOrder) {
    EXPECT_TRUE(Cell() <= Cell());
    EXPECT_TRUE(Cell() <= Cell(1));
    EXPECT_FALSE(Cell(1) <= Cell());

    EXPECT_TRUE(Cell(1) >= Cell());
    EXPECT_FALSE(Cell() >= Cell(1));

    EXPECT_FALSE(Cell() != Cell());
    EXPECT_TRUE(Cell() != Cell(1));
    EXPECT_FALSE(Cell(1) != Cell(1.0));
}

TEST(CellTest, SetTreatsNullsAsOneValue) {
    std::set<Cell> values{Cell(), Cell(), Cell(0), Cell(1)};

    EXPECT_EQ(values.size(), 3u);
}


TEST(CellTest, HashOfNullsMatches) {
    const std::hash<Cell> hashCell;

    EXPECT_EQ(hashCell(Cell()), hashCell(Cell()));
}

TEST(CellTest, HashOfEqualValuesMatches) {
    const std::hash<Cell> hashCell;

    EXPECT_EQ(hashCell(Cell(42)), hashCell(Cell(42)));
    EXPECT_EQ(hashCell(Cell(3.14)), hashCell(Cell(3.14)));
    EXPECT_EQ(hashCell(Cell(std::string("abc"))), hashCell(Cell(std::string("abc"))));
    EXPECT_EQ(hashCell(Cell(true)), hashCell(Cell(true)));
}

TEST(CellTest, HashTreatsIntAndDoubleEqually) {
    const std::hash<Cell> hashCell;

    EXPECT_EQ(hashCell(Cell(0)), hashCell(Cell(0.0)));
    EXPECT_EQ(hashCell(Cell(1)), hashCell(Cell(1.0)));
    EXPECT_EQ(hashCell(Cell(-7)), hashCell(Cell(-7.0)));
}

TEST(CellTest, HashMatchesUnderlyingValueHash) {
    const std::hash<Cell> hashCell;

    EXPECT_EQ(hashCell(Cell(2.5)), std::hash<double>{}(2.5));
    EXPECT_EQ(hashCell(Cell(std::string("abc"))), std::hash<std::string>{}(std::string("abc")));
    EXPECT_EQ(hashCell(Cell(true)), std::hash<bool>{}(true));
    EXPECT_EQ(hashCell(Cell(false)), std::hash<bool>{}(false));
}

TEST(CellTest, UnorderedSetGroupsNullsAndNumericEquivalents) {
    std::unordered_set<Cell> values{
        Cell(), Cell(),
        Cell(1), Cell(1.0),
        Cell(2)
    };

    EXPECT_EQ(values.size(), 3u);
    EXPECT_EQ(values.count(Cell()), 1u);
    EXPECT_EQ(values.count(Cell(1.0)), 1u);
}

TEST(CellTest, UnorderedMapKeyedByCell) {
    std::unordered_map<Cell, std::string> map;

    map[Cell()] = "null";
    map[Cell(1)] = "one";
    map[Cell(std::string("key"))] = "text";
    map[Cell(true)] = "bool";

    EXPECT_EQ(map.at(Cell()), "null");
    EXPECT_EQ(map.at(Cell(1.0)), "one");
    EXPECT_EQ(map.at(Cell(std::string("key"))), "text");
    EXPECT_EQ(map.at(Cell(true)), "bool");
    EXPECT_EQ(map.size(), 4u);
}
