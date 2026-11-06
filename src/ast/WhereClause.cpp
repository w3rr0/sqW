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

// ComparisonCondition
ComparisonCondition::ComparisonCondition(std::string col, std::string oper, std::string val)
    : column(std::move(col)), op(std::move(oper)), value(std::move(val)) {
    if (op == "=") opType = OpType::EQ;
    else if (op == ">") opType = OpType::GT;
    else if (op == "<") opType = OpType::LT;
    else if (op == ">=") opType = OpType::GTE;
    else if (op == "<=") opType = OpType::LTE;
    else if (op == "!=") opType = OpType::NEQ;
    else if (op == "IS") opType = OpType::IS_NULL;
    else if (op == "IS NOT") opType = OpType::IS_NOT_NULL;
    else if (op == "LIKE") opType = OpType::LIKE;
    else if (op == "NOT LIKE") opType = OpType::NOT_LIKE;
    else opType = OpType::UNKNOWN;

    if (opType == OpType::LIKE || opType == OpType::NOT_LIKE) {
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
            regexObj = std::regex(regexPattern, std::regex_constants::icase);
            regexValid = true;
        } catch (...) {
            regexValid = false;
        }
    }
}

void ComparisonCondition::prepare(const Table& table) const {
    cachedTable = &table;
    cachedColIdx = getColIdx(table, column);
    if (opType != OpType::IS_NULL && opType != OpType::IS_NOT_NULL) {
        Cell::Type type = table.getColumns()[cachedColIdx].getType();
        cachedLiteral = createLiteralCell(value, type);
    }
    prepared = true;
}

bool ComparisonCondition::evaluateCell(const Cell& valInTable) const {
    switch (opType) {
        case OpType::EQ:          return valInTable == cachedLiteral;
        case OpType::GT:          return valInTable >  cachedLiteral;
        case OpType::LT:          return valInTable <  cachedLiteral;
        case OpType::GTE:         return valInTable >= cachedLiteral;
        case OpType::LTE:         return valInTable <= cachedLiteral;
        case OpType::NEQ:         return valInTable != cachedLiteral;
        case OpType::IS_NULL:     return valInTable.getType() == Cell::Type::NULL_TYPE;
        case OpType::IS_NOT_NULL: return valInTable.getType() != Cell::Type::NULL_TYPE;
        case OpType::LIKE: {
            if (valInTable.getType() != Cell::TEXT || !regexValid) return false;
            return std::regex_match(valInTable.as<std::string>(), regexObj);
        }
        case OpType::NOT_LIKE: {
            if (valInTable.getType() != Cell::TEXT || !regexValid) return false;
            return !std::regex_match(valInTable.as<std::string>(), regexObj);
        }
        default: return false;
    }
}

bool ComparisonCondition::evaluate(const Table& table, const std::vector<Cell>& row) const {
    if (!prepared || cachedTable != &table) {
        prepare(table);
    }
    const Cell& valInTable = row[cachedColIdx];
    return evaluateCell(valInTable);
}

bool ComparisonCondition::evaluate(const Table& table, size_t rowIndex) const {
    if (!prepared || cachedTable != &table) {
        prepare(table);
    }
    if (opType == OpType::IS_NULL) {
        return table.getColumns()[cachedColIdx].isNull(rowIndex);
    }
    if (opType == OpType::IS_NOT_NULL) {
        return !table.getColumns()[cachedColIdx].isNull(rowIndex);
    }
    if (table.getColumns()[cachedColIdx].isNull(rowIndex)) {
        if (opType == OpType::EQ) return cachedLiteral.isNull();
        if (opType == OpType::NEQ) return !cachedLiteral.isNull();
        return false;
    }
    Cell valInTable = table.getColumns()[cachedColIdx].get(rowIndex);
    return evaluateCell(valInTable);
}

void ComparisonCondition::print() const {
    std::cout << column << " " << op << " " << value;
}

