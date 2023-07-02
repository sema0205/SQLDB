#include <vector>
#include <variant>
#include <regex>
#include <string>
#include <memory>

#include "DB.h"
#include "../../../resources/resources.h"

void SQLDB::operator()(const std::string& request) {

    if (std::regex_search(request, regexes_bank["create_table"])) {
        CreateTableParse(request);
    } else if (std::regex_search(request, regexes_bank["insert_into"])) {
        InsertIntoTableParse(request);
    } else if (std::regex_search(request, regexes_bank["select_join"])) {
        JoinTableParse(request);
    } else if (std::regex_search(request, regexes_bank["select_from"])) {
        SelectTableParse(request);
    } else if (std::regex_search(request, regexes_bank["update_from"])) {
        UpdateTableParse(request);
    } else if (std::regex_search(request, regexes_bank["delete_from"])) {
        DeleteTableParse(request);
    } else if (std::regex_search(request, regexes_bank["drop_table"])) {
        DropTableParse(request);
    }

}

void SQLDB::CreateTableParse(const std::string& request) {

    std::regex create_regex = regexes_bank["create_table_info"];
    std::smatch create_match;

    if (std::regex_search(request, create_match, create_regex)) {

        std::string table_name = create_match[1];
        std::string columns_info = create_match[2];

        auto table = std::make_shared<Table>();
        table->SetName(table_name);

        std::regex column_regex = regexes_bank["create_table_column"];
        std::smatch column_match;

        std::string::const_iterator columns_it(columns_info.cbegin());
        std::string::const_iterator columns_end(columns_info.cend());

        while(std::regex_search(columns_it, columns_end, column_match, column_regex)) {

            std::string column_name = column_match[1];
            std::string column_type = column_match[2];

            if (column_match[3].matched) {
                std::regex regex_type = regexes_bank["only_letters"];
                std::smatch match_type;
                std::regex_search(column_type, match_type, regex_type);
                column_type = match_type[1];
            }

            bool is_not_null = column_match[4].matched;
            std::string primary_key = column_match[5];

            auto column = std::make_shared<Column>();
            column->SetName(column_name);
            column->SetType(column_type);

            column->SetIsNull(!is_not_null);
            if (!primary_key.empty()) {
                table->SetPrimaryKey(column_name);
            }

            table->SetColumns(column_name, column);
            table->SetColumnsPositions(column_name);
            columns_it = column_match.suffix().first;
        }

        AddTable(table_name, table);
    }

}

void SQLDB::InsertIntoTableParse(const std::string& request) {

    std::regex insert_regex = regexes_bank["insert_into_info"];
    std::smatch insert_match;

    if (std::regex_search(request, insert_match, insert_regex)) {

        std::vector<std::string> columns_massive;
        std::vector<std::string> values_massive;

        std::string table_name = insert_match[1];
        bool first_format = insert_match[2].matched;

        std::string values_regex = insert_match[4];
        values_massive = ParseEnumeratedValues(values_regex);

        auto table = sqldb_[table_name];

        if (first_format) {
            std::string columns_info = insert_match[3];
            columns_massive = ParseEnumeratedValues(columns_info);
            table->AddRecord(columns_massive, values_massive);
            return;
        }

        table->AddRecord(values_massive);
    }

}

void SQLDB::JoinTableParse(const std::string& request) {

    std::regex join_regex = regexes_bank["select_join_info"];
    std::smatch join_match;

    if (std::regex_search(request, join_match, join_regex)) {

        std::vector<std::pair<int, int>> rows;
        std::vector<std::pair<std::string, std::string>> table_columns;
        std::vector<std::string> print_columns;
        std::vector<std::vector<Column::where>> where_massive;
        std::vector<std::vector<Column::where>> on_massive;

        std::string left_table = join_match[3];
        std::string right_table = join_match[7];
        auto table_left = sqldb_[left_table];
        auto table_right = sqldb_[right_table];
        auto new_table = std::make_shared<Table>();

        bool all_format = join_match[1].matched;
        bool is_where = join_match[9].matched;

        // right join
        if (join_match[6].matched) {
            std::swap(table_left, table_right);
        }

        if (!all_format) {
            std::string columns_regex = join_match[2];
            table_columns = ParseJoinTableColumns(columns_regex);
            for (const auto& column : table_columns) {
                print_columns.push_back(column.second);
            }
        }

        new_table->NewJoinTable(table_left, table_right, std::vector<std::pair<std::string, std::string>>());

        std::string on_conditions = join_match[8];
        on_massive = ParseOnJoinWhere(on_conditions);

        if (is_where) {
            std::string where_string = join_match[9];
            where_massive = ParseWhereConstruction(where_string);
        }

        if (join_match[4].matched) {
            rows = JoinTables(table_left, table_right, on_massive, "INNER");
            new_table->CreateInnerJoinedTable(table_left, table_right, rows);
        } else if (join_match[5].matched) {
            rows = JoinTables(table_left, table_right, on_massive, "LEFT");
            new_table->CreateSidedJoinedTable(table_left, table_right, rows);
        } else if (join_match[6].matched) {
            rows = JoinTables(table_left, table_right, on_massive, "RIGHT");
            new_table->CreateSidedJoinedTable(table_left, table_right, rows);
        }

        new_table->FetchSelectedRecords(print_columns, where_massive);
    }
}

