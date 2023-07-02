#pragma once

#include "../Column/Column.h"

class Table {
public:

    Table() = default;

    // abstract actions for table
    void AddRecord(const std::vector<std::string>& values);
    void AddRecord(const std::vector<std::string>& columns, const std::vector<std::string>& values);
    void FetchSelectedRecords(const std::vector<std::string>& columns, const std::vector<std::vector<Column::where>>& conditions);
    void FetchUpdatedRecords(const std::vector<std::pair<std::string, std::string>>& set_columns, const std::vector<std::vector<Column::where>>& conditions);
    void FetchDeletedRecords(const std::vector<std::vector<Column::where>>& conditions);

    // output/change work of table
    void PrintRecords(const std::vector<std::string>& columns, const std::vector<int>& rows, std::vector<std::vector<std::string>>& table);
    void PrintTable(const std::vector<std::vector<std::string>>& table);
    void ChangeRecords(const std::vector<std::pair<std::string, std::string>>& columns, const std::vector<int>& rows);
    void DeleteRecords(const std::vector<int>& rows);


    // helpers
    std::vector<int> GetSpecificRows(const std::vector<std::vector<Column::where>>& conditions);
    bool IsColumnExists(const std::string& column_name);

    // getters setters
    std::shared_ptr<Column>& GetColumn(const std::string& name);
    std::vector<std::string>& GetColumnsPositions();
    int GetCurrentRow();
    std::string& GetPrimaryKey();
    std::string& GetName();

    void SetColumns(const std::string& column_name, std::shared_ptr<Column>& column);
    void SetColumnsPositions(const std::string& column_name);
    void SetPrimaryKey(const std::string& primary_key);
    void SetName(const std::string& table_name);

    // joins
    void NewJoinTable(std::shared_ptr<Table>& left, std::shared_ptr<Table>& right, const std::vector<std::pair<std::string, std::string>>& columns);
    void AddTableColumns(std::shared_ptr<Table>& other, const std::string& column_name);

    void CreateInnerJoinedTable(std::shared_ptr<Table>& left, std::shared_ptr<Table>& right, const std::vector<std::pair<int, int>>& rows);
    void CreateSidedJoinedTable(std::shared_ptr<Table>& left, std::shared_ptr<Table>& right, const std::vector<std::pair<int, int>>& rows);

    // , const std::vector<std::pair<std::string, std::string>>& columns_info


private:
    std::unordered_map<std::string, std::shared_ptr<Column>> columns_;
    std::vector<std::string> columns_positions_;
    int current_row_ = 0;
    std::string primary_key_;
    std::string table_name_;
};