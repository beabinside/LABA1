#ifndef TABLE_H
#define TABLE_H

#include <QMainWindow>
#include <QObject>
#include <QWidget>

#pragma once
#include <string>
#include <iostream>
#include <cassert>
#include <cstdint>
#include <map>
#include <vector>
#include <cmath>
#include "cell.h"
using namespace std;
class abstr_cell;
class Table {
private:
    friend class cell_formula;

    map<int, map<int, abstr_cell*>> tabl;

    unsigned int element_counter = 0;


    vector<abstr_cell*> get_cells(pair<int, int> idx_beg, pair<int, int> idx_end) const;

    vector<abstr_cell*> get_column(int column_idx) const;

    float sum(const vector<abstr_cell*>& cells) const;

    float mul(const vector<abstr_cell*>& cells) const;

    float avg(const vector<abstr_cell*>& cells) const;

public:
    class cell_interface {
        abstr_cell*& element;
        Table* table;

        friend class Table;

        cell_interface(abstr_cell*& el, Table* table);

    public:
        cell_interface& operator=(abstr_cell* new_element);

        operator abstr_cell* () {
            return element;
        }
    };

    Table() = default;

    unsigned int size() const;

    void print() const;

    cell_interface operator[](pair<int, int> idx);

    float calc_column_sum(int column_idx) const;
    float calc_column_mul(int column_idx) const;
    float calc_column_avg(int column_idx) const;
    void output_file(Table tabl);
    void file_input(const string &name);
};

#endif // TABLE_H