void SQLDB::SelectTableParse(const std::string& request) {

    std::regex select_regex = regexes_bank["select_from_info"];
    std::smatch select_match;

    if (std::regex_search(request, select_match, select_regex)) {

        std::vector<std::string> columns_massive;
        std::vector<std::vector<Column::where>> where_massive;

        std::string table_name = select_match[3];
        auto table = sqldb_[table_name];

        bool all_format = select_match[1].matched;
        bool is_where = select_match[4].matched;

        if (!all_format) {
            std::string columns_regex = select_match[2];
            columns_massive = ParseEnumeratedValues(columns_regex);
        }

        if (is_where) {
            std::string where_string = select_match[5];
            where_massive = ParseWhereConstruction(where_string);
        }

        table->FetchSelectedRecords(columns_massive, where_massive);
    }

}


void SQLDB::DropTableParse(const std::string& request) {

    std::regex drop_regex = regexes_bank["drop_table_info"];
    std::smatch drop_match;

    if (std::regex_search(request, drop_match, drop_regex)) {
        std::string table_name = drop_match[1];
        sqldb_.erase(table_name);
    }

}

void SQLDB::UpdateTableParse(const std::string& request) {

    std::regex update_regex = regexes_bank["update_from_info"];
    std::smatch update_match;

    if (std::regex_search(request, update_match, update_regex)) {

        std::vector<std::pair<std::string, std::string>> set_values;
        std::vector<std::vector<Column::where>> where_massive;

        std::string table_name = update_match[1];
        auto table = sqldb_[table_name];

        std::string pairs = update_match[2];
        bool is_where = update_match[3].matched;

        set_values = ParseSetValues(pairs);

        if (is_where) {
            std::string where_string = update_match[3];
            where_massive = ParseWhereConstruction(where_string);
        }

        table->FetchUpdatedRecords(set_values, where_massive);
    }

}

void SQLDB::DeleteTableParse(const std::string& request) {

    std::regex delete_regex = regexes_bank["delete_from_info"];
    std::smatch delete_match;

    if (std::regex_search(request, delete_match, delete_regex)) {

        std::vector<std::pair<std::string, std::string>> set_values;
        std::vector<std::vector<Column::where>> where_massive;

        std::string table_name = delete_match[1];
        auto table = sqldb_[table_name];

        bool is_where = delete_match[2].matched;

        if (is_where) {
            std::string where_string = delete_match[3];
            where_massive = ParseWhereConstruction(where_string);
        }

        table->FetchDeletedRecords(where_massive);
    }

}

std::vector<std::string> SQLDB::ParseEnumeratedValues(const std::string& request) {

    std::string::const_iterator start = request.cbegin();
    std::string::const_iterator second = request.cend();

    std::vector<std::string> temp_massive;
    std::smatch column_match;

    while(std::regex_search(start, second, column_match, regexes_bank["parse_columns"])) {

        std::string temp_column;

        if (column_match[2].matched) {
            temp_column = column_match[2];
            if (column_match[3].matched) {
                std::string second_part = column_match[3];
                temp_column += second_part;
            }
        } else if (column_match[4].matched) {
            temp_column = column_match[4];
        } else if (column_match[5].matched) {
            temp_column = column_match[5];
        }

        if (!temp_column.empty()) {
            temp_massive.push_back(temp_column);
        }

        start = column_match.suffix().first;
    }

    return temp_massive;
}

std::vector<std::pair<std::string, std::string>> SQLDB::ParseJoinTableColumns(const std::string& request) {

    std::string::const_iterator start = request.cbegin();
    std::string::const_iterator second = request.cend();

    std::vector<std::pair<std::string, std::string>> temp_massive;
    std::smatch table_column_match;

    while(std::regex_search(start, second, table_column_match, regexes_bank["join_table_column"])) {

        std::string table = table_column_match[1];
        std::string column = table_column_match[2];

        if (!table.empty() && !column.empty()) {
            temp_massive.emplace_back(table, column);
        }

        start = table_column_match.suffix().first;
    }

    return temp_massive;
}


