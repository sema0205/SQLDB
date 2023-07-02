#pragma once

class Column {
public:

    using variant_value = std::variant<std::monostate, bool, int, double, std::string>;

    Column() = default;

    // setters and getters
    void AddValue(const std::string& value, int row_id);
    void SetIsNull(bool is_null);
    void SetPrimaryKey(bool is_primary_key);
    void SetType(const std::string& type);
    void SetName(const std::string& name);

    std::unordered_map<int, variant_value>& GetValues();
    bool GetIsNull() const;
    bool IsPrimaryKey() const;
    std::string& GetType();
    std::string& GetName();

    // helpers structures
    struct where {
        std::string column;
        std::string action;
        std::string value;
    };

    // actions with values
    void PrintValue(const variant_value& value);
    void ChangeValue(const std::string& new_value, int row);
    void DeleteValue(int row);

    // helpers
    template <typename T>
    bool CompareValues(const T& first, const T& second, const std::string& action);
    bool CheckCondition(const variant_value& value, const where& condition);
    variant_value GetValue(int row);
    std::string GetStringValue(int row);
    bool IsRowExists(int row);

private:
    std::unordered_map<int, std::variant<std::monostate, bool, int, double, std::string>> values_;
    bool is_null_ = true;
    bool is_primary_key_ = false;
    std::string type_;
    std::string column_name_;
};