// LogicalCondition
LogicalCondition::LogicalCondition(std::unique_ptr<WhereClause> l, std::string oper, std::unique_ptr<WhereClause> r)
    : left(std::move(l)), op(std::move(oper)), right(std::move(r)) {
    logicalOp = (op == "OR") ? LogicalOp::OR : LogicalOp::AND;
}

void LogicalCondition::prepare(const Table& table) const {
    if (left) left->prepare(table);
    if (right) right->prepare(table);
}

bool LogicalCondition::evaluate(const Table& table, const std::vector<Cell>& row) const {
    if (logicalOp == LogicalOp::AND) {
        return left && left->evaluate(table, row) && right && right->evaluate(table, row);
    }
    if (logicalOp == LogicalOp::OR) {
        return (left && left->evaluate(table, row)) || (right && right->evaluate(table, row));
    }
    return false;
}

bool LogicalCondition::evaluate(const Table& table, size_t rowIndex) const {
    if (logicalOp == LogicalOp::AND) {
        return left && left->evaluate(table, rowIndex) && right && right->evaluate(table, rowIndex);
    }
    if (logicalOp == LogicalOp::OR) {
        return (left && left->evaluate(table, rowIndex)) || (right && right->evaluate(table, rowIndex));
    }
    return false;
}

void LogicalCondition::print() const {
    std::cout << "(";
    if (left) left->print();
    std::cout << " " << op << " ";
    if (right) right->print();
    std::cout << ")";
}

// BetweenCondition
void BetweenCondition::prepare(const Table& table) const {
    cachedTable = &table;
    cachedColIdx = getColIdx(table, column);
    Cell::Type type = table.getColumns()[cachedColIdx].getType();
    cachedMin = createLiteralCell(valMin, type);
    cachedMax = createLiteralCell(valMax, type);
    prepared = true;
}

bool BetweenCondition::evaluate(const Table& table, const std::vector<Cell>& row) const {
    if (!prepared || cachedTable != &table) {
        prepare(table);
    }
    const Cell& v = row[cachedColIdx];
    return v >= cachedMin && v <= cachedMax;
}

bool BetweenCondition::evaluate(const Table& table, size_t rowIndex) const {
    if (!prepared || cachedTable != &table) {
        prepare(table);
    }
    if (table.getColumns()[cachedColIdx].isNull(rowIndex)) {
        return false;
    }
    Cell v = table.getColumns()[cachedColIdx].get(rowIndex);
    return v >= cachedMin && v <= cachedMax;
}

void BetweenCondition::print() const {
    std::cout << column << " BETWEEN " << valMin << " AND " << valMax;
}

// InCondition
void InCondition::prepare(const Table& table) const {
    cachedTable = &table;
    cachedColIdx = getColIdx(table, column);
    Cell::Type type = table.getColumns()[cachedColIdx].getType();
    cachedValues.clear();
    cachedValues.reserve(values.size());
    for (const auto& val : values) {
        cachedValues.push_back(createLiteralCell(val, type));
    }
    prepared = true;
}

bool InCondition::evaluate(const Table& table, const std::vector<Cell>& row) const {
    if (!prepared || cachedTable != &table) {
        prepare(table);
    }
    const Cell& v = row[cachedColIdx];
    for (const auto& lit : cachedValues) {
        if (v == lit) return true;
    }
    return false;
}

bool InCondition::evaluate(const Table& table, size_t rowIndex) const {
    if (!prepared || cachedTable != &table) {
        prepare(table);
    }
    if (table.getColumns()[cachedColIdx].isNull(rowIndex)) {
        return false;
    }
    Cell v = table.getColumns()[cachedColIdx].get(rowIndex);
    for (const auto& lit : cachedValues) {
        if (v == lit) return true;
    }
    return false;
}

void InCondition::print() const {
    std::cout << column << " IN (";
    for (size_t i = 0; i < values.size(); ++i) {
        std::cout << values[i] << (i == values.size() - 1 ? "" : ", ");
    }
    std::cout << ")";
}
