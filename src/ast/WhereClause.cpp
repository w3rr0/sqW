#include "ast/WhereClause.hpp"
#include <iostream>
#include <regex>
#include <stdexcept>
#include "storage/Cell.hpp"


// helper method for removing '' and ""
static std::string clearQuotes(std::string s) {
    if (s.size() >= 2 && ((s.front() == '\'' && s.back() == '\'') || (s.front() == '\"' && s.back() == '\"'))) {
        return s.substr(1, s.size() - 2);
    }
    return s;
}

// helper method for finding an index of certain column from the table
static size_t getColIdx(const Table& table, const std::string& colName) {
    const auto& cols = table.getColumns();
    for (size_t i = 0; i < cols.size(); ++i) {
        if (cols[i].getName() == colName) return i;
    }
    throw std::invalid_argument("Column not found: " + colName);
}

// helper method that creates a Cell object with the correct type
Cell createLiteralCell(const std::string& val, Cell::Type type) {
    std::string clean = clearQuotes(val);

    if (clean == "NULL" || clean == "null") {
        return Cell();
    }
    if (type == Cell::INT) return Cell(std::stoi(clean));
    if (type == Cell::DOUBLE) return Cell(std::stod(clean));
    if (type == Cell::BOOL) return Cell(clean == "true" || clean == "1");
    return Cell(clean); // TEXT
}

// ComparisonCondition -> OPERATORS and LIKE
bool ComparisonCondition::evaluate(const Table& table, const std::vector<Cell>& row) const {
    size_t idx = getColIdx(table, column);
    const Cell& valInTable = row[idx];
    Cell literal = createLiteralCell(value, table.getColumns()[idx].getType());

    if (op == "=")  return valInTable == literal;
    if (op == ">")  return valInTable > literal;
    if (op == "<")  return valInTable < literal;
    if (op == ">=") return valInTable >= literal;
    if (op == "<=") return valInTable <= literal;
    if (op == "!=") return valInTable != literal;

    if (op == "IS") {
        return valInTable.getType() == Cell::Type::NULL_TYPE;
    }
    if (op == "IS NOT") {
        return valInTable.getType() != Cell::Type::NULL_TYPE;
    }

    if (op == "LIKE" || op == "NOT LIKE") {
        // LIKE requires converting from Cell to string
        std::string v;
        if (valInTable.getType() == Cell::TEXT) v = valInTable.as<std::string>();
        else return false; // works only for texts

        std::string pattern = clearQuotes(value);
        std::string regexPattern = pattern;

        size_t pos = 0;
        // changing '%' in SQL for '.*' in REGEX
        while ((pos = regexPattern.find('%', pos)) != std::string::npos) {
            regexPattern.replace(pos, 1, ".*");
            pos += 2;
        }
        pos = 0;
        // changing '_' in SQL for '.' in REGEX
        while ((pos = regexPattern.find('_', pos)) != std::string::npos) {
            regexPattern.replace(pos, 1, ".");
            pos += 1;
        }

        try {
            std::regex re(regexPattern, std::regex_constants::icase); // icase = case insensitive

            bool isMatch = std::regex_match(v, re);
            // if NOT LIKE -> !isMatch
            return (op == "NOT LIKE") ? !isMatch : isMatch;
        } catch (...) {
            return false;
        }
    }
    return false;
}

void ComparisonCondition::print() const { std::cout << column << " " << op << " " << value; }

// LogicalCondition -> AND / OR
bool LogicalCondition::evaluate(const Table& table, const std::vector<Cell>& row) const {
    if (op == "AND") return left->evaluate(table, row) && right->evaluate(table, row);
    if (op == "OR")  return left->evaluate(table, row) || right->evaluate(table, row);
    return false;
}

void LogicalCondition::print() const {
    std::cout << "("; left->print(); std::cout << " " << op << " "; right->print(); std::cout << ")";
}

// BetweenCondition -> BETWEEN
bool BetweenCondition::evaluate(const Table& table, const std::vector<Cell>& row) const {
    size_t idx = getColIdx(table, column);
    const Cell& v = row[idx];
    Cell minL = createLiteralCell(valMin, table.getColumns()[idx].getType());
    Cell maxL = createLiteralCell(valMax, table.getColumns()[idx].getType());

    return v >= minL && v <= maxL;
}

void BetweenCondition::print() const { std::cout << column << " BETWEEN " << valMin << " AND " << valMax; }

// InCondition -> IN
bool InCondition::evaluate(const Table& table, const std::vector<Cell>& row) const {
    size_t idx = getColIdx(table, column);
    const Cell& v = row[idx];
    Cell::Type type = table.getColumns()[idx].getType();

    for (const auto& val : values) {
        if (v == createLiteralCell(val, type)) return true;
    }
    return false;
}

void InCondition::print() const {
    std::cout << column << " IN (";
    for(size_t i=0; i<values.size(); ++i) std::cout << values[i] << (i==values.size()-1 ? "" : ", ");
    std::cout << ")";
}