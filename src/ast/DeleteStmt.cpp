#include "ast/DeleteStmt.hpp"
#include <iostream>
#include "storage/Database.hpp"

extern Database db;
extern std::vector<std::string> gui_log;

void DeleteStmt::execute() {
    try {
        Table& table = db.getTable(tableName);
        int deleteCount = 0;

        if (where) {
            where->prepare(table);
        }

        const size_t totalRows = table.getRowCount();
        for (size_t i = 0; i < totalRows; i++) {
            // skipping rows that are marked as deleted
            if (table.isDeleted(i)) continue;

            // WHERE condition evaluated directly by row index without allocating row
            if (!where || where->evaluate(table, i)) {
                table.deleteRow(i);
                deleteCount++;
            }
        }
        std::string msg = "DELETED " + std::to_string(deleteCount) + " rows";
        gui_log.push_back(msg);

    } catch (const std::exception& e) {
        std::string errMsg = e.what();
        gui_log.push_back("Error: " + errMsg);
    }
}
