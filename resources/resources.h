#include <iostream>
#include <regex>
#include <unordered_map>


std::unordered_map<std::string, std::regex> regexes_bank = {

        // create
        {"create_table", std::regex(R"(^CREATE)", std::regex_constants::icase)},
        {"create_table_info", std::regex(R"(CREATE\s+TABLE\s+(\w+)\s+\((.*)\))", std::regex_constants::icase)},
        {"create_table_column", std::regex(R"((\w+)\s+(\w+(\(\d+\))?)\s*(NOT\s+NULL)?\s*(PRIMARY\s+KEY)?\s*(FOREIGN\s+KEY)?\s*(?:,|$))", std::regex_constants::icase)},

        // insert
        {"insert_into", std::regex(R"(^INSERT)", std::regex_constants::icase)},
        {"insert_into_info", std::regex(R"(INSERT\s+INTO\s+(\w+)\s+(\((.*)\))?\s*VALUES\s+\((.*)\))", std::regex_constants::icase)},

        // select
        {"select_from", std::regex(R"(^SELECT)", std::regex_constants::icase)},
        {"select_from_info", std::regex(R"(SELECT\s+(\*)?\s*(.*)?\s*FROM\s+(\w+)\s*(WHERE\s*(.*))?)", std::regex_constants::icase)},

        // join
        {"select_join", std::regex(R"(\b(?=.*SELECT)(?=.*JOIN).+\b)", std::regex_constants::icase)},
        {"select_join_info",std::regex(R"(SELECT\s+(\*)?\s*(.*?)?\s+FROM\s+(\w+)\s+(\bINNER\b)?(\bLEFT\b)?(\RIGHT\b)?\s+JOIN\s+(\w+)\s+ON\s+(.*?)\s*(?:WHERE\s+(.*))?(?:;|$))",std::regex_constants::icase)},

        // update
        {"update_from", std::regex(R"(^UPDATE)", std::regex_constants::icase)},
        {"update_from_info", std::regex(R"(UPDATE\s+(\w+)\s+SET\s+(.*?)\s*(?:WHERE\s*(.*))?\s*(?:$|;))", std::regex_constants::icase)},

        // delete
        {"delete_from", std::regex(R"(^DELETE)", std::regex_constants::icase)},
        {"delete_from_info", std::regex(R"(DELETE\s+FROM\s+(\w+)\s+(WHERE\s*(.*))?)", std::regex_constants::icase)},

        // drop
        {"drop_table", std::regex(R"(^DROP)", std::regex_constants::icase)},
        {"drop_table_info", std::regex(R"(DROP\s+TABLE\s+(\w+)\s*(?:$|;))", std::regex_constants::icase)},

        // where
        {"where_struct", std::regex(R"((\w+)\s*(\>\=|\<\=|\!\=|\=|\>|\<)\s*('((\w+\s*)+)')?\s*(\d+)?\s*)")},
        {"or_match", std::regex(R"(\s+OR\s+)", std::regex_constants::icase)},
        {"and_match", std::regex(R"(\s+AND\s+)", std::regex_constants::icase)},

        // helpers
        {"only_letters", std::regex(R"((\w+))")},
        {"parse_columns", std::regex(R"(((\d+)(\.\d+)?|'((\w+\s*)+)'|(\w+))\s*(?:,|$))")},
        {"parse_set_values", std::regex(R"((\w+)\s*\=\s*(\')?((\w+|\s*)+)(\')?)")},
        {"join_table_column", std::regex(R"((\w+)\.(\w+)\s*(?:,|$))")},
        {"parse_join_values", std::regex(R"((\w+)\.(\w+)\s*\=\s*(\w+)\.(\w+))")},
//        {"parse_join_values", std::regex(R"((\w+)\.(\w+)\s*(\>\=|\<\=|\!\=|\=|\>|\<)\s*((\w+)\.(\w+))?\s*('(\w+\s*)+')?\s*(\d+)?\s*)")},
};