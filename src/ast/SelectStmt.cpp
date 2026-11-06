#include "ast/SelectStmt.hpp"
#include <iostream>
#include <set>
#include <sstream>
#include "storage/Database.hpp"

extern Database db;
extern std::vector<std::string> gui_log;

// Deklaracje dla interfejsu graficznego
extern std::vector<std::vector<std::string>> gui_results;
extern std::vector<std::string> gui_headers;
extern std::string gui_error;

// helper method
static size_t getColIdx(const Table& table, const std::string& colName) {
    const auto& cols = table.getColumns();
    for (size_t i = 0; i < cols.size(); ++i) {
        if (cols[i].getName() == colName) return i;
    }
    throw std::invalid_argument("Column not found: " + colName);
}

void SelectStmt::execute() {
    try {
        Table& table = db.getTable(tableName);
        const auto& allTableCols = table.getColumns();
        const size_t totalRows = table.getRowCount();

        // mapping columns
        std::vector<size_t> projectionIdx;
        if (columns.size() == 1 && columns[0] == "*") {
            projectionIdx.reserve(allTableCols.size());
            for (size_t i = 0; i < allTableCols.size(); ++i) {
                projectionIdx.push_back(i);
            }
        } else {
            projectionIdx.reserve(columns.size());
            for (const auto& name : columns) {
                projectionIdx.push_back(getColIdx(table, name));
            }
        }

        // FILTERS: store row indices instead of allocating full rows
        std::vector<size_t> matchedIndices;
        if (where) {
            where->prepare(table);
            matchedIndices.reserve(std::min(totalRows, size_t(1024)));
            for (size_t i = 0; i < totalRows; ++i) {
                if (table.isDeleted(i)) continue;
                if (where->evaluate(table, i)) {
                    matchedIndices.push_back(i);
                }
            }
        } else {
            // Optimization for queries with LIMIT, no WHERE, no ORDER BY, no DISTINCT, no AGGREGATE
            if (orderByList.empty() && !isDistinct && aggType == Aggregate::NONE && limit >= 0) {
                size_t needed = static_cast<size_t>(std::max(0, offset) + limit);
                matchedIndices.reserve(needed);
                for (size_t i = 0; i < totalRows && matchedIndices.size() < needed; ++i) {
                    if (!table.isDeleted(i)) {
                        matchedIndices.push_back(i);
                    }
                }
            } else {
                matchedIndices.reserve(totalRows);
                for (size_t i = 0; i < totalRows; ++i) {
                    if (!table.isDeleted(i)) {
                        matchedIndices.push_back(i);
                    }
                }
            }
        }

        // AGGREGATION: computed directly without full row vectors or sorting
        if (aggType != Aggregate::NONE) {
            gui_headers.clear();
            gui_results.clear();
            gui_headers.push_back("RESULT");
            std::vector<std::string> gui_row;
            std::stringstream res_ss;

            // COUNT(*)
            if (aggType == Aggregate::COUNT && aggColumn == "*") {
                std::cout << "COUNT(*): " << matchedIndices.size() << std::endl;
                res_ss << "COUNT(*): " << matchedIndices.size();
                gui_row.push_back(res_ss.str());
                gui_results.push_back(std::move(gui_row));
                return;
            }

            size_t colIdx = getColIdx(table, aggColumn);
            const Column& targetCol = allTableCols[colIdx];

            if (aggType == Aggregate::COUNT) {
                size_t nonNullCount = 0;
                for (size_t idx : matchedIndices) {
                    if (!targetCol.isNull(idx)) nonNullCount++;
                }
                std::cout << "COUNT(" << aggColumn << "): " << nonNullCount << std::endl;
                res_ss << "COUNT(" << aggColumn << "): " << nonNullCount;
            }
            else if (aggType == Aggregate::AVG) {
                double sum = 0;
                size_t nonNullCount = 0;
                for (size_t idx : matchedIndices) {
                    if (targetCol.isNull(idx)) continue;
                    Cell c = targetCol.get(idx);
                    if (c.getType() == Cell::INT) {
                        sum += c.as<int>();
                        nonNullCount++;
                    } else if (c.getType() == Cell::DOUBLE) {
                        sum += c.as<double>();
                        nonNullCount++;
                    }
                }
                if (nonNullCount == 0) {
                    std::cout << "AVG(" << aggColumn << "): NULL" << std::endl;
                    res_ss << "AVG(" << aggColumn << "): NULL";
                } else {
                    double avg = sum / static_cast<double>(nonNullCount);
                    std::cout << "AVG(" << aggColumn << "): " << avg << std::endl;
                    res_ss << "AVG(" << aggColumn << "): " << avg;
                }
            }
            else if (aggType == Aggregate::SUM) {
                double sum = 0;
                bool hasNonNull = false;
                for (size_t idx : matchedIndices) {
                    if (targetCol.isNull(idx)) continue;
                    Cell c = targetCol.get(idx);
                    if (c.getType() == Cell::INT) {
                        sum += c.as<int>();
                        hasNonNull = true;
                    } else if (c.getType() == Cell::DOUBLE) {
                        sum += c.as<double>();
                        hasNonNull = true;
                    }
                }
                if (!hasNonNull) {
                    std::cout << "SUM(" << aggColumn << "): NULL" << std::endl;
                    res_ss << "SUM(" << aggColumn << "): NULL";
                } else {
                    std::cout << "SUM(" << aggColumn << "): " << sum << std::endl;
                    res_ss << "SUM(" << aggColumn << "): " << sum;
                }
            }
            else if (aggType == Aggregate::MIN || aggType == Aggregate::MAX) {
                Cell result;
                bool foundFirst = false;

                for (size_t idx : matchedIndices) {
                    if (targetCol.isNull(idx)) continue;
                    Cell current = targetCol.get(idx);

                    if (!foundFirst) {
                        result = std::move(current);
                        foundFirst = true;
                    } else {
                        if (aggType == Aggregate::MIN) {
                            if (current < result) result = std::move(current);
                        } else {
                            if (current > result) result = std::move(current);
                        }
                    }
                }

                std::string label = (aggType == Aggregate::MIN) ? "MIN" : "MAX";
                if (!foundFirst) {
                    std::cout << label << "(" << aggColumn << "): NULL" << std::endl;
                    res_ss << label << "(" << aggColumn << "): NULL";
                } else {
                    std::cout << label << "(" << aggColumn << "): " << result << std::endl;
                    res_ss << label << "(" << aggColumn << "): " << result;
                }
            }

            gui_row.push_back(res_ss.str());
            gui_results.push_back(std::move(gui_row));
            return;
        }

        // ORDER BY: operates on row indices
        if (!orderByList.empty()) {
            struct ResolvedOrder {
                size_t colIdx;
                bool isAsc;
            };
            std::vector<ResolvedOrder> resolved;
            resolved.reserve(orderByList.size());
            for (const auto& item : orderByList) {
                resolved.push_back({getColIdx(table, item.column), item.isAscending});
            }

            auto comparator = [&](size_t a, size_t b) {
                for (const auto& r : resolved) {
                    Cell ca = allTableCols[r.colIdx].get(a);
                    Cell cb = allTableCols[r.colIdx].get(b);
                    if (ca == cb) continue;
                    if (r.isAsc) return ca < cb;
                    return ca > cb;
                }
                return false;
            };

            if (!isDistinct && limit >= 0 && (size_t)(std::max(0, offset) + limit) < matchedIndices.size()) {
                size_t targetCount = static_cast<size_t>(std::max(0, offset) + limit);
                std::partial_sort(matchedIndices.begin(), matchedIndices.begin() + targetCount, matchedIndices.end(), comparator);
            } else {
                std::sort(matchedIndices.begin(), matchedIndices.end(), comparator);
            }
        }

        // DISTINCT and Pagination
        std::vector<std::vector<Cell>> finalRows;

        if (isDistinct) {
            std::set<std::vector<Cell>> uniqueSet;
            for (size_t rowIdx : matchedIndices) {
                std::vector<Cell> projectedRow;
                projectedRow.reserve(projectionIdx.size());
                for (size_t idx : projectionIdx) {
                    projectedRow.push_back(allTableCols[idx].get(rowIdx));
                }

                if (uniqueSet.insert(projectedRow).second) {
                    finalRows.push_back(std::move(projectedRow));
                }
            }

            // OFFSET
            if (offset > 0) {
                if (offset >= (int)finalRows.size()) {
                    finalRows.clear();
                } else {
                    finalRows.erase(finalRows.begin(), finalRows.begin() + offset);
                }
            }

            // LIMIT
            if (limit >= 0 && limit < (int)finalRows.size()) {
                finalRows.resize(limit);
            }
        } else {
            // No DISTINCT: apply OFFSET and LIMIT directly on indices before row materialization
            if (offset > 0) {
                if (offset >= (int)matchedIndices.size()) {
                    matchedIndices.clear();
                } else {
                    matchedIndices.erase(matchedIndices.begin(), matchedIndices.begin() + offset);
                }
            }

            if (limit >= 0 && limit < (int)matchedIndices.size()) {
                matchedIndices.resize(limit);
            }

            finalRows.reserve(matchedIndices.size());
            for (size_t rowIdx : matchedIndices) {
                std::vector<Cell> projectedRow;
                projectedRow.reserve(projectionIdx.size());
                for (size_t idx : projectionIdx) {
                    projectedRow.push_back(allTableCols[idx].get(rowIdx));
                }
                finalRows.push_back(std::move(projectedRow));
            }
        }

        // data for GUI
        gui_headers.clear();
        gui_results.clear();
        gui_headers.reserve(projectionIdx.size());
        for (size_t idx : projectionIdx) {
            gui_headers.push_back(allTableCols[idx].getName());
        }

        gui_results.reserve(finalRows.size());
        for (const auto& row : finalRows) {
            std::vector<std::string> gui_row;
            gui_row.reserve(row.size());
            for (const auto& cell : row) {
                switch (cell.getType()) {
                    case Cell::NULL_TYPE:
                        gui_row.push_back("NULL");
                        break;
                    case Cell::INT:
                        gui_row.push_back(std::to_string(cell.as<int>()));
                        break;
                    case Cell::TEXT:
                        gui_row.push_back(cell.as<std::string>());
                        break;
                    case Cell::BOOL:
                        gui_row.push_back(cell.as<bool>() ? "TRUE" : "FALSE");
                        break;
                    case Cell::DOUBLE: {
                        std::stringstream ss;
                        ss << cell.as<double>();
                        gui_row.push_back(ss.str());
                        break;
                    }
                    default:
                        gui_row.push_back("");
                        break;
                }
            }
            gui_results.push_back(std::move(gui_row));
        }

        // DISPLAY
        std::string msg = "SELECT: Found " + std::to_string(finalRows.size()) + " rows";
        gui_log.push_back(msg);

    } catch (const std::exception& e) {
        gui_error = e.what();
        std::string errMsg = "SELECT failed: " + gui_error;
        gui_log.push_back(errMsg);
    }
}
