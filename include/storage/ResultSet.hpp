#ifndef RESULTSET_HPP
#define RESULTSET_HPP

#include "storage/Cell.hpp"


class ResultSet {
public:
    ResultSet() = default;
    explicit ResultSet(std::vector<std::string> headers, std::vector<std::vector<Cell>> rows);

    void addRow(std::vector<Cell> row);

    std::vector<std::string> getHeaders();
    std::vector<std::vector<Cell>> getRows();

private:
    std::vector<std::string> headers;
    std::vector<std::vector<Cell>> rows;
};


#endif //RESULTSET_HPP