std::vector<std::pair<std::string, std::string>> SQLDB::ParseSetValues(const std::string& request) {

    std::string::const_iterator start = request.cbegin();
    std::string::const_iterator second = request.cend();

    std::vector<std::pair<std::string, std::string>> temp_massive;
    std::smatch pair_match;

    while(std::regex_search(start, second, pair_match, regexes_bank["parse_set_values"])) {
        std::string column_name = pair_match[1];
        std::string value = pair_match[3];

        if (!column_name.empty() && !value.empty()) {
            temp_massive.emplace_back(column_name, value);
        }

        start = pair_match.suffix().first;
    }

    return temp_massive;
}


void SQLDB::AddTable(const std::string& table_name, std::shared_ptr<Table>& table) {
    sqldb_.insert({table_name, std::move(table)});
}

Column::where SQLDB::ParseSingleWhere(const std::string& request) {

    Column::where condition;
    std::regex where_regex = regexes_bank["where_struct"];
    std::smatch where_match;

    if (std::regex_search(request, where_match, where_regex)) {
        std::string name = where_match[1];
        std::string sign = where_match[2];
        std::string value;

        condition.column = name;
        condition.action = sign;

        if (where_match[3].matched) {
            value = where_match[4];
            condition.value = value;
        } else {
            value = where_match[6];
            condition.value = value;
        }
    }

    return condition;
}

std::vector<std::vector<Column::where>> SQLDB::ParseWhereConstruction(const std::string& request) {

    std::vector<std::vector<Column::where>> where_conditions;
    std::regex or_regex = regexes_bank["or_match"];
    std::regex and_regex = regexes_bank["and_match"];

    std::sregex_token_iterator iter_or(request.begin(), request.end(), or_regex, -1);
    std::sregex_token_iterator end_or;

    for (; iter_or != end_or; ++iter_or) {

        std::vector<Column::where> where_vec;
        std::string or_string = iter_or->str();
        std::sregex_token_iterator iter_and(or_string.begin(), or_string.end(), and_regex, -1);
        std::sregex_token_iterator end_and;

        for (; iter_and != end_and; ++iter_and) {
            std::string condition = iter_and->str();
            where_vec.push_back(ParseSingleWhere(condition));
        }

        where_conditions.push_back(where_vec);
    }

    return where_conditions;
}

std::vector<std::vector<Column::where>> SQLDB::ParseOnJoinWhere(const std::string& request) {

    std::vector<std::vector<Column::where>> where_conditions;
    std::regex or_regex = regexes_bank["or_match"];
    std::regex and_regex = regexes_bank["and_match"];

    std::sregex_token_iterator iter_or(request.begin(), request.end(), or_regex, -1);
    std::sregex_token_iterator end_or;

    for (; iter_or != end_or; ++iter_or) {

        std::vector<Column::where> where_vec;
        std::string or_string = iter_or->str();
        std::sregex_token_iterator iter_and(or_string.begin(), or_string.end(), and_regex, -1);
        std::sregex_token_iterator end_and;

        for (; iter_and != end_and; ++iter_and) {
            std::string condition = iter_and->str();
            where_vec.push_back(ParseSingleOn(condition));
        }

        where_conditions.push_back(where_vec);
    }

    return where_conditions;
}

Column::where SQLDB::ParseSingleOn(const std::string& request) {

    Column::where condition;
    std::regex where_regex = regexes_bank["parse_join_values"];
    std::smatch where_match;

    if (std::regex_search(request, where_match, where_regex)) {
        std::string left_table_column = where_match[2];
        std::string right_table_column = where_match[4];

        condition.column = right_table_column;
        condition.action = "=";
        condition.value = left_table_column;
    }

    return condition;
}

std::vector<std::pair<int, int>> SQLDB::JoinTables(std::shared_ptr<Table>& left, std::shared_ptr<Table>& right, std::vector<std::vector<Column::where>>& conditions, const std::string& join) {

    std::vector<std::pair<int, int>> rows;

    for (int i = 0; i < left->GetCurrentRow(); ++i) {
        auto new_conditions = conditions;

        for (auto& where_mas : new_conditions) {
            for (auto& where : where_mas) {
                if (join == "RIGHT") {
                    std::swap(where.value, where.column);
                }
                where.value = left->GetColumn(where.value)->GetStringValue(i);
            }
        }

        auto matched_rows = right->GetSpecificRows(new_conditions);

        if (join == "INNER") {
            for (const auto& elem : matched_rows) {
                if (elem != -1) {
                    rows.emplace_back(i, elem);
                }
            }
        } else if (join == "LEFT" || join == "RIGHT") {
            if (matched_rows.empty()) {
                rows.emplace_back(i, -1);
            } else {
                for (const auto& elem : matched_rows) {
                    rows.emplace_back(i, elem);
                }
            }
        }

    }

    return rows;
}
