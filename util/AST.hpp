#ifndef AST_H
#define AST_H
#pragma once 

#include "MIPS.hpp"
#include <string>

enum node_type {_ID_, _EXP_, _INT_NUM_, _ARRAY_, _ASSIGN_, _ROOT_};

class Node {
    public:
        Node *left;
        Node *right;
        int int_val;
        std::string var_name;
        std::string op;
        node_type type;

        // Forbid copying and moving
        Node(const Node &) = delete;
        Node &operator=(const Node &) = delete;
        Node(Node &&) = delete;
        Node &operator=(Node &&) = delete;

        Node(node_type _type);
        Node(node_type _type, int value);
        Node(node_type _type, std::string str);
        Node(node_type _type, std::string id, int num);
        ~Node();

        int gen_declare_code(MIPS &code);
        int gen_code(MIPS &code);
};

#endif // AST_H