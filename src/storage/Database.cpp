/**
 * @file Database.cpp
 * @brief Implementation of Database class.
 */

#include "storage/Database.hpp"
#include <fstream>
#include <cereal/archives/binary.hpp>

Database::Database(std::string dbName)
    : name(std::move(dbName)) {}

void Database::createTable(const std::string& tableName) {
    if (hasTable(tableName)) {
        throw std::invalid_argument("Error CREATE: Table '" + tableName + "' already exist in database '" + name + "'.");
    }

    tables.emplace(tableName, Table(tableName));
}

void Database::dropTable(const std::string& tableName) {
    if (tables.erase(tableName) == 0) { // Table not found
        throw std::invalid_argument("Error DROP: Table '" + tableName + "' does not exist.");
    }
}

bool Database::hasTable(const std::string& tableName) const {
    return tables.find(tableName) != tables.end();
}

const Table& Database::getTable(const std::string& tableName) const {
    auto it = tables.find(tableName);

    if (it != tables.end()) {
        return it->second; // Value (Table object)
    }
    throw std::invalid_argument("Error: Table '" + tableName + "' was not found in the database.");
}

Table& Database::getTable(const std::string& tableName) {
    return const_cast<Table&>(static_cast<const Database*>(this)->getTable(tableName));
}

std::vector<std::string> Database::getTableNames() const {
    std::vector<std::string> namesTab;
    namesTab.reserve(tables.size());
    for (const auto& [n, table] : tables) {
        namesTab.push_back(n);
    }
    return namesTab;
}

void Database::vacuum() {
    for (auto& pair : tables) {
        pair.second.vacuum();
    }
}

void Database::vacuum(const std::string& tableName) {
    getTable(tableName).vacuum();
}

void Database::saveToFile(const std::string& filepath) const {
    std::ofstream os(filepath, std::ios::binary);
    if (!os.is_open()) {
        throw std::runtime_error("Error: Could not open file for writing: " + filepath);
    }

    cereal::BinaryOutputArchive archive(os);
    archive(*this);
}

void Database::loadFromFile(const std::string& filepath) {
    std::ifstream is(filepath, std::ios::binary);
    if (!is.is_open()) {
        throw std::runtime_error("Error: Could not open file for reading: " + filepath);
    }

    cereal::BinaryInputArchive archive(is);
    archive(*this);
}
