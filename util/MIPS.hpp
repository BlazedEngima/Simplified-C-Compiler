#ifndef MIPS_H
#define MIPS_H
#pragma once

#include "SymbTable.hpp"
// #include "AST.hpp"
#include <vector>

// class SymbTable;
class Node;

enum node_type {_ID_, _EXP_, _INT_NUM_, _ARRAY_, _ASSIGN_, _ROOT_};
typedef enum op_type {_OROR_, _ANDAND_, _OR_OP_, _AND_OP_, _EQ_, _NOTEQ_, _LT_, _GT_, _LTEQ_, _GTEQ_, _SHL_OP_, _SHR_OP_, _PLUSOP_, _MINUSOP_, _MUL_OP_, _DIV_OP_, _NOT_OP_, _ASSIGN_OP_, _READ, _WRITE, _IF, _ELSE, _DO, _WHILE} op_type;
typedef enum return_type {int_num, addr, error} Return_type;
typedef std::tuple<return_type, int> Return_val;

class MIPS {
    private:
        std::vector<std::string> code;

    public:
        SymbTable *sym_table;
        std::vector<std::string> *declared_id;
        MIPS(SymbTable *sym_table, std::vector<std::string> *declared_id);

        void load_int(const std::string &reg, int num);
        std::string load_sym(const std::string &reg, const std::string &source);
        void load_addr(const std::string &reg, int addr);
        void save_reg(const std::string &reg, int addr);
        void reg_int_op(const std::string &dest_reg, const std::string &source_reg, int num, op_type op);
        void reg_reg_op(const std::string &dest_reg, const std::string &source_reg_1, const std::string &source_reg_2, op_type op);
        void clear_reg(const std::string &reg);

        void nop(void);
        void sltu(const std::string &dest_reg, const std::string &source_reg_1, const std::string &source_reg_2);
        void sltiu(const std::string &dest_reg, const std::string &source_reg, int num);
        void andi(const std::string &dest_reg, const std::string &source_reg, int num);
        void addiu(const std::string &dest_reg, const std::string &source_reg, int num);
        void addu(const std::string &dest_reg, const std::string &source_reg_1, const std::string &source_reg_2);
        void subu(const std::string &dest_reg, const std::string &source_reg_1, const std::string &source_reg_2);
        void sll(const std::string &dest_reg, const std::string &source_reg, const std::string &num_or_reg);
        void sra(const std::string &dest_reg, const std::string &source_reg, const std::string &num_or_reg);
        void syscall();
        void read_op();
        void write_op();


        void print_id_list();
        void print();
};

#endif