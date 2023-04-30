#include "AST.hpp"

Node::Node(node_type _type) : type(_type) {
    this->left = nullptr;
    this->right = nullptr;

}

Node::Node(node_type _type, int value) : type(_type) {
    this->int_val = value;

    this->left = nullptr;
    this->right = nullptr;

}

Node::Node(node_type _type, std::string str) : type(_type) {
    this->left = nullptr;
    this->right = nullptr;

    switch (this->type) {
        case _ID_:
            this->var_name = str;
            break;
        
        case _EXP_:
            this->op = str;

        default:
            break;
    }

}

Node::~Node() {delete this;}


int Node::gen_declare_code(MIPS &code) {

    std::string reg1 = "$2", reg2 = "$3", reg3 = "$8";
    
    // If assign type node
    if (this->type == _ROOT_) {
        code.declared_id->push_back(this->left->var_name);

        code.sym_table->place_symbol(this->left->var_name);

        code.load_int(reg1, this->right->int_val);
        code.save_reg(reg1, code.sym_table->get_symbol(this->left->var_name));

        return 0;
    }

    else if (this->type == _ARRAY_) {
        for (size_t i = 0; i < this->right->int_val; i++) {
            std::string temp = this->left->var_name + "[" + std::to_string(i) + "]";
            code.declared_id->push_back(temp);
        }
        
        return 0;
    }

    else if (this->type == _ID_) {
        code.declared_id->push_back(this->var_name);
        
        return 0;
    }

    return -1;

}

int Node::gen_code(MIPS &code) {

    std::string reg1 = "$2", reg2 = "$3", reg3 = "$8";

    switch (this->type) {
        case _ID_:
            // generate code for ID node
            break;

        case _INT_NUM_:
            // generate code for INT_NUM node
            break;

        case _ARRAY_:
            // generate code for ARRAY node
            break;

        case _EXP_:
            // generate code for EXP node
            break;

        case _ROOT_:
            // generate code for ROOT node
            break;

    }
}