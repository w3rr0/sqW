#include "ast/UpdateStmt.hpp"
#include <iostream>
#include <vector>
#include <storage/Database.hpp>

extern Database db;
extern std::vector<std::string> gui_log;

// method for cleaning '' / ""
static std::string clearQuotes(std::string s) {
    if (s.size() >= 2 && ((s.front() == '\'' && s.back() == '\'') || (s.front() == '\"' && s.back() == '\"'))) {
        return s.substr(1, s.size() - 2);
    }
    return s;
}

// helper method that creates a Cell object with the correct type
Cell createCellUpdate(const std::string& val, Cell::Type type) {
    std::string clean = clearQuotes(val);
    std::string lowerVal = clean;

    for (auto & c : lowerVal) {
        c = (unsigned char)std::tolower(c);
    }

    if (lowerVal == "null") {
        return Cell();
    }

    if (type == Cell::INT) return Cell(std::stoi(clean));
    if (type == Cell::DOUBLE) return Cell(std::stod(clean));
    if (type == Cell::BOOL) return Cell(clean == "true" || clean == "1");
    return Cell(clean); // TEXT
}

void UpdateStmt::execute() {
    try {
        Table& table = db.getTable(tableName);
        int updateCount = 0;

        struct PreparedUpdate {
            Column& col;
            Cell newValue;
        };

        std::vector<PreparedUpdate> preparedUpdates;
        preparedUpdates.reserve(columnsToUpdate.size());
        for (auto const& [colName, newVal] : columnsToUpdate) {
            Column& col = table.getColumn(colName);
            preparedUpdates.push_back({col, createCellUpdate(newVal, col.getType())});
        }

        if (where) {
            where->prepare(table);
        }

        const size_t totalRows = table.getRowCount();
        for (size_t i = 0; i < totalRows; i++) {
            if (table.isDeleted(i)) continue;

            if (!where || where->evaluate(table, i)) {
                for (auto& upd : preparedUpdates) {
                    upd.col.set(i, upd.newValue);
                }
                updateCount++;
            }
        }

        std::string msg = "UPDATED " + std::to_string(updateCount) + " rows";
        gui_log.push_back(msg);

    } catch (const std::exception& e) {
        std::string errMsg = e.what();
        gui_log.push_back("Error: " + errMsg);
    }
}