#include "MIPS.hpp"

MIPS::MIPS(SymbTable *sym_table, Map *declared_id, LabelStack *entry_labels, LabelStack *exit_labels) {
    this->sym_table = sym_table;
    this->declared_id = declared_id;
    this->entry_labels = entry_labels;
    this->exit_labels = exit_labels;

    this->code.push_back("main:");
    this->code.push_back("\tmove $fp, $sp");
}

void MIPS::load_int(const std::string &reg, int num) {
    this->code.push_back("\tli " + reg + ", " + std::to_string(num));
}

std::string MIPS::load_sym(const std::string &reg, const std::string &source) {
    int addr = this->sym_table->get_symbol(source);
    this->code.push_back("\tlw " + reg + ", " + std::to_string(addr * -4) + "($fp)");
    return reg;
}

void MIPS::load_addr(const std::string &reg, int addr) {
    this->code.push_back("\tlw " + reg + ", " + std::to_string(addr * -4) + "($fp)");
}

void MIPS::save_reg(const std::string &reg, int addr) {
    this->code.push_back("\tsw " + reg + ", " + std::to_string(addr * -4) + "($fp)");
}

void MIPS::nop(void) {
    this->code.push_back("\tnop");
}

void MIPS::sltu(const std::string &dest_reg, const std::string &source_reg_1, const std::string &source_reg_2) {
    this->code.push_back("\tsltu " + dest_reg + ", " + source_reg_1 + ", " + source_reg_2);
}

void MIPS::slt(const std::string &dest_reg, const std::string &source_reg_1, const std::string &source_reg_2) {
    this->code.push_back("\tslt " + dest_reg + ", " + source_reg_1 + ", " + source_reg_2);
}

void MIPS::sltiu(const std::string &dest_reg, const std::string &source_reg, int num) {
    this->code.push_back("\tsltiu " + dest_reg + ", " + source_reg + ", " + std::to_string(num));
}

void MIPS::slti(const std::string &dest_reg, const std::string &source_reg, int num) {
    this->code.push_back("\tslti " + dest_reg + ", " + source_reg + ", " + std::to_string(num));
}

void MIPS::and_op(const std::string &dest_reg, const std::string &source_reg_1, const std::string &source_reg_2) {
    this->code.push_back("\tand " + dest_reg + ", " + source_reg_1 + ", " + source_reg_2);
}

void MIPS::andi(const std::string &dest_reg, const std::string &source_reg, int num) {
    this->code.push_back("\tandi " + dest_reg + ", " + source_reg + ", " + std::to_string(num));
}

void MIPS::xor_op(const std::string &dest_reg, const std::string &source_reg_1, const std::string &source_reg_2) {
    this->code.push_back("\txor " + dest_reg + ", " + source_reg_1 + ", " + source_reg_2);
}

void MIPS::xori(const std::string &dest_reg, const std::string &source_reg, int num) {
    this->code.push_back("\txori " + dest_reg + ", " + source_reg + ", " + std::to_string(num));
}

void MIPS::or_op(const std::string &dest_reg, const std::string &source_reg_1, const std::string &source_reg_2) {
    this->code.push_back("\tor " + dest_reg + ", " + source_reg_1 + ", " + source_reg_2);
}

void MIPS::ori(const std::string &dest_reg, const std::string &source_reg, int num) {
    this->code.push_back("\tori " + dest_reg + ", " + source_reg + ", " + std::to_string(num));
}

void MIPS::addiu(const std::string &dest_reg, const std::string &source_reg, int num) {
    this->code.push_back("\taddiu " + dest_reg + ", " + source_reg + ", " + std::to_string(num));
}

void MIPS::addu(const std::string &dest_reg, const std::string &source_reg_1, const std::string &source_reg_2) {
    this->code.push_back("\taddu " + dest_reg + ", " + source_reg_1 + ", " + source_reg_2);
}

void MIPS::subu(const std::string &dest_reg, const std::string &source_reg_1, const std::string &source_reg_2) {
    this->code.push_back("\tsubu " + dest_reg + ", " + source_reg_1 + ", " + source_reg_2);
}

void MIPS::sll(const std::string &dest_reg, const std::string &source_reg, const std::string &num_or_reg) {
    this->code.push_back("\tsll " + dest_reg + ", " + source_reg + ", " + num_or_reg);
}

void MIPS::sra(const std::string &dest_reg, const std::string &source_reg, const std::string &num_or_reg) {
    this->code.push_back("\tsra " + dest_reg + ", " + source_reg + ", " + num_or_reg);
}

void MIPS::mul(const std::string &dest_reg, const std::string &source_reg_1, const std::string &source_reg_2) {
    this->code.push_back("\tmul " + dest_reg + ", " + source_reg_1 + ", " + source_reg_2);
}

void MIPS::div(const std::string &dest_reg, const std::string &source_reg_1, const std::string &source_reg_2) {
    this->code.push_back("\tdiv " + source_reg_1 + ", " + source_reg_2);
    this->code.push_back("\tmflo " + dest_reg);
}

void MIPS::jump(const std::string &label) {
    this->code.push_back("\tj " + label);
}

void MIPS::add_return(void) {
    this->code.push_back("\tj exit");
}

void MIPS::add_exit(void) {
    this->code.push_back("exit:");
}

void MIPS::add_branch_label(const std::string &branch_name) {
    this->code.push_back(branch_name + ":");
}

void MIPS::syscall(void) {
    this->code.push_back("\tsyscall");
}

void MIPS::read_op(void) {
    this->code.push_back("\tli $2, 5");
    this->syscall();
}

void MIPS::write_op(void) {
    this->code.push_back("\tli $2, 1");
}

void MIPS::reg_int_op(const std::string &dest_reg, const std::string &source_reg, int num, op_type op) {
    switch (op) {
        case _OROR_: {
            if (num != 0)
                this->load_int(dest_reg, 1);
            else {
                this->sltu(dest_reg, "$0", source_reg);
                this->andi(dest_reg, dest_reg, 255);    
            }
            break;
        }
        case _ANDAND_: {
            if (num == 0)
                this->load_int(dest_reg, 0);
            else {
                this->sltu(dest_reg, "$0", source_reg);
                this->andi(dest_reg, dest_reg, 255);    
            }
            break;
        }
        case _OR_OP_: {
            if (num < 0) {
                this->load_int(dest_reg, num);
                this->or_op(dest_reg, source_reg, dest_reg);
            } else 
                this->ori(dest_reg, source_reg, num);
            break;
        }
        case _AND_OP_: {
            if (num < 0) {
                this->load_int(dest_reg, num);
                this->and_op(dest_reg, source_reg, dest_reg);
            } else 
                this->andi(dest_reg, source_reg, num);
            break;
        }
        case _EQ_: {
            if (num > 0)
                this->xori(dest_reg, source_reg, num);
            else 
                this->addiu(dest_reg, source_reg, -num);

            this->sltiu(dest_reg, dest_reg, 1);
            this->andi(dest_reg, dest_reg, 255);
            break;
        }
        case _NOTEQ_: {
            if (num > 0)
                this->xori(dest_reg, source_reg, num);
            else 
                this->addiu(dest_reg, source_reg, -num);

            this->sltu(dest_reg, "$0", dest_reg);
            this->andi(dest_reg, dest_reg, 255);
            break;
        }
        case _LT_: {
            this->slti(dest_reg, source_reg, num);
            this->andi(dest_reg, dest_reg, 255);
            break;
        }
        case _GT_: {
            this->slti(dest_reg, source_reg, num + 1);
            this->xori(dest_reg, dest_reg, 1);
            this->andi(dest_reg, dest_reg, 255);
            break;
        }
            break;
        case _LTEQ_: {
            this->slti(dest_reg, source_reg, num + 1);
            this->andi(dest_reg, dest_reg, 255);
            break;
        }
        case _GTEQ_: {
            this->slti(dest_reg, source_reg, num);
            this->xori(dest_reg, dest_reg, 1);
            this->andi(dest_reg, dest_reg, 255);
            break;
        }
        case _SHL_OP_:
            this->sll(dest_reg, source_reg, std::to_string(num));
            break;
        case _SHR_OP_:
            this->sra(dest_reg, source_reg, std::to_string(num));
            break;
        case _PLUSOP_: 
            this->addiu(dest_reg, source_reg, num);
            break;
        case _MINUSOP_:
            this->addiu(dest_reg, source_reg, -num);
            break;
        case _MUL_OP_: {
            this->load_int("$9", num);
            this->mul(dest_reg, source_reg, "$9");
            break;
        }
        case _DIV_OP_: {
            if (!num){
                std::cout << "Error, division by zero" << std::endl;
                exit(1);
            }
            this->load_int("$9", num);
            this->div(dest_reg, source_reg, "$9");
            break;
        }
        case _NOT_OP_: 
            (num) ? this->load_int(dest_reg, 1) : this->load_int(dest_reg, 0);
            break;
        default:
            break;
    }
}

void MIPS::reg_reg_op(const std::string &dest_reg, const std::string &source_reg_1, const std::string &source_reg_2, op_type op) {
    switch (op) {
        case _OROR_: {
            this->xor_op(dest_reg, source_reg_1, source_reg_2);
            this->sltu(dest_reg, "$0", dest_reg);
            break;
        }
        case _ANDAND_: {
            // Do negation on A and B
            this->sltiu(source_reg_1, source_reg_1, 1);
            this->sltiu(source_reg_2, source_reg_2, 1);
            // Perform bitwise or
            this->or_op(dest_reg, source_reg_1, source_reg_2);
            // Set less than 0 !(!A || !B) = (A && B);
            this->sltu(dest_reg, "$0", dest_reg);
            this->sltiu(dest_reg, dest_reg, 1);
            break;
        }
        case _OR_OP_:
            this->or_op(dest_reg, source_reg_1, source_reg_2);
            break;
        case _AND_OP_:
            this->and_op(dest_reg, source_reg_1, source_reg_2);
            break;
        case _EQ_: {
            this->xor_op(dest_reg, source_reg_1, source_reg_2);
            this->sltiu(dest_reg, dest_reg, 1);
            this->andi(dest_reg, dest_reg, 255);
            break;
        }
        case _NOTEQ_: {
            this->xor_op(dest_reg, source_reg_1, source_reg_2);
            this->sltu(dest_reg, "$0", dest_reg);
            this->andi(dest_reg, dest_reg, 255);
            break;
        }
        case _LT_: {
            this->slt(dest_reg, source_reg_1, source_reg_2);
            this->andi(dest_reg, dest_reg, 255);
            break;
        }
        case _GT_: {
            this->slt(dest_reg, source_reg_2, source_reg_1);
            this->andi(dest_reg, dest_reg, 255);
            break;
        }
        case _LTEQ_: {
            this->slt(dest_reg, source_reg_2, source_reg_1);
            this->xori(dest_reg, dest_reg, 1);
            this->andi(dest_reg, dest_reg, 255);
            break;
        }
        case _GTEQ_: {
            this->slt(dest_reg, source_reg_1, source_reg_2);
            this->xori(dest_reg, dest_reg, 1);
            this->andi(dest_reg, dest_reg, 255);
            break;
        }
        case _SHL_OP_:
            this->sll(dest_reg, source_reg_1, source_reg_2);
            break;
        case _SHR_OP_:
            this->sra(dest_reg, source_reg_1, source_reg_2);
            break;
        case _PLUSOP_:
            this->addu(dest_reg, source_reg_1, source_reg_2);
            break;
        case _MINUSOP_:
            this->subu(dest_reg, source_reg_1, source_reg_2);
            break;
        case _MUL_OP_:
            this->mul(dest_reg, source_reg_1, source_reg_2);
            break;
        case _DIV_OP_:
            this->div(dest_reg, source_reg_1, source_reg_2);
            break;
        case _NOT_OP_:
            this->sltiu(dest_reg, source_reg_1, 1);
            this->andi(dest_reg, source_reg_1, 255);
            break;
        default:
            break;
    }
}

void MIPS::branch(const std::string &source_reg, const std::string &branch_name) {
    this->code.push_back("\tbne " + source_reg + ", $0, " + branch_name);
}

void MIPS::clear_reg(const std::string &reg) {
    this->code.push_back("\tli " + reg + ", 0");
}

void MIPS::print_id_list() {
    for (auto const &pair : *(this->declared_id)) {
        std::cout << "{" << pair.first << "\t: " << pair.second << "}" << std::endl;
    }
}

void MIPS::print() {
    for (auto const &elem : this->code) {
        std::cout << elem << std::endl;
    }
}