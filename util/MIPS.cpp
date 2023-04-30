#include "MIPS.hpp"

MIPS::MIPS(SymbTable *sym_table, std::vector<std::string> *declared_id) {
    this->sym_table = sym_table;
    this->declared_id = declared_id;

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

void MIPS::sltiu(const std::string &dest_reg, const std::string &source_reg, int num) {
    this->code.push_back("\tsltiu " + dest_reg + ", " + source_reg + ", " + std::to_string(num));
}

void MIPS::andi(const std::string &dest_reg, const std::string &source_reg, int num) {
    this->code.push_back("\tandi " + dest_reg + ", " + source_reg + ", " + std::to_string(num));
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

void MIPS::sll(const std::string &dest_reg, const std::string &source_reg, int num) {
    this->code.push_back("\tsll " + dest_reg + ", " + source_reg + std::to_string(num));
}

void MIPS::syscall() {
    this->code.push_back("\tsyscall");
}

void MIPS::read_op() {
    this->code.push_back("\tli $2, 5");
    this->syscall();
}

void MIPS::write_op() {
    this->code.push_back("\tli $2, 1");
}

void MIPS::reg_int_op(const std::string &dest_reg, const std::string &source_reg, int num, op_type op) {
    switch (op) {
        case _OROR_: {
            if (num != 0)
                this->load_int(dest_reg, 1);
            else {
                this->load_sym(dest_reg, source_reg);
                this->sltu(dest_reg, "$0", dest_reg);
                this->andi(dest_reg, dest_reg, 255);    
            }

            break;
        }
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
            this->addiu(dest_reg, source_reg, num);
            break;
        
        case _MINUSOP_:
            this->addiu(dest_reg, source_reg, -num);
            break;

        case _MUL_OP_:
            break;
        case _DIV_OP_:
            break;
        case _NOT_OP_: {
            (num) ? this->load_int(dest_reg, 1) : this->load_int(dest_reg, 0);
        }
            break;
        default:
            break;
    }
}

void MIPS::reg_reg_op(const std::string &dest_reg, const std::string &source_reg_1, const std::string &source_reg_2, op_type op) {
    switch (op) {
        case _OROR_: {
            break;
        }
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
            this->addu(dest_reg, source_reg_1, source_reg_2);
            break;
        case _MINUSOP_:
            this->subu(dest_reg, source_reg_1, source_reg_2);
            break;
        case _MUL_OP_:
            break;
        case _DIV_OP_:
            break;
        case _NOT_OP_:
            this->sltiu(dest_reg, source_reg_1, 1);
            this->andi(dest_reg, source_reg_1, 255);
            break;
        default:
            break;
    }    
}

void MIPS::clear_reg(const std::string &reg) {
    this->code.push_back("\tli " + reg + ", 0");
}

void MIPS::print_id_list() {
    for (auto const &elem : *(this->declared_id)) {
        std::cout << elem << std::endl;
    }
}
void MIPS::print() {
    for (auto const &elem : this->code) {
        std::cout << elem << std::endl;
    }
}