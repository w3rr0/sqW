#ifndef RESULTSET_HPP
#define RESULTSET_HPP

#include <string>
#include <vector>

#include "storage/Cell.hpp"


class ResultSet {
public:
    explicit ResultSet(std::vector<std::string> headers);
    explicit ResultSet(std::vector<std::string> headers, std::vector<std::vector<Cell>> initialRows);

    void addRow(std::vector<Cell> row);

    [[nodiscard]] const std::vector<std::string>& getHeaders() const;
    [[nodiscard]] const std::vector<std::vector<Cell>>& getRows() const;

private:
    std::vector<std::string> headers;
    std::vector<std::vector<Cell>> rows;
};


#endif //RESULTSET_HPP
