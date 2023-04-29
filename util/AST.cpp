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

Node::Node(node_type _type, std::string id, int num) : type(_type) {
    this->var_name = id;
    this->int_val = num;

    this->left = nullptr;
    this->right = nullptr;
}

Node::~Node() {delete this;}


int Node::gen_declare_code(MIPS &code) {

    std::string reg1 = "$2", reg2 = "$3", reg3 = "$8";
    
    if (this->type != _ROOT_) {return -1;}

    if (this->left->type == _ID_ && this->right->type == _INT_NUM_) {
        code.sym_table->place_symbol(this->left->var_name);

        code.load_int(reg1, this->right->int_val);
        code.save_reg(reg1, code.sym_table->get_symbol(this->left->var_name));

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