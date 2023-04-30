#include "AST.hpp"

Node::Node(node_type _type) : type(_type) {}

Node::Node(node_type _type, int _value) : type(_type), int_val(_value)  {}

Node::Node(node_type _type, std::string _id) : type(_type), var_name(_id) {}

Node::Node(node_type _type, op_type _op) : type(_type), op(_op) {}

Node::~Node() {delete this;}

int Node::handle_expression(int left_val, int right_val) {
    switch (this->op) {
        case _OROR_:
            break;
        case _ANDAND_:
            break;
        case _OR_OP_:
            break;
        case _AND_OP_:
            break;
        case _EQ_:
            break;
        case _NOTEQ_:
            break;
        case _LT_:
            break;
        case _GT_:
            break;
        case _LTEQ_:
            break;
        case _GTEQ_:
            break;
        case _SHL_OP_:
            break;
        case _SHR_OP_:
            break;
        case _PLUSOP_:
            break;
        case _MINUSOP_:
            break;
        case _MUL_OP_:
            break;
        case _DIV_OP_:
            break;
        case _NOT_OP_:
            break;
        default:
            break;
   }
}

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
            code.sym_table->place_symbol(temp);
            code.declared_id->push_back(temp);
        }
        
        return 0;
    }

    else if (this->type == _ID_) {
        code.sym_table->place_symbol(this->var_name);
        code.declared_id->push_back(this->var_name);
        
        return 0;
    }

    return -1;

}

int Node::gen_code(MIPS &code) {

    std::string reg1 = "$2", reg2 = "$3", reg3 = "$8";

    switch (this->type) {
        case _ID_:  {
            // generate code for ID node
            assert(this->left == nullptr && this->right == nullptr);
            return code.sym_table->get_symbol(this->var_name);
            break;
        }

        case _INT_NUM_: {
            // generate code for INT_NUM node
            assert(this->left == nullptr && this->right == nullptr);
            return this->int_val;
            break;
        }

        case _ARRAY_:
            // generate code for ARRAY node

            break;

        case _EXP_: {
            node_type left_type = this->left->type;
            node_type right_type = this->right->type;

            // generate code for EXP node
            if (left_type == _INT_NUM_ && this->right->type == _INT_NUM_) {
                assert(this->op != _NOT_OP_);

                int left_val = this->left->int_val;
                int right_val = this->right->int_val;

                this->handle_expression(left_val, right_val);
            }

            break;
        }

        case _ROOT_:
            // generate code for ROOT node
            
            break;

    }
}