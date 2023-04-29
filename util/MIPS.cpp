#include "MIPS.hpp"

MIPS::MIPS(SymbTable *sym_table) {
    this->sym_table = sym_table;

    this->code.push_back("move $fp, $sp");
}

void MIPS::load_int(const std::string &reg, int num) {
    this->code.push_back("li " + reg + ", " + std::to_string(num));
}

void MIPS::load_reg(const std::string &reg, int addr) {
    this->code.push_back("lw " + reg + ", " + std::to_string(addr * -4) + "($fp)");
}

void MIPS::save_reg(const std::string &reg, int addr) {
    this->code.push_back("sw " + reg + ", " + std::to_string(addr * -4) + "($fp)");
}

void MIPS::print() {
    for (auto const &elem : this->code) {
        std::cout << elem << std::endl;
    }
}