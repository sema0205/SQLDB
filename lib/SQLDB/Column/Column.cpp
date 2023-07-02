#include <iostream>
#include <variant>
#include <unordered_map>

#include "Column.h"

void Column::AddValue(const std::string& value, int row_id) {

    Column::variant_value new_value;

    if (value.empty()) {
        new_value = std::monostate{};
        values_.insert({row_id, new_value});
        return;
    }

    if (type_ == "int") {
        new_value = std::stoi(value);
    } else if (type_ == "varchar") {
        new_value = value;
    } else if (type_ == "double") {
        new_value = std::stod(value);
    } else if (type_ == "bool") {
        new_value = (value == "true");
    }

    values_.insert({row_id, new_value});
}

void Column::SetIsNull(bool is_null) {
    is_null_ = is_null;
}

void Column::SetPrimaryKey(bool is_primary_key) {
    is_primary_key_ = is_primary_key;
}

void Column::SetType(const std::string& type) {
    type_ = type;
}

void Column::SetName(const std::string& name) {
    column_name_ = name;
}

std::unordered_map<int, Column::variant_value>& Column::GetValues() {
    return values_;
}

bool Column::GetIsNull() const {
    return is_null_;
}

bool Column::IsPrimaryKey() const {
    return is_primary_key_;
}

std::string& Column::GetType() {
    return type_;
}

std::string& Column::GetName() {
    return column_name_;
}

void Column::PrintValue(const Column::variant_value& value) {

    if (std::holds_alternative<std::monostate>(value)) {
        std::cout << "NULL";
        return;
    }

    if (type_ == "int") {
        std::cout << std::get<int>(value);
    } else if (type_ == "varchar") {
        std::cout << std::get<std::string>(value);
    } else if (type_ == "double") {
        std::cout << std::get<double>(value);
    } else if (type_ == "bool") {
        std::cout << std::get<bool>(value);
    }

}

Column::variant_value Column::GetValue(int row) {
    return values_[row];
}

bool Column::CheckCondition(const Column::variant_value& value, const where& condition) {

    bool answer = false;

    if (std::holds_alternative<std::monostate>(value)) {
        return false;
    }

    if (type_ == "int") {
        int first = std::get<int>(value);
        int second = stoi(condition.value);
        return CompareValues(first, second, condition.action);
    } else if (type_ == "varchar") {
        std::string first = std::get<std::string>(value);
        std::string second = condition.value;
        return CompareValues(first, second, condition.action);
    } else if (type_ == "double") {
        double first = std::get<double>(value);
        double second = stod(condition.value);
        return CompareValues(first, second, condition.action);
    } else if (type_ == "bool") {
        bool first = std::get<bool>(value);
        bool second = (condition.value == "true");
        return CompareValues(first, second, condition.action);
    }

    return answer;
}

void Column::ChangeValue(const std::string& new_value, int row) {

    if (new_value == "NULL") {
        values_[row] = std::monostate{};
    }

    if (type_ == "int") {
        values_[row] = stoi(new_value);
    } else if (type_ == "varchar") {
        values_[row] = new_value;
    } else if (type_ == "double") {
        values_[row] = stod(new_value);
    } else if (type_ == "bool") {
        values_[row] = (new_value == "true");
    }

}

void Column::DeleteValue(int row) {
    values_.erase(row);
}

bool Column::IsRowExists(int row) {
    return values_.count(row);
}

std::string Column::GetStringValue(int row) {

    auto value = values_[row];
    std::string answer;

    if (std::holds_alternative<std::monostate>(value)) {
        return "NULL";
    }

    if (type_ == "int") {
        int val = std::get<int>(value);
        answer = std::to_string(val);
    } else if (type_ == "varchar") {
        answer = std::get<std::string>(value);
    } else if (type_ == "double") {
        double val = std::get<double>(value);
        answer = std::to_string(val);
    } else if (type_ == "bool") {
        bool val = std::get<bool>(value);
        answer = std::to_string(val);
    }

    return answer;
}

template<typename T>
bool Column::CompareValues(const T& first, const T& second, const std::string& action) {

    if (action == "=") {
        return first == second;
    } else if (action == "!=") {
        return first != second;
    } else if (action == ">") {
        return first > second;
    } else if (action == "<") {
        return first < second;
    } else if (action == ">=") {
        return first >= second;
    } else if (action == "<=") {
        return first <= second;
    }

    return false;
}
