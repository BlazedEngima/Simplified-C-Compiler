#include "SymbTable.hpp"

SymbTable::SymbTable() {
    this->idx = 1;
}

const int SymbTable::get_symbol(const std::string &id) {
    auto it = this->sym_table.find(id);

    // Needs to be refactored with proper variable scoping
    if (it != this->sym_table.end())
        return it->second;

    std::cout << id << " Undeclared variable" << std::endl;
    exit(1);
    return -1;
}

int SymbTable::place_symbol(const std::string &id) {

    auto it = this->sym_table.find(id);
    
    // Checks to see if it is already in the map
    if (it != this->sym_table.end())
        return -1;

    // if (!(this->free_mem_addr.empty())) {
    //     int addr = *(this->free_mem_addr.begin());
    //     this->sym_table.emplace(id, addr);
    //     this->free_mem_addr.erase(addr);
        
    //     return addr;
    // }

    this->sym_table.emplace(id, this->idx);
    this->idx++;

    return this->idx;
}

int SymbTable::get_branch_counter(void) {
    auto it = this->sym_table.find("Branch_counter");
    return it->second;
}

void SymbTable::add_branch_counter(void) {
    int branch_counter = this->get_branch_counter();
    branch_counter++;

    auto it = this->sym_table.find("Branch_counter");
    it->second = branch_counter;
}

void SymbTable::free_temp_symbol(int id) {
    this->free_temp_symbols.insert(id);
}

std::string SymbTable::place_temp_symbol() {
    std::string temp_name;

    if (this->free_temp_symbols.empty()) {
        temp_name = std::to_string(this->idx);
        this->temp_symbols.insert(this->idx);
        this->place_symbol(temp_name);

        return temp_name;
    }

    int addr = *(this->free_temp_symbols.begin());
    temp_name = std::to_string(addr);
    this->free_temp_symbols.erase(addr);

    return temp_name;
}

bool SymbTable::is_temp_symbol(int val) {
    return this->temp_symbols.count(val) > 0;
}

// Debug
void SymbTable::print_table() {
    for (auto const &pair : this->sym_table) {
        std::cout << "{" << pair.first << "\t: " << pair.second << "}" << std::endl;
    }
}