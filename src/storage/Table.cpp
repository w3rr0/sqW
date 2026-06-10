/**
 * @file Table.cpp
 * @brief Implementation of Table class.
 */

#include "storage/Table.hpp"
#include <algorithm>

Table::Table(std::string tableName)
    : name(std::move(tableName)) {}

void Table::addColumn(const std::string& colName, Cell::Type colType) {
    for (const auto& col : columns) {
        if (col.getName() == colName) {
            throw std::invalid_argument("Error: Column '" + colName + "' already exists in table '" + name + "'.");
        }
    }

    Column newCol(colName, colType);

    size_t rowCount = getRowCount();
    for (size_t i = 0; i < rowCount; ++i) {
        // new Cells with default NULL TYPE -> for ALTER TABLE ADD COLUMN
        newCol.add(Cell());
    }

    columns.push_back(std::move(newCol));
}

void Table::dropColumn(const std::string& colName) {
    // fidning name of the column to drop in the table
    auto it = std::find_if(columns.begin(), columns.end(),
        [&colName](const Column& col) { return col.getName() == colName; });

    if (it == columns.end()) {
        std::cout << "Error. ALTER TABLE: Column " << colName << " does not exist in table '" << name << "'" << std::endl;
    }

    // removing certain column if exists
    columns.erase(it);
}

void Table::insertRow(const std::vector<Cell>& row) {
    if (row.size() != columns.size()) {
        throw std::invalid_argument(
            "Error INSERT: Table '" + name + "' has " + std::to_string(columns.size()) +
            " columns, but tried to insert " + std::to_string(row.size()) + " values."
        );
    }

    // Data types validation
    for (size_t i = 0; i < columns.size(); ++i) {
        const Cell& cell = row[i];
        if (cell.getType() != Cell::Type::NULL_TYPE && cell.getType() != columns[i].getType()) {
            throw std::invalid_argument(
                "Type error in column '" + columns[i].getName() +
                "': Expecting type " + std::to_string(static_cast<int>(columns[i].getType())) +
                ", but got " + std::to_string(static_cast<int>(cell.getType()))
            );
        }
    }

    // Inserting data
    for (size_t i = 0; i < columns.size(); ++i) {
        columns[i].add(row[i]);
    }

    deletedMask.push_back(false);
}

std::vector<Cell> Table::getRow(size_t rowIndex) const {
    if (rowIndex >= getRowCount()) {
        throw std::out_of_range("Error: Row with index " + std::to_string(rowIndex) + " does not exist.");
    }

    std::vector<Cell> resultRow;
    resultRow.reserve(columns.size());

    for (const auto& col : columns) {
        resultRow.push_back(col.get(rowIndex));
    }

    return resultRow;
}

Column& Table::getColumn(const std::string& colName) {
    // Wrapper for const getColumn version
    return const_cast<Column&>(static_cast<const Table*>(this)->getColumn(colName));
}

const Column& Table::getColumn(const std::string& colName) const {
    auto it = std::find_if(columns.begin(), columns.end(),
        [&colName](const Column& col) { return col.getName() == colName; }
    );

    if (it != columns.end()) {
        return *it;
    }

    throw std::invalid_argument("Error: Column '" + colName + "' don't exist in table '" + name + "'.");
}

void Table::updateCell(size_t rowIndex, const std::string& colName, const Cell& newValue) {
    if (rowIndex >= getRowCount()) {
        throw std::out_of_range("Error UPDATE: row with index" + std::to_string(rowIndex) + " does not exist in table '" + name + "'.");
    }

    Column& col = getColumn(colName);

    col.set(rowIndex, newValue);
}

void Table::deleteRow(size_t rowIndex) {
    if (rowIndex >= getRowCount()) {
        throw std::out_of_range("Error DELETE: Row index " + std::to_string(rowIndex) + " out of range.");
    }

    deletedMask[rowIndex] = true;
}

bool Table::isDeleted(size_t rowIndex) const {
    if (rowIndex >= deletedMask.size()) { // Should not happen
        return false;
    }
    return deletedMask[rowIndex];
}

size_t Table::getRowCount() const {
    if (columns.empty()) {
        return 0;
    }
    return columns[0].size();
}

void Table::vacuum() {
    for (auto& col : columns) {
        col.vacuum(deletedMask);
    }

    size_t aliveCount = 0;
    for (bool isDeleted : deletedMask) {
        if (!isDeleted) aliveCount++;
    }

    deletedMask = std::vector<bool>(aliveCount, false);
}
