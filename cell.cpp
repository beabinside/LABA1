#include "cell.h"
#include <string>
#include <iostream>
#include <cassert>
#include <cstdint>
#include <map>
#include <vector>
#include <cmath>
#include <string>

using namespace std;

bool abstr_cell::is_float() const {
    return false;
}
bool abstr_cell::is_string() const {
    return false;
}
bool abstr_cell::is_formula() const {
    return false;
}


cell_float::cell_float(float num)
    : flo_obj(new float(num))
{
}
cell_float::cell_float(char j[256]) {
    flo_obj = new float(atof(j));
}


cell_float::~cell_float() {
    if (flo_obj) {
        delete flo_obj;
    }
}

bool cell_float::is_float() const {
    return true;
}

float cell_float::get_value() const {
    return *flo_obj;
}

void cell_float::print() {
    if (flo_obj) {
        cout << *flo_obj << endl;
    }
    else {
        cout << "EMPTY_FLOAT" << endl;
    }
}


cell_string::cell_string(char j[256]) {
    str_obj = new string(j);
}

bool cell_string::is_string() const {
    return true;
}
string cell_string::get_string() const {
    return *str_obj;
}
cell_string::~cell_string() {
    if (str_obj) {
        delete str_obj;
    }
}

void cell_string::print() {
    if ((*str_obj).length() != 0)
    {
        cout << "'" << *str_obj << "'" << endl;
    }
    else {
        cout << "EMPTY_STRING" << endl;
    }
}

cell_formula::cell_formula(EOp op, pair<int, int> idx_beg, pair<int, int> idx_end)
    : operation(op)
    , idx_beg(idx_beg)
    , idx_end(idx_end)
{
}

void cell_formula::set_table(Table* tabl) {
    table = tabl;
}

float cell_formula::calc_value() const {
    if (table == nullptr) {
        return NAN;
    }
    auto cells = table->get_cells(idx_beg, idx_end);
    switch (operation) {
    case SUM:
        return table->sum(cells);
    case MUL:
        return table->mul(cells);
    case AVG:
        return table->avg(cells);
    case ERR:
        return NAN;
    }
}

bool cell_formula::is_float() const {
    return true;
}

bool cell_formula::is_formula() const {
    return true;
}

void cell_formula::print() {
    string cmd;
    switch (operation) {
    case SUM:
        cmd = "SUM";
        break;
    case MUL:
        cmd = "MUL";
        break;
    case AVG:
        cmd = "AVG";
        break;
    case ERR:
        cmd = "ERR";
        break;
    }
    cout << cmd << "((" << idx_beg.first << "," << idx_beg.second << "), (" << idx_end.first << "," << idx_end.second << ")) = " << calc_value() << endl;
}
