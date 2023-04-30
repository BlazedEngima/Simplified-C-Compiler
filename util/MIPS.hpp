#ifndef MIPS_H
#define MIPS_H
#pragma once

#include "SymbTable.hpp"
#include <vector>

class SymbTable;

class MIPS {
    private:
        std::vector<std::string> code;

    public:
        SymbTable *sym_table;
        std::vector<std::string> *declared_id;
        MIPS(SymbTable *sym_table, std::vector<std::string> *declared_id);

        void load_int(const std::string &reg, int num);
        void load_reg(const std::string &reg, int addr);
        void save_reg(const std::string &reg, int addr);
        
        void print_id_list();
        void print();
};

#endif