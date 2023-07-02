#include <iostream>
#include <regex>
#include <string>
#include <vector>
#include <variant>

#include "../lib/SQLDB/DB/DB.h"

int main() {

    SQLDB db;

    while(true) {
        std::string request;
        std::getline(std::cin, request);
        if (request == "exit") {break;}
        db(request);
    }

    return 0;
}
