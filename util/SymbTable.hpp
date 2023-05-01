#ifndef SYMBTABLE_H
#define SYMBTABLE_H
#pragma once

#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <assert.h>


typedef std::unordered_map<std::string, int> Map;

class SymbTable {
    private:
        Map sym_table;
        // Data type is int because we use the idx as the temp var name
        std::unordered_set<int> temp_symbols;
        std::unordered_set<int> free_temp_symbols;
        int idx;

    public:
        SymbTable();
        const int get_symbol(const std::string &id);
        int place_symbol(const std::string &id);
        void free_temp_symbol(int id);
        std::string place_temp_symbol();

        bool is_temp_symbol(int val);
        
        // Debug
        void print_table();
};

#endif // SYMBTABLE_H