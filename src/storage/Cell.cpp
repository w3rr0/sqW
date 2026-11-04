#include "storage/Cell.hpp"
#include <stdexcept>

Cell::Type Cell::getType() const {
    // std::variant has the same order as enum Type -> we cast indexes
    return static_cast<Type>(value.index());
}

bool Cell::isNull() const {
    return getType() == NULL_TYPE;
}

void Cell::checkNullComparison(const Cell& other) const {
    if (getType() == Type::NULL_TYPE || other.getType() == Type::NULL_TYPE) {
        // Should be interpreted as "UNKNOWN"
        throw std::invalid_argument("Cannot compare NULL.");
    }
}

bool Cell::operator==(const Cell& other) const {
    if (isNull() || other.isNull()) {
        return isNull() && other.isNull();
    }

    if (getType() == Type::INT && other.getType() == Type::DOUBLE) {
        return as<int>() == other.as<double>();
    }
    if (getType() == Type::DOUBLE && other.getType() == Type::INT) {
        return as<double>() == other.as<int>();
    }

    if (getType() == other.getType()) {
        return value == other.value;
    }

    // Different types are not equal
    return false;
}

bool Cell::operator<(const Cell& other) const {
    if (isNull() || other.isNull()) {
        return isNull() && !other.isNull();
    }

    if (getType() == Type::INT && other.getType() == Type::DOUBLE) {
        return as<int>() < other.as<double>();
    }
    if (getType() == Type::DOUBLE && other.getType() == Type::INT) {
        return as<double>() < other.as<int>();
    }

    if (getType() == other.getType()) {
        return value < other.value;
    }

    // Cannot compare different types
    throw std::invalid_argument("Cannot compare (<, <=, >, >=) different types.");
}

std::ostream& operator<<(std::ostream& os, const Cell& cell) {
    std::visit([&os](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;

        if constexpr (std::is_same_v<T, std::monostate>) {
            os << "NULL";
        }
        else if constexpr (std::is_same_v<T, bool>) {
            os << (arg ? "TRUE" : "FALSE");
        }
        else {
            os << arg;
        }
    }, cell.value);

    return os;
}

std::size_t std::hash<Cell>::operator()(const Cell &cell) const noexcept {
    switch (cell.getType()) {
        case Cell::NULL_TYPE:   return 0;
        case Cell::INT:         return std::hash<double>{}(cell.as<int>());
        case Cell::DOUBLE:      return std::hash<double>{}(cell.as<double>());
        case Cell::TEXT:        return std::hash<std::string>{}(cell.as<std::string>());
        case Cell::BOOL:        return std::hash<bool>{}(cell.as<bool>());
        default:                throw std::runtime_error("Unsupported cell type");
    }
}
