#include "ast/InsertStmt.hpp"
#include <iostream>
#include "storage/Database.hpp"
#include "storage/Cell.hpp"
#include <stdexcept>

extern Database db;
extern std::vector<std::string> gui_log;

// helper method for removing '' and "" from new added values
static std::string cleanInput(std::string s) {
    if (s.size() >= 2 && ((s.front() == '\'' && s.back() == '\'') || (s.front() == '\"' && s.back() == '\"'))) {
        return s.substr(1, s.size() - 2);
    }
    return s;
}

static Cell parseValueToCell(const std::string& rawVal, Cell::Type targetType) {
    std::string val = cleanInput(rawVal);
    std::string lowerVal = val;
    for (auto & c : lowerVal) {
        c = (unsigned char)std::tolower(c);
    }

    if (lowerVal == "null") {
        return Cell();
    }

    if (targetType == Cell::INT) {
        return Cell(std::stoi(val));
    } else if (targetType == Cell::DOUBLE) {
        return Cell(std::stod(val));
    } else if (targetType == Cell::BOOL) {
        return Cell(val == "true" || val == "1");
    } else {
        return Cell(val); // default text type
    }
}

void InsertStmt::execute() {
    try {
        Table& table = db.getTable(tableName);
        const auto& targetColumns = table.getColumns(); // all columns
        int insertedCount = 0;

        // If specific columns are listed, find their corresponding indices in targetColumns
        std::vector<size_t> colIndices;
        if (!columnNames.empty()) {
            for (const auto& colName : columnNames) {
                size_t foundIdx = static_cast<size_t>(-1);
                for (size_t i = 0; i < targetColumns.size(); ++i) {
                    if (targetColumns[i].getName() == colName) {
                        foundIdx = i;
                        break;
                    }
                }
                if (foundIdx == static_cast<size_t>(-1)) {
                    std::string err = "Column '" + colName + "' does not exist in table '" + tableName + "'";
                    gui_log.push_back("Error: " + err);
                    std::cerr << "Error: " << err << std::endl;
                    return;
                }
                colIndices.push_back(foundIdx);
            }
        }

        for (auto const& rowVal : allRows) {
            std::vector<Cell> rowToInsert;

            if (columnNames.empty()) {
                // checking if number of columns matches full table schema
                if (rowVal.size() != targetColumns.size()) {
                    std::string err = "Table '" + tableName + "' expects " + std::to_string(targetColumns.size()) +
                                      " values, but " + std::to_string(rowVal.size()) + " provided.";
                    std::cerr << "Error. " << err << std::endl;
                    gui_log.push_back("Error: " + err);
                    return;
                }

                rowToInsert.reserve(targetColumns.size());
                for (size_t i = 0; i < rowVal.size(); ++i) {
                    rowToInsert.push_back(parseValueToCell(rowVal[i], targetColumns[i].getType()));
                }
            } else {
                // checking if number of values matches the specified column count
                if (rowVal.size() != columnNames.size()) {
                    std::string err = "Expected " + std::to_string(columnNames.size()) +
                                      " values, but " + std::to_string(rowVal.size()) + " provided.";
                    std::cerr << "Error. " << err << std::endl;
                    gui_log.push_back("Error: " + err);
                    return;
                }

                // initialize all columns as NULL, then fill the specified ones
                rowToInsert.assign(targetColumns.size(), Cell());
                for (size_t i = 0; i < rowVal.size(); ++i) {
                    size_t targetIdx = colIndices[i];
                    rowToInsert[targetIdx] = parseValueToCell(rowVal[i], targetColumns[targetIdx].getType());
                }
            }

            table.insertRow(rowToInsert);
            insertedCount++;
        }
        std::string msg = "INSERTED " + std::to_string(insertedCount) + " new rows";
        gui_log.push_back(msg);

    } catch (const std::exception& e) {
        std::string errMsg = e.what();
        gui_log.push_back("Error: " + errMsg);
    }
}