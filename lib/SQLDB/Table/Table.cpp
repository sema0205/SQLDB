#include <iostream>
#include <vector>
#include <variant>
#include <memory>
#include <algorithm>
#include <unordered_map>
#include <string>
#include <iomanip>

#include "Table.h"

int Table::GetCurrentRow() {
    return current_row_;
}

std::vector<std::string>& Table::GetColumnsPositions() {
    return columns_positions_;
}

std::string& Table::GetPrimaryKey() {
    return primary_key_;
}

void Table::SetColumns(const std::string& column_name, std::shared_ptr<Column>& column) {
    columns_.insert({column_name, std::move(column)});
}

void Table::SetColumnsPositions(const std::string& column_name) {
    columns_positions_.push_back(column_name);
}

void Table::SetPrimaryKey(const std::string& primary_key) {
    primary_key_ = primary_key;
}

std::string& Table::GetName() {
    return table_name_;
}

void Table::SetName(const std::string& table_name) {
    table_name_ = table_name;
}

std::shared_ptr<Column>& Table::GetColumn(const std::string& name) {
    return columns_[name];
}

void Table::AddRecord(const std::vector<std::string>& values) {
    for (int i = 0; i < columns_positions_.size(); ++i) {
        if (i == columns_positions_.size()) {
            columns_[columns_positions_[i]]->AddValue(std::string(), current_row_);
        }
        columns_[columns_positions_[i]]->AddValue(values[i], current_row_);
    }

    ++current_row_;
}

void Table::AddRecord(const std::vector<std::string>& columns, const std::vector<std::string>& values) {
    int iter_val = 0;
    for (int i = 0; i < columns_positions_.size(); ++i) {
        if (std::find(columns.begin(), columns.end(), columns_positions_[i]) != columns.end()) {
            columns_[columns_positions_[i]]->AddValue(values[iter_val], current_row_);
            ++iter_val;
        } else {
            columns_[columns_positions_[i]]->AddValue(std::string(), current_row_);
        }
    }

    ++current_row_;
}

void Table::PrintRecords(const std::vector<std::string>& columns, const std::vector<int>& rows, std::vector<std::vector<std::string>>& table) {

    table.resize(rows.size() + 1);
    int current_row = 0;

    if (columns.empty()) {
        for (const auto& column_name : columns_positions_) {
            table[current_row].push_back(column_name);
        }
    } else {
        for (const auto& column_name : columns) {
            table[current_row].push_back(column_name);
        }
    }
    ++current_row;

    for (auto row : rows) {
        if (columns.empty()) {
            for (const auto& column_name : columns_positions_) {
                auto column = columns_[column_name];
                table[current_row].push_back(column->GetStringValue(row));
            }
        } else {
            for (const auto& column_name : columns) {
                auto column = columns_[column_name];
                table[current_row].push_back(column->GetStringValue(row));
            }
        }
        ++current_row;
    }

}


void Table::FetchSelectedRecords(const std::vector<std::string>& columns, const std::vector<std::vector<Column::where>>& conditions) {

    std::vector<int> rows;
    std::vector<std::vector<std::string>> table;


    if (conditions.empty()) {
        for (int i = 0; i < current_row_; ++i) {
            if (columns_[columns_positions_[0]]->IsRowExists(i)) {
                rows.push_back(i);
            }
        }
        PrintRecords(columns, rows, table);
    } else {
        PrintRecords(columns, GetSpecificRows(conditions), table);
    }

    PrintTable(table);
}

std::vector<int> Table::GetSpecificRows(const std::vector<std::vector<Column::where>>& conditions) {

    std::vector<int> rows;
    for (int i = 0; i < current_row_; ++i) {

        if (!columns_[columns_positions_[0]]->IsRowExists(i)) {
            continue;
        }

        bool cur_and = true;

        for (const auto& condition : conditions) {
            cur_and = true;
            for (const auto& where : condition) {
                auto column = columns_[where.column];
                if (!column->CheckCondition(column->GetValue(i), where)) {
                    cur_and = false;
                    break;
                }
            }
            if (cur_and) {
                break;
            }
        }

        if (cur_and) {
            rows.push_back(i);
        }
    }


    return rows;
}

void Table::FetchUpdatedRecords(const std::vector<std::pair<std::string, std::string>>& set_columns, const std::vector<std::vector<Column::where>>& conditions) {
    std::vector<int> rows;

    if (conditions.empty()) {
        for (int i = 0; i < current_row_; ++i) {
            if (columns_[columns_positions_[0]]->IsRowExists(i)) {
                rows.push_back(i);
            }
        }
        ChangeRecords(set_columns, rows);
        return;
    }

    ChangeRecords(set_columns, GetSpecificRows(conditions));
}

void Table::ChangeRecords(const std::vector<std::pair<std::string, std::string>>& columns, const std::vector<int>& rows) {
    for (auto row : rows) {
        for (const auto& column_change : columns) {
            auto column = columns_[column_change.first];
            column->ChangeValue(column_change.second, row);
        }
    }
}

void Table::FetchDeletedRecords(const std::vector<std::vector<Column::where>>& conditions) {
    std::vector<int> rows;

    if (conditions.empty()) {
        for (int i = 0; i < current_row_; ++i) {
            if (columns_[columns_positions_[0]]->IsRowExists(i)) {
                rows.push_back(i);
            }
        }
        DeleteRecords(rows);
        return;
    }

    DeleteRecords(GetSpecificRows(conditions));
}

void Table::DeleteRecords(const std::vector<int>& rows) {
    for (auto row : rows) {
        for (const auto& column_name : columns_positions_) {
            auto column = columns_[column_name];
            column->DeleteValue(row);
        }
    }
}

void Table::NewJoinTable(std::shared_ptr<Table>& left, std::shared_ptr<Table>& right, const std::vector<std::pair<std::string, std::string>>& columns) {

    for (const auto& column_name : left->GetColumnsPositions()) {
        AddTableColumns(left, column_name);
    }

    for (const auto& column_name : right->GetColumnsPositions()) {
        if (std::find(columns_positions_.begin(), columns_positions_.end(), column_name) == columns_positions_.end()) {
            AddTableColumns(right, column_name);
        }
    }

}

void Table::AddTableColumns(std::shared_ptr<Table>& other, const std::string& column_name) {
    auto new_column = std::make_shared<Column>();
    auto copy_column = other->GetColumn(column_name);

    new_column->SetIsNull(copy_column->GetIsNull());
    new_column->SetPrimaryKey(copy_column->IsPrimaryKey());
    new_column->SetType(copy_column->GetType());
    new_column->SetName(copy_column->GetName());

    SetColumns(column_name, new_column);
    SetColumnsPositions(column_name);
}

bool Table::IsColumnExists(const std::string& column_name) {
    return columns_.count(column_name);
}

void Table::CreateInnerJoinedTable(std::shared_ptr<Table>& left, std::shared_ptr<Table>& right, const std::vector<std::pair<int, int>>& rows) {

    std::vector<std::vector<std::string>> columns(rows.size() + 1);
    std::vector<std::vector<std::string>> values(rows.size() + 1);
    int current_record = 0;

    for (const auto& row_pair : rows) {

        for (const auto& column_name : left->GetColumnsPositions()) {
                columns[current_record].push_back(column_name);
                values[current_record].push_back(left->GetColumn(column_name)->GetStringValue(row_pair.first));
        }

        for (const auto& column_name : right->GetColumnsPositions()) {
            columns[current_record].push_back(column_name);
            values[current_record].push_back(right->GetColumn(column_name)->GetStringValue(row_pair.second));
        }

        AddRecord(columns[current_record], values[current_record]);
        ++current_record;
    }

}

void Table::CreateSidedJoinedTable(std::shared_ptr<Table>& left, std::shared_ptr<Table>& right, const std::vector<std::pair<int, int>>& rows) {

    std::vector<std::vector<std::string>> columns(rows.size() + 1);
    std::vector<std::vector<std::string>> values(rows.size() + 1);
    int current_record = 0;

    for (const auto& row_pair : rows) {

        for (const auto& column_name : left->GetColumnsPositions()) {
            if (row_pair.first != -1) {
                columns[current_record].push_back(column_name);
                values[current_record].push_back(left->GetColumn(column_name)->GetStringValue(row_pair.first));
            } else if (row_pair.first == -1) {
                columns[current_record].push_back(column_name);
                values[current_record].emplace_back();
            }
        }

        for (const auto& column_name : right->GetColumnsPositions()) {
            if (row_pair.second != -1) {
                columns[current_record].push_back(column_name);
                values[current_record].push_back(right->GetColumn(column_name)->GetStringValue(row_pair.second));
            } else if (row_pair.second == -1) {
                columns[current_record].push_back(column_name);
                values[current_record].emplace_back();
            }
        }

        AddRecord(columns[current_record], values[current_record]);
        ++current_record;
    }

}

void Table::PrintTable(const std::vector<std::vector<std::string>>& table) {

    std::vector<size_t> maxColumnLengths(table[0].size(), 0);


    for (const auto& row : table) {
        for (size_t i = 0; i < row.size(); ++i) {
            if (row[i].length() > maxColumnLengths[i]) {
                maxColumnLengths[i] = row[i].length();
            }
        }
    }

    const char corner = '+';
    const char horizontalBorder = '-';
    const char verticalBorder = '|';


    std::cout << corner;
    for (size_t i = 0; i < table[0].size(); ++i) {
        std::cout << std::setfill(horizontalBorder) << std::setw(maxColumnLengths[i] + 2) << corner;
    }
    std::cout << std::endl;


    std::cout << verticalBorder;
    for (size_t i = 0; i < table[0].size(); ++i) {
        std::cout << std::setfill(' ') << std::setw(maxColumnLengths[i] + 1) << std::left << table[0][i] << verticalBorder;
    }
    std::cout << std::endl;


    std::cout << corner;
    for (size_t i = 0; i < table[0].size(); ++i) {
        std::cout << std::setfill(horizontalBorder) << std::setw(maxColumnLengths[i] + 2) << corner;
    }
    std::cout << std::endl;


    for (size_t rowIdx = 1; rowIdx < table.size(); ++rowIdx) {
        std::cout << verticalBorder;
        for (size_t colIdx = 0; colIdx < table[rowIdx].size(); ++colIdx) {
            std::cout << std::setfill(' ') << std::setw(maxColumnLengths[colIdx] + 1) << std::left << table[rowIdx][colIdx] << verticalBorder;
        }
        std::cout << "\n";
    }


    std::cout << corner;
    for (size_t i = 0; i < table[0].size(); ++i) {
        std::cout << std::setfill(horizontalBorder) << std::setw(maxColumnLengths[i] + 2) << corner;
    }
    std::cout << "\n";

}
