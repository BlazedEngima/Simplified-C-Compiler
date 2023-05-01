#ifndef AST_H
#define AST_H
#pragma once 

#include "MIPS.hpp"
#include <tuple>
#include <string>
#include <assert.h>

// enum node_type {_ID_, _EXP_, _INT_NUM_, _ARRAY_, _ASSIGN_, _ROOT_};
// typedef enum op_type {_OROR_, _ANDAND_, _OR_OP_, _AND_OP_, _EQ_, _NOTEQ_, _LT_, _GT_, _LTEQ_, _GTEQ_, _SHL_OP_, _SHR_OP_, _PLUSOP_, _MINUSOP_, _MUL_OP_, _DIV_OP_, _NOT_OP_} op_type;
// typedef enum return_type {int_num, addr, error} Return_type;
// typedef std::tuple<return_type, int> Return_val;

class Node {
    public:
        Node *left = nullptr;
        Node *right = nullptr;
        int int_val;
        std::string var_name;
        op_type op;
        node_type type;

        // Forbid copying and moving
        Node(const Node &) = delete;
        Node &operator=(const Node &) = delete;
        Node(Node &&) = delete;
        Node &operator=(Node &&) = delete;

        Node(node_type _type);
        Node(node_type _type, int _value);
        Node(node_type _type, std::string _id);
        Node(node_type _type, op_type _op);
        ~Node();

        int handle_expression(int left_val, int right_val);
        int gen_declare_code(MIPS &code);
        // Return_val gen_code(MIPS &code);
        int gen_code(MIPS &code);
};

#endif // AST_H