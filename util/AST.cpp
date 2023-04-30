#include "AST.hpp"

Node::Node(node_type _type) : type(_type) {}

Node::Node(node_type _type, int _value) : type(_type), int_val(_value)  {}

Node::Node(node_type _type, std::string _id) : type(_type), var_name(_id) {}

Node::Node(node_type _type, op_type _op) : type(_type), op(_op) {}

Node::~Node() {delete this;}

int Node::handle_expression(int left_val, int right_val) {
    switch (this->op) {
        case _OROR_:
            return left_val || right_val;
            break;
        case _ANDAND_:
            return left_val && right_val;
            break;
        case _OR_OP_:
            return left_val | right_val;
            break;
        case _AND_OP_:
            return left_val & right_val;
            break;
        case _EQ_:
            return left_val == right_val;
            break;
        case _NOTEQ_:
            return left_val != right_val;
            break;
        case _LT_:
            return left_val < right_val;
            break;
        case _GT_:
            return left_val > right_val;
            break;
        case _LTEQ_:
            return left_val <= right_val;
            break;
        case _GTEQ_:
            return left_val >= right_val;
            break;
        case _SHL_OP_:
            return left_val << right_val;
            break;
        case _SHR_OP_:
            return left_val >> right_val;
            break;
        case _PLUSOP_: 
            return left_val + right_val;
            break;
        case _MINUSOP_:
            return left_val - right_val;
            break;
        case _MUL_OP_:
            return left_val * right_val;
            break;
        case _DIV_OP_:
            return left_val / right_val;
            break;
        default:
            std::cout << "Operator not used properly" << std::endl;
            exit(1);
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

Return_val Node::gen_code(MIPS &code) {

    std::string reg1 = "$2", reg2 = "$3", reg3 = "$8";

    switch (this->type) {
        case _ID_:  {
            // returns address of ID node
            assert(this->left == nullptr && this->right == nullptr);
            return std::make_tuple(addr, code.sym_table->get_symbol(this->var_name));
            break;
        }

        case _INT_NUM_: {
            // returns the integer value of the INT_NUM node
            assert(this->left == nullptr && this->right == nullptr);
            return std::make_tuple(int_num, this->int_val);
            break;
        }

        case _ARRAY_:
            // generate code for ARRAY node
            assert(this->left->type == _ID_);
            Return_type right_ret_type;
            int right_val;

            std::tie(right_ret_type, right_val) = this->right->gen_code(code);
            
            if (right_ret_type == int_num) {
                std::string id = this->left->var_name + "[" + std::to_string(right_val) + "]";
                return std::make_tuple(addr, code.sym_table->get_symbol(id));
            } 
            
            code.load_addr(reg1, right_val);
            code.sll(reg1, reg1, 2);
            break;

        case _EXP_: {
            Return_type left_ret_type, right_ret_type;
            int left_val, right_val;

            if (this->left == nullptr) {
                assert(this->right != nullptr);
                std::tie(right_ret_type, right_val) = this->right->gen_code(code);

                if (this->op == _MINUSOP_) {
                    if (right_ret_type == int_num)
                        return std::make_tuple(int_num, -right_val);

                    code.load_addr(reg1, right_val);
                    code.reg_reg_op(reg1, "$0", reg1, this->op);
                    std::string temp_symbol = code.sym_table->place_temp_symbol();

                    int new_addr = code.sym_table->get_symbol(temp_symbol);
                    code.save_reg(reg1, new_addr);

                    if (code.sym_table->is_temp_symbol(right_val))
                        code.sym_table->free_temp_symbol(right_val);

                    return std::make_tuple(addr, new_addr);

                } else if (this->op == _NOT_OP_) {
                    if (right_ret_type == int_num) {
                        code.reg_int_op(reg1, reg1, right_val, this->op);
                        return std::make_tuple(int_num, !right_val);
                    }

                    code.load_addr(reg1, right_val);
                    code.reg_reg_op(reg1, reg1, reg1, this->op);
                    std::string temp_symbol = code.sym_table->place_temp_symbol();

                    int new_addr = code.sym_table->get_symbol(temp_symbol);
                    code.save_reg(reg1, new_addr);

                    if (code.sym_table->is_temp_symbol(right_val))
                        code.sym_table->free_temp_symbol(right_val);

                    return std::make_tuple(addr, new_addr);
                }

                return std::make_tuple(error, -1);

            } else if (this->right == nullptr) {
                assert(this->left != nullptr);
                return this->left->gen_code(code);

            }

            std::tie(right_ret_type, right_val) = this->right->gen_code(code);
            std::tie(left_ret_type, left_val) = this->left->gen_code(code);

            if (left_ret_type == error || right_ret_type == error) {
                std::cout << "Code generation error: node return type error" << std::endl;
                exit(1);
            }

            // Return_type new_type = static_cast<Return_type>(left_ret_type || right_ret_type);
            Return_type new_type = (left_ret_type == addr || right_ret_type == addr) ? addr : int_num;

            std::cout << new_type << std::endl;

            // generate code for EXP node
            if (left_ret_type == int_num && right_ret_type == int_num) {
                int left_val = this->left->int_val;
                int right_val = this->right->int_val;
                
                return std::make_tuple(new_type, this->handle_expression(left_val, right_val));

            } else if (left_ret_type == addr && right_ret_type == int_num) {
                code.load_addr(reg1, left_val);
                code.reg_int_op(reg2, reg1, right_val, this->op);
                std::string temp_symbol = code.sym_table->place_temp_symbol();

                int addr = code.sym_table->get_symbol(temp_symbol);
                code.save_reg(reg2, addr);

                if (code.sym_table->is_temp_symbol(left_val))
                    code.sym_table->free_temp_symbol(left_val);

                return std::make_tuple(new_type, addr);

            } else if (left_ret_type == int_num && right_ret_type == addr) {    
                code.load_addr(reg1, right_val);
                code.reg_int_op(reg2, reg1, left_val, this->op);
                std::string temp_symbol = code.sym_table->place_temp_symbol();

                int addr = code.sym_table->get_symbol(temp_symbol);
                code.save_reg(reg2, addr);

                if (code.sym_table->is_temp_symbol(right_val))
                    code.sym_table->free_temp_symbol(right_val);

                return std::make_tuple(new_type, addr);

            } else if (left_ret_type == addr && right_ret_type == addr) {
                code.load_addr(reg1, left_val);
                code.load_addr(reg2, right_val);
                code.reg_reg_op(reg3, reg1, reg2, this->op);
                std::string temp_symbol = code.sym_table->place_temp_symbol();

                int addr = code.sym_table->get_symbol(temp_symbol);
                code.save_reg(reg3, addr);
                code.clear_reg(reg3);

                if (code.sym_table->is_temp_symbol(left_val))
                    code.sym_table->free_temp_symbol(left_val);

                if (code.sym_table->is_temp_symbol(right_val))
                    code.sym_table->free_temp_symbol(right_val);    

                return std::make_tuple(new_type, addr);

            }

            return std::make_tuple(error, -1);
            break;
        }

        case _ROOT_: {
            // generate code for ROOT node
            assert(this->op == _ASSIGN_OP_ || this->op == _READ || this->op == _WRITE || this->op == _IF || this->op == _ELSE || this->op == _DO || this->op == _WHILE);

            Return_type left_ret_type, right_ret_type;
            int left_val, right_val;

            switch (this->op) {
                case _ASSIGN_OP_: {
                    std::tie(right_ret_type, right_val) = this->right->gen_code(code);
                    std::tie(left_ret_type, left_val) = this->left->gen_code(code);

                    if (right_ret_type == addr) {
                        code.load_addr(reg1, right_val);

                        code.save_reg(reg1, code.sym_table->get_symbol(this->left->var_name));
                        return std::make_tuple(int_num, 0);

                    } else if (right_ret_type == int_num) {
                        code.load_int(reg1, right_val);

                        code.save_reg(reg1, code.sym_table->get_symbol(this->left->var_name));
                        return std::make_tuple(int_num, 0);
                    }

                    break;
                }

                case _READ: {
                    code.read_op();
                    code.load_addr(reg1, code.sym_table->get_symbol(this->right->var_name));
                    break;
                }

                case _WRITE:
                    code.write_op();

                    std::tie(right_ret_type, right_val) = this->right->gen_code(code);

                    if (right_ret_type == addr) {
                        code.load_addr("$4", right_val);
                        code.syscall();
                        return std::make_tuple(int_num, 0);

                    } else if (right_ret_type == int_num) {
                        code.load_int("$4", right_val);
                        code.syscall();
                        return std::make_tuple(int_num, 0);

                    }
                    break;

                default:
                    return std::make_tuple(error, -1);
                    break;
            }

            return std::make_tuple(error, -1);
        }
    }
}