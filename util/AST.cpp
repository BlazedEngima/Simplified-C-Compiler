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
        case _DIV_OP_: {
            if (!right_val) {
                return left_val;
            }
            return left_val / right_val;
            break;
        }
        default:
            std::cout << "Operator not used properly" << std::endl;
            exit(1);
            break;
   }
}

int Node::gen_declare_code(MIPS &code) {
    std::string reg1 = "$2", reg2 = "$3", reg3 = "$8";
    code.sym_table->place_symbol("Branch_counter");
    
    // If assign type node
    if (this->type == _ROOT_) {
        assert(this->right->type == _INT_NUM_);

        code.declared_id->emplace(this->left->var_name, this->right->int_val);
        code.sym_table->place_symbol(this->left->var_name);

        code.load_int(reg1, this->right->int_val);
        code.save_reg(reg1, code.sym_table->get_symbol(this->left->var_name));
        
        this->left->int_val = this->right->int_val;
        return 0;
    }

    else if (this->type == _ARRAY_) {
        for (size_t i = 0; i < this->right->int_val; i++) {
            std::string temp = this->left->var_name + "[" + std::to_string(i) + "]";
            code.sym_table->place_symbol(temp);
            code.declared_id->emplace(temp, 0);
        }
        
        return 0;
    }

    else if (this->type == _ID_) {
        code.sym_table->place_symbol(this->var_name);
        code.declared_id->emplace(this->var_name, 0);
        
        return 0;
    }
    return -1;
}

int Node::evaluate_root(MIPS &code) {
    std::string reg1 = "$2", reg2 = "$3", reg3 = "$8";
    switch (this->op) {
        case _ASSIGN_OP_: {
            int left_addr = this->left->gen_code(code);
            int right_addr = this->right->gen_code(code);
            code.sym_table->free_temp_symbol(right_addr);
            code.sym_table->free_temp_symbol(left_addr);
            
            if (right_addr != -1) 
                code.load_addr(reg1, right_addr);
            else 
                code.load_int(reg1, this->right->int_val);
            
            code.save_reg(reg1, left_addr);
            return 0;
            break;
        }
        case _READ: {
            code.read_op();
            code.save_reg(reg1, code.sym_table->get_symbol(this->right->var_name));
            return 0;
            break;
        }
        case _WRITE: {
            int right_addr = this->right->gen_code(code);
            code.sym_table->free_temp_symbol(right_addr);
            
            if (right_addr != -1)
                code.load_addr("$4", right_addr);
            else
                code.load_int("$4", this->right->int_val);
                
            code.write_op();
            code.syscall();
            return 0;
            break;
        }
        case _IF: {
            int left_addr = this->left->gen_code(code);
            code.sym_table->free_temp_symbol(left_addr);
            code.load_addr(reg1, left_addr);

            int branch_num = code.sym_table->get_branch_counter();
            std::string branch_name = "if_branch_" + std::to_string(branch_num);
            code.sym_table->add_branch_counter();

            code.branch_if(reg1, branch_name);
            for (auto it = this->code_block->begin(); it != this->code_block->end(); it++) {
                (*it)->gen_code(code);
            }
            code.add_branch_label(branch_name);
            return 0;
            break;
        }
        case _ELSE: {
            int left_addr = this->left->gen_code(code);
            code.sym_table->free_temp_symbol(left_addr);

            std::string branch_exit_name = code.exit_labels->top();
            code.exit_labels->pop();

            for (auto it = this->code_block->begin(); it != this->code_block->end(); it++) {
                (*it)->gen_code(code);
            }
            code.add_branch_label(branch_exit_name);
            return 0;
            break;
        }
        case _DO: {
            int branch_num = code.sym_table->get_branch_counter();
            std::string branch_name = "do_while_branch_" + std::to_string(branch_num);
            std::string exit_branch_name = "exit_do_while_branch_" + std::to_string(branch_num);

            code.add_branch_label(branch_name);
            for (auto it = this->code_block->begin(); it != this->code_block->end(); it++) {
                (*it)->gen_code(code);
            }

            int left_addr = this->left->gen_code(code);
            code.sym_table->free_temp_symbol(left_addr);

            code.load_addr(reg1, left_addr);
            code.branch(reg1, branch_name);
            code.jump(exit_branch_name);
            code.add_branch_label(exit_branch_name);
            return 0;
            break;
        }
        case _WHILE: {
            int branch_num = code.sym_table->get_branch_counter();
            std::string branch_name = "while_branch_" + std::to_string(branch_num);
            std::string exit_branch_name = "exit_while_branch_" + std::to_string(branch_num);
            code.sym_table->add_branch_counter();

            code.jump(exit_branch_name);
            code.add_branch_label(branch_name);
            for (auto it = this->code_block->begin(); it != this->code_block->end(); it++) {
                (*it)->gen_code(code);
            }
            code.add_branch_label(exit_branch_name);

            int left_addr = this->left->gen_code(code);
            code.sym_table->free_temp_symbol(left_addr);

            code.load_addr(reg1, left_addr);
            code.branch(reg1, branch_name);
            return 0;
            break;
        }
        case _RETURN_: {
            code.add_return();
            return 0;
            break;
        }

        default:
            return -2;
            break;
    }
    return -2;
}

int Node::evaluate_flip_op(MIPS &code) {
    std::string reg1 = "$2", reg2 = "$3", reg3 = "$8";

    int right_addr = this->right->gen_code(code);
    if (this->op == _MINUSOP_)
        this->int_val = -1 * this->right->int_val;
    else if (this->op == _NOT_OP_)
        this->int_val = !(this->right->int_val);

    if (code.sym_table->is_temp_symbol(right_addr))
        code.sym_table->free_temp_symbol(right_addr);

    if (right_addr == -1) {
        if (this->op == _NOT_OP_)
            code.reg_int_op(reg1, reg1, this->right->int_val, this->op);
        return -1;
    }

    code.load_addr(reg1, right_addr);
    if (this->op == _MINUSOP_)
        code.reg_reg_op(reg1, "$0", reg1, this->op);
    else if (this->op == _NOT_OP_)
        code.reg_reg_op(reg1, reg1, reg1, this->op);

    std::string temp_symbol = code.sym_table->place_temp_symbol();
    int new_addr = code.sym_table->get_symbol(temp_symbol);
    code.save_reg(reg1, new_addr);

    return new_addr;
}

int Node::gen_code(MIPS &code) {
    std::string reg1 = "$2", reg2 = "$3", reg3 = "$8", reg4 = "$9";

    switch (this->type) {
        case _ROOT_: {
            return this->evaluate_root(code);
            break;
        }
        case _ID_: {
            auto it = code.declared_id->find(this->var_name);
            if (it != code.declared_id->end())
                this->int_val = code.declared_id->at(this->var_name);
            else {
                std::cout << "Error, variable not declared" << std::endl;
                exit(1);
            }
            return code.sym_table->get_symbol(this->var_name);
            break;
        }
        case _INT_NUM_: 
            return -1;
            break;
        case _ARRAY_: {
            int right_addr = this->right->gen_code(code);
            code.sym_table->free_temp_symbol(right_addr);

            if (this->right->int_val < 0) {
                std::cout << "Error, array index cannot evaluate to a negative number" << std::endl;
                exit(1);
            }

            if (right_addr == -1) {
                std::string id = this->left->var_name + "[" + std::to_string(this->right->int_val) + "]";
                return code.sym_table->get_symbol(id);
            }

            int array_head = code.sym_table->get_symbol(this->left->var_name + "[0]");
            std::string temp_symbol = code.sym_table->place_temp_symbol();
            int new_addr = code.sym_table->get_symbol(temp_symbol);

            code.move(reg4, "$fp");
            code.load_int(reg3, -4 * array_head);
            code.load_addr(reg2, right_addr);
            code.sll(reg2, reg2, std::to_string(2));
            code.subu(reg2, "$0", reg2);
            code.add(reg2, reg2, reg3);
            code.add(reg4, reg4, reg2);
            code.load_reg(reg1, reg4);
            code.save_reg(reg1, new_addr);
            
            return new_addr;
            break;
        }
        case _EXP_: {
            int left_addr, right_addr;
            if (this->left == nullptr)
                return this->evaluate_flip_op(code);

            // If branch
            else if (this->right == nullptr) {
                int left_addr = this->left->gen_code(code);
                code.sym_table->free_temp_symbol(left_addr);
                code.load_addr(reg1, left_addr);

                int branch_num = code.sym_table->get_branch_counter();
                std::string branch_name = "if_branch_" + std::to_string(branch_num);
                std::string branch_exit_name = "if_exit_branch_" + std::to_string(branch_num);
                code.sym_table->add_branch_counter();
                
                code.exit_labels->push(branch_exit_name);
                code.branch_if(reg1, branch_name);
                for (auto it = this->code_block->begin(); it != this->code_block->end(); it++) {
                    (*it)->gen_code(code);
                }
                code.jump(branch_exit_name);
                code.add_branch_label(branch_name);

                return 0;
            }

            left_addr = this->left->gen_code(code);
            right_addr = this->right->gen_code(code);
            code.sym_table->free_temp_symbol(left_addr);
            code.sym_table->free_temp_symbol(right_addr);

            this->int_val = this->handle_expression(this->left->int_val, this->right->int_val);
            std::string temp_symbol = code.sym_table->place_temp_symbol();
            int new_addr = code.sym_table->get_symbol(temp_symbol);

            if (left_addr == -1 && right_addr == -1)
                return -1;
            
            else if (left_addr != -1 && right_addr != -1) {
                code.load_addr(reg1, left_addr);
                code.load_addr(reg2, right_addr);
                code.reg_reg_op(reg3, reg1, reg2, this->op);
                code.save_reg(reg3, new_addr);

                return new_addr;
            } else if (left_addr != -1 && right_addr == -1) {
                code.load_addr(reg1, left_addr);
                code.reg_int_op(reg2, reg1, this->right->int_val, this->op);
                code.save_reg(reg2, new_addr);

                return new_addr;
            } else if (left_addr == -1 && right_addr != -1) {
                code.load_addr(reg1, right_addr);
                code.reg_int_op(reg2, reg1, this->left->int_val, this->op);
                code.save_reg(reg2, new_addr);

                return new_addr;
            }
            return -2;
            break;
        }
        default:
            return -2;
            break;
    }
    return -2;
}