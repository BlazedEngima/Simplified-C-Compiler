#ifndef SYMBTABLE_H
#define SYMBTABLE_H
#pragma once

#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>


typedef std::unordered_map<std::string, int> symbol_table;

class SymbTable {
    private:
        symbol_table sym_table;
        std::unordered_set<int> free_mem;
        int idx;

    public:
        SymbTable();
        const int get_symbol(std::string id);
        int place_symbol(std::string id);
        void free_symbol(std::string id);
        
        // Debug
        void print_table();
};

#endif // SYMBTABLE_H