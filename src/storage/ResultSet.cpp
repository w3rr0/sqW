#include <utility>
#include <stdexcept>

#include "storage/ResultSet.hpp"

ResultSet::ResultSet(std::vector<std::string> headers)
    : headers(std::move(headers)) {}

ResultSet::ResultSet(std::vector<std::string> headers, std::vector<std::vector<Cell>> initialRows)
    : headers(std::move(headers)) {
    rows.reserve(initialRows.size());
    for (auto& row : initialRows) {
        addRow(std::move(row));
    }
}

void ResultSet::addRow(std::vector<Cell> row) {
    if (row.size() != headers.size()) {
        throw std::invalid_argument("Row size does not match header size");
    }
    rows.push_back(std::move(row));
}

const std::vector<std::string>& ResultSet::getHeaders() const {
    return headers;
}

const std::vector<std::vector<Cell>>& ResultSet::getRows() const {
    return rows;
}