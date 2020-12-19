#ifndef CELL_H
#define CELL_H
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
#include "table.h"
using namespace std;


class Table;
class abstr_cell {
public:
    virtual ~abstr_cell() {}
    virtual bool is_float() const;
    virtual bool is_string() const;
    virtual bool is_formula() const;
    virtual void print() = 0;
};

class cell_float : public abstr_cell {
private:
    float* flo_obj = nullptr;

public:
    cell_float(float num);
    cell_float(const cell_float&) = default;
    cell_float(char j[256]);

    ~cell_float();

    bool is_float() const override;

    float get_value() const;

    void print() override;
};

class cell_string : public abstr_cell {
private:
    string* str_obj = nullptr;

public:
    cell_string(const cell_string&) = default;
    cell_string(char j[256]);
    string get_string() const;
    virtual ~cell_string();

    bool is_string() const override;

    void print() override;
};

class cell_formula : public abstr_cell {
public:
    enum EOp {
        SUM,
        MUL,
        AVG,
        ERR
    };

private:
    EOp operation;
    pair<int, int> idx_beg;
    pair<int, int> idx_end;
    Table* table;

public:
    cell_formula(const cell_formula&) = default;
    cell_formula(EOp op, pair<int, int> idx_beg, pair<int, int> idx_end);

    void set_table(Table* tabl);

    float calc_value() const;

    bool is_float() const override;

    bool is_formula() const override;

    void print() override;
};

#endif // CELL_H
