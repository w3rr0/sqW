/**
 * @file Cell.hpp
 * @brief Definition of the Cell class representing a single data element in the database.
 */

#ifndef CELL_HPP
#define CELL_HPP

#include <functional>
#include <variant>
#include <string>
#include <iostream>

/**
 * @class Cell
 * @brief Represents a single piece of data (a cell) within a database table.
 * This class abstracts the underlying data type by using std::variant,
 * allowing for unified handling of different SQL data types as well as NULL values.
 * It also provides SQL-compliant comparison operators.
 */
class Cell {
public:

    /**
     * @brief The underlying variant type used to store the cell's data.
     * std::monostate is used to represent a SQL NULL value.
     */
    using ValueType = std::variant<std::monostate, int, double, std::string, bool>;

    /**
     * @brief Enum representing the supported SQL data types.
     * The order of these types matches the order in ValueType.
     */
    enum Type {
        NULL_TYPE, INT, DOUBLE, TEXT, BOOL
    };

    // Constructors

    /**
     * @brief Default constructor. Initializes the cell with a NULL value.
     */
    Cell() : value(std::monostate{}) {}

    /**
     * @brief Constructs a cell containing an integer.
     * @param v The integer value.
     */
    explicit Cell(int v) : value(v) {}

    /**
     * @brief Constructs a cell containing a double.
     * @param v The double value.
     */
    explicit Cell(double v) : value(v) {}

    /**
     * @brief Constructs a cell containing a text string.
     * Utilizes move semantics to prevent unnecessary copying.
     * @param v The string value.
     */
    explicit Cell(std::string v) : value(std::move(v)) {}

    /**
     * @brief Constructs a cell containing a boolean value.
     * @param v The boolean value.
     */
    explicit Cell(bool v) : value(v) {}

    // Getters

    /**
     * @brief Retrieves the current data type of the cell.
     * @return The Type enum representing the stored data.
     */
    Type getType() const;

    /**
     * @brief Extracts the underlying value as the specified type.
     * @tparam T The expected C++ type (e.g., int, std::string).
     * @return A constant reference to the stored value.
     * @throws std::bad_variant_access If the requested type does not match the stored type.
     */
    template<typename T>
    const T& as() const { return std::get<T>(value); }

    // Comparisons
    bool operator==(const Cell& other) const;
    bool operator<(const Cell& other) const;
    bool operator!=(const Cell& other) const { return !(*this == other); }
    bool operator>(const Cell& other) const  { return other < *this; }
    bool operator<=(const Cell& other) const { return !(*this > other); }
    bool operator>=(const Cell& other) const { return !(*this < other); }

    /**
     * Checks if the cell is NULL.
     */
    bool isNull() const;

    /**
     * @brief Helper method to prevent invalid relational comparisons involving NULL.
     * @param other The other cell involved in the comparison.
     * @throws std::invalid_argument If either cell is NULL.
     */
    void checkNullComparison(const Cell& other) const;

    // Display

    /**
     * @brief Overloads the stream insertion operator for easy printing of cell contents.
     * @param os The output stream.
     * @param cell The cell to print.
     * @return The modified output stream.
     */
    friend std::ostream& operator<<(std::ostream& os, const Cell& cell);

private:

    /**
     * @brief The actual container holding the data.
     */
    ValueType value;
};

template<>
struct std::hash<Cell> {
    std::size_t operator()(const Cell& cell) const noexcept;
};

#endif
