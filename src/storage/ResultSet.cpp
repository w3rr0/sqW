#include "../../include/storage/ResultSet.hpp"

ResultSet::ResultSet(std::vector<std::string> headers, std::vector<std::vector<Cell> > rows)
    : headers(std::move(headers)), rows(std::move(rows)) {}

void ResultSet::addRow(std::vector<Cell> row) {
    if (row.size() != headers.size()) {
        throw std::invalid_argument("Row size does not match header size");
    }
    rows.push_back(std::move(row));
}

std::vector<std::string> ResultSet::getHeaders() {
    return headers;
}

std::vector<std::vector<Cell>> ResultSet::getRows() {
    return rows;
}