/**
* @file WhereClause.hpp
 * @brief Header definition for the WhereClause base class and its derived condition types.
 */
#pragma once
#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <regex>
#include <storage/Cell.hpp>
#include <storage/Table.hpp>

/**
 * @class WhereClause
 * @brief Abstract base class representing any condition in a WHERE clause.
 */
class WhereClause {
public:
    /**
     * @brief Virtual destructor for proper cleanup of derived condition objects.
     */
    virtual ~WhereClause() = default;

    /**
     * @brief Prepares and caches column indices / literal cells for a given table.
     */
    virtual void prepare(const Table& table) const {}

    /**
     * @brief Evaluates the condition for a specific row.
     * @param table Reference to the table being processed.
     * @param row The row data as a vector of Cells.
     * @return true if the row meets the condition, false otherwise.
     */
    virtual bool evaluate(const Table& table, const std::vector<Cell>& row) const = 0;

    /**
     * @brief Evaluates the condition for a row by index in the table, avoiding row allocation.
     * @param table Reference to the table being processed.
     * @param rowIndex Row index in the table.
     * @return true if the row meets the condition, false otherwise.
     */
    virtual bool evaluate(const Table& table, size_t rowIndex) const {
        return evaluate(table, table.getRow(rowIndex));
    }

    /** @brief Prints the condition structure.*/
    virtual void print() const = 0;
};

/**
 * @class ComparisonCondition
 * @brief Handles simple comparisons (=, !=, <, >, >=, <=, LIKE).
 */
class ComparisonCondition : public WhereClause {
public:
    enum class OpType {
        EQ, GT, LT, GTE, LTE, NEQ, IS_NULL, IS_NOT_NULL, LIKE, NOT_LIKE, UNKNOWN
    };

private:
    /** The column name to check. */
    std::string column;
    /** The operator string (e.g., ">="). */
    std::string op;
    /** The literal value to compare against. */
    std::string value;

    OpType opType = OpType::UNKNOWN;
    std::regex regexObj;
    bool regexValid = false;

    mutable const Table* cachedTable = nullptr;
    mutable size_t cachedColIdx = size_t(-1);
    mutable Cell cachedLiteral;
    mutable bool prepared = false;

    bool evaluateCell(const Cell& valInTable) const;

public:
    /**
     * @brief Constructs a new ComparisonCondition object.
     * @param col The name of the target column.
     * @param oper The operator string.
     * @param val The value to compare with.
     */
    ComparisonCondition(std::string col, std::string oper, std::string val);

    void prepare(const Table& table) const override;
    bool evaluate(const Table& table, const std::vector<Cell>& row) const override;
    bool evaluate(const Table& table, size_t rowIndex) const override;
    void print() const override;

    const std::string& getColumn() const { return column; }
    const std::string& getOp() const { return op; }
    const std::string& getValue() const { return value; }
};

/**
 * @class LogicalCondition
 * @brief Handles logical operations (AND, OR) between two other conditions.
 */
class LogicalCondition : public WhereClause {
public:
    enum class LogicalOp { AND, OR };

private:
    /** Left side condition. */
    std::unique_ptr<WhereClause> left;
    /** Logical operator (AND / OR). */
    std::string op;
    /** Right side condition. */
    std::unique_ptr<WhereClause> right;

    LogicalOp logicalOp = LogicalOp::AND;

public:
    /**
     * @brief Constructs a new LogicalCondition object.
     * @param l Unique pointer to the left condition.
     * @param oper Logical operator string.
     * @param r Unique pointer to the right condition.
     */
    LogicalCondition(std::unique_ptr<WhereClause> l, std::string oper, std::unique_ptr<WhereClause> r);

    void prepare(const Table& table) const override;
    bool evaluate(const Table& table, const std::vector<Cell>& row) const override;
    bool evaluate(const Table& table, size_t rowIndex) const override;
    void print() const override;

    const std::unique_ptr<WhereClause>& getLeft() const { return left; }
    const std::string& getOp() const { return op; }
    const std::unique_ptr<WhereClause>& getRight() const { return right; }
};

/**
 * @class BetweenCondition
 * @brief Handles the BETWEEN condition (col BETWEEN val1 AND val2).
 */
class BetweenCondition : public WhereClause {
private:
    /** The column name to check. */
    std::string column;
    /** The minimum boundary value. */
    std::string valMin;
    /** The maximum boundary value. */
    std::string valMax;

    mutable const Table* cachedTable = nullptr;
    mutable size_t cachedColIdx = size_t(-1);
    mutable Cell cachedMin;
    mutable Cell cachedMax;
    mutable bool prepared = false;

public:
    /**
     * @brief Constructs a new BetweenCondition object.
     * @param col The name of the target column.
     * @param min The minimum value of the range.
     * @param max The maximum value of the range.
     */
    BetweenCondition(std::string col, std::string min, std::string max)
        : column(std::move(col)), valMin(std::move(min)), valMax(std::move(max)) {}

    void prepare(const Table& table) const override;
    bool evaluate(const Table& table, const std::vector<Cell>& row) const override;
    bool evaluate(const Table& table, size_t rowIndex) const override;
    void print() const override;

    const std::string& getColumn() const { return column; }
    const std::string& getMin() const { return valMin; }
    const std::string& getMax() const { return valMax; }
};

/**
 * @class InCondition
 * @brief Handles the IN condition (col IN (val1, val2, ...)).
 */
class InCondition : public WhereClause {
private:
    /** The column name to check. */
    std::string column;
    /** List of allowed values. */
    std::vector<std::string> values;

    mutable const Table* cachedTable = nullptr;
    mutable size_t cachedColIdx = size_t(-1);
    mutable std::vector<Cell> cachedValues;
    mutable bool prepared = false;

public:
    /**
     * @brief Constructs a new InCondition object.
     * @param col The name of the target column.
     * @param vals A vector of string values to match against.
     */
    InCondition(std::string col, std::vector<std::string> vals)
        : column(std::move(col)), values(std::move(vals)) {}

    void prepare(const Table& table) const override;
    bool evaluate(const Table& table, const std::vector<Cell>& row) const override;
    bool evaluate(const Table& table, size_t rowIndex) const override;
    void print() const override;

    const std::string& getColumn() const { return column; }
    const std::vector<std::string>& getValues() const { return values; }
};
