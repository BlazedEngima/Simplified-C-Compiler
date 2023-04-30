#include "SymbTable.hpp"

SymbTable::SymbTable() {
    this->idx = 1;
}

const int SymbTable::get_symbol(std::string id) {
    auto it = this->sym_table.find(id);

    // Needs to be refactored with proper variable scoping
    if (it != this->sym_table.end())
        return it->second;

    return -1;
}

int SymbTable::place_symbol(std::string id) {

    auto it = this->sym_table.find(id);

    if (it != this->sym_table.end())
        return -1;

    if (!(this->free_mem.empty())) {
        this->sym_table.emplace(id, *(this->free_mem.begin()));
        
        return *(this->free_mem.begin());
    }

    this->sym_table.emplace(id, this->idx);
    this->idx++;

    return this->idx;
}

void SymbTable::free_symbol(std::string id) {
    auto it = this->sym_table.find(id);

    if (it == this->sym_table.end())
        return;

    this->free_mem.insert(it->second);
}

// Debug
void SymbTable::print_table() {
    for (auto const &pair : this->sym_table) {
        std::cout << "{" << pair.first << "\t: " << pair.second << "}" << std::endl;
    }
}