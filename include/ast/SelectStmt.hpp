/**
* @file SelectStmt.hpp
 * @brief Implementation of the SELECT statement execution logic.
 */
#pragma once
#include "Statement.hpp"
#include "WhereClause.hpp"
#include <vector>
#include <string>
#include <memory>

/**
 * @class SelectStmt
 * @brief Class that represents and manages the execution of a SQL SELECT statement.
 */
class SelectStmt : public Statement {
public:
    /**
     * @brief Supported aggregate functions.
     */
    enum class Aggregate { NONE, COUNT, SUM, AVG, MIN, MAX };

    /**
     * @brief Representation of an ORDER BY clause item.
     */
    struct OrderByItem {
        std::string column;
        bool isAscending = true;
    };

    /** @brief Maximum number of rows to return (-1 for no limit). */
    int limit;
    /** @brief Number of rows to skip before starting to return results. */
    int offset;
private:
    std::string tableName;
    std::vector<std::string> columns;
    bool isDistinct;
    Aggregate aggType;
    std::string aggColumn = "";
    std::vector<OrderByItem> orderByList;
    std::unique_ptr<WhereClause> where;

public:
    /**
     * @brief Constructs a new SelectStmt object.
     * @param table The name of the table to select from.
     * @param cols Vector of column names to select.
     * @param distinct Whether to apply the DISTINCT keyword for only unique columns.
     */
    SelectStmt(std::string table, std::vector<std::string> cols, bool distinct = false) {
        this->tableName = std::move(table);
        this->columns = std::move(cols);
        this->isDistinct = distinct;
        this->limit = -1;
        this->offset = 0;
        this->aggType = Aggregate::NONE;
        this->aggColumn = "";
        this->orderByList = {};
        this->where = nullptr;
    }

    /** * @brief getters */
    std::string getTableName() const { return tableName; }
    const std::vector<std::string>& getColumns() const { return columns; }
    bool getDistinct() const { return isDistinct; }
    Aggregate getAggregate() const { return aggType; }
    std::string getAggColumn() const { return aggColumn; }
    std::string getOrderBy() const { return orderByList.empty() ? "" : orderByList[0].column; }
    bool getAscending() const { return orderByList.empty() ? true : orderByList[0].isAscending; }
    const std::vector<OrderByItem>& getOrderByList() const { return orderByList; }

    // setters
    /**
     * @brief Configures an aggregate function for the query.
     * @param type The type of aggregation.
     * @param col The column to aggregate.
     */
    void setAggregate(Aggregate type, std::string col) { aggType = type; aggColumn = std::move(col); }
    void setOrder(std::string col, bool asc) {
        orderByList.clear();
        if (!col.empty()) {
            orderByList.push_back({std::move(col), asc});
        }
    }
    void setOrderList(std::vector<OrderByItem> list) { orderByList = std::move(list); }
    void setWhere(std::unique_ptr<WhereClause> w) { where = std::move(w); }

    /**
     * @brief Executes the SELECT statement.
     * * The execution follows:
     * 1. Column mapping
     * 2. Row filtering (WHERE clause)
     * 3. Sorting (ORDER BY)
     * 4. Deduplication (DISTINCT)
     * 5. Aggregation (COUNT, SUM, etc.)
     * 6. Pagination (OFFSET/LIMIT)
     * 7. GUI Result preparation
     */
    void execute() override;
};