#pragma once

#include "../Table/Table.h"

class SQLDB {
public:

    SQLDB() = default;

    // entry point for requests to db
    void operator()(const std::string& request);

    // main operations logic
    void AddTable(const std::string& table_name, std::shared_ptr<Table>& table);
    std::vector<std::pair<int, int>> JoinTables(std::shared_ptr<Table>& left, std::shared_ptr<Table>& right, std::vector<std::vector<Column::where>>& conditions, const std::string& join);

    // main operations parsers
    void CreateTableParse(const std::string& request);
    void InsertIntoTableParse(const std::string& request);
    void SelectTableParse(const std::string& request);
    void JoinTableParse(const std::string& request);
    void DeleteTableParse(const std::string& request);
    void DropTableParse(const std::string& request);
    void UpdateTableParse(const std::string& request);

    // helpers parsers
    std::vector<std::string> ParseEnumeratedValues(const std::string& request);
    std::vector<std::pair<std::string, std::string>> ParseSetValues(const std::string& request);

    std::vector<std::pair<std::string, std::string>> ParseJoinTableColumns(const std::string& request);
    std::vector<std::vector<Column::where>> ParseWhereConstruction(const std::string& request);
    std::vector<std::vector<Column::where>> ParseOnJoinWhere(const std::string& request);

    Column::where ParseSingleWhere(const std::string& request);
    Column::where ParseSingleOn(const std::string& request);

private:
    std::unordered_map<std::string, std::shared_ptr<Table>> sqldb_;
};
