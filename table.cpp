#include <string>
#include <iostream>
#include <cassert>
#include <cstdint>
#include <map>
#include <vector>
#include <cmath>
#include "table.h"
#include <fstream>
using namespace std;
vector<abstr_cell*> Table::get_cells(pair<int, int> idx_beg, pair<int, int> idx_end) const {
    if (idx_beg.first > idx_end.first) {
        swap(idx_beg.first, idx_end.first);
    }
    if (idx_beg.second > idx_end.second) {
        swap(idx_beg.second, idx_end.second);
    }

    vector<abstr_cell*> results;
    for (auto col_it = tabl.lower_bound(idx_beg.second); col_it != tabl.end() && col_it->first <= idx_end.second; col_it++) {
        const map<int, abstr_cell*>& col = col_it->second;
        for (auto row_it = col.lower_bound(idx_beg.first); row_it != col.end() && row_it->first <= idx_end.first; row_it++) {
            abstr_cell* cell = row_it->second;
            if (cell->is_float()) {
                results.push_back(cell);
            }
        }
    }

    return results;
}
int input;
vector<abstr_cell*> Table::get_column(int column_idx) const {
    return get_cells({ INT_MIN, column_idx }, { INT_MAX, column_idx });
}

float Table::sum(const vector<abstr_cell*>& cells) const {
    float result = 0;
    for (auto cell : cells) {
        cell_float* fl = dynamic_cast<cell_float*>(cell);
        if (fl) {
            result += fl->get_value();
        }
        else {
            cell_formula* form = dynamic_cast<cell_formula*>(cell);
            if (form) {
                result += form->calc_value();
            }
        }
    }
    return result;
}

float Table::mul(const vector<abstr_cell*>& cells) const {
    float result = 1;
    for (auto cell : cells) {
        cell_float* fl = dynamic_cast<cell_float*>(cell);
        if (fl) {
            result *= fl->get_value();
        }
        else {
            cell_formula* form = dynamic_cast<cell_formula*>(cell);
            if (form) {
                result *= form->calc_value();
            }
        }
    }
    return result;
}

float Table::avg(const vector<abstr_cell*>& cells) const {
    float result = 0;
    unsigned int count = 0;
    for (auto cell : cells) {
        cell_float* fl = dynamic_cast<cell_float*>(cell);
        if (fl) {
            result += fl->get_value();
            count += 1;
        }
        else {
            cell_formula* form = dynamic_cast<cell_formula*>(cell);
            if (form) {
                result += form->calc_value();
                count += 1;
            }
        }
    }
    return result / count;
}

Table::cell_interface::cell_interface(abstr_cell*& el, Table* table) : element(el), table(table)
{
}

Table::cell_interface& Table::cell_interface::operator=(abstr_cell* new_element) {
    if (!element && new_element) {
        table->element_counter++;
    }
    if (element && !new_element) {
        table->element_counter--;
    }
    if (element != nullptr && element != new_element) {
        delete element;
    }
    element = new_element;

    if (element && element->is_formula()) {
        cell_formula* fl = dynamic_cast<cell_formula*>(element);
        if (fl) {
            fl->set_table(table);
        }

    }
    return *this;
}

unsigned int Table::size() const {
    return element_counter;
}

void Table::print() const {
    for (auto& [col_idx, column] : tabl) {
        cout << "column: " << col_idx << endl;
        for (auto& [row_idx, cell] : column) {
            if (cell) {
                cout << '\t' << row_idx << ": ";
                cell->print();
            }
        }
    }
}

// [row][column]
Table::cell_interface Table::operator[](pair<int, int> idx) {
    return Table::cell_interface(tabl[idx.second][idx.first], this);
}

float Table::calc_column_sum(int column_idx) const {
    return sum(get_column(column_idx));
}
float Table::calc_column_mul(int column_idx) const {
    return mul(get_column(column_idx));
}
float Table::calc_column_avg(int column_idx) const {
    return avg(get_column(column_idx));
}

void Table::file_input(const string &name) {
    ifstream file(name);
    if (file.is_open()) {
        while (!file.eof()) {
            bool is_digit = false;
            bool is_formula = false;
            bool is_str = false;
//            float type_switch;
            int i, j;
            string type_check;
            file >> i >> j >> type_check;
            string h;
            int row[2], col[2];
            row[0] = row[1] = col[1] = col[0] = -1;
            if (type_check[0] == '=') {
                for (int i = 1; i < 4; i++)
                    h += type_check[i];
                if ((h == "sum") || (h == "avg") || (h == "mul")) {
                    bool dotcoma_counted = false;
                    bool coma_counted = false;
                    int j1 = 0;
                    if ((type_check[4] == '(') && (type_check[type_check.length() - 1] == ')')) {
                        for (unsigned int i = 5; i < type_check.length() - 1; i++) {
                            if (!coma_counted)
                                if (type_check[i] >= '0' && type_check[i] <= '9') {
                                    if (row[j1] == -1)
                                        row[j1] = 0;
                                    row[j1] = row[j1] * 10 + (type_check[i] - '0');

                                }
                                else if (!(type_check[i] >= '0' && type_check[i] <= '9') && (type_check[i] == ',')) {
                                    coma_counted = true;
                                }
                                else {
                                    type_check = "=err";
                                }
                            else if (coma_counted) {
                                if (type_check[i] >= '0' && type_check[i] <= '9') {
                                    if (col[j1] == -1)
                                        col[j1] = 0;
                                    col[j1] = col[j1] * 10 + (type_check[i] - '0');
                                }
                                else if (!(type_check[i] >= '0' && type_check[i] <= '9') && (type_check[i] == ',')) {
                                    coma_counted = true;
                                }
                                else if (!(type_check[i] >= '0' && type_check[i] <= '9') && !(type_check[i] == ',') && (type_check[i] == ';')) {
                                    dotcoma_counted = true;
                                    j1++;
                                    coma_counted = false;
                                }
                                else {
                                    type_check = "=err";
                                    break;
                                }
                            }
                        }


                    }
                    else type_check = "=err";
                }
                else type_check = "=err";
                if ((row[0] == -1) || (row[1] == -1) || (col[0] == -1) || (col[1] == -1))
                    type_check = "=err";

                 std::cout << type_check << ' ' << row[0] << ' ' << row[1] << ' ' << col[0] << ' ' << col[1];
                is_formula = true;
            }
            else {
                bool dot_counted = false;
                for (unsigned int i = 0; i < type_check.length(); i++) {
                    if ((type_check[i] == '.') && !(dot_counted))
                        dot_counted = true;
                    else
                        if ((type_check[i] < '0') || (type_check[i] > '9') || ((type_check[i] == '.') && (dot_counted))) {
                            is_str = true;
                            break;
                        }
                }
                if (!is_str) {
                    is_digit = true;
                }
                else is_str = true;
            }
            if (is_str) {
                char type_switch[256];
                for (unsigned int i=0;i<type_check.length();i++) {
                    type_switch[i]=type_check[i];
                    cout<<type_switch[i]<<endl;
                }
                cout<<"vivod"<<type_switch<<endl;
                operator[]({i,j})=new cell_string(type_switch);
            }
            else if (is_digit) {
                char type_switch[256];
                for (unsigned int i=0;i<type_check.length();i++) {
                    type_switch[i]=type_check[i];
                    cout<<type_switch[i]<<endl;
                }
                operator[]({i,j})=new cell_float(type_switch);
                abstr_cell* cell=operator[]({i,j});
                float value = static_cast<cell_float*>(cell)->get_value();
                cout<<"vivod   "<<value<<endl;
            }
            else if (is_formula) {
                if (h=="sum")
                    operator[]({i,j}) = new cell_formula(cell_formula::SUM, { row[0], col[0] }, { row[1], col[1] });
                else if (h=="mul")
                    operator[]({i,j}) = new cell_formula(cell_formula::MUL, { row[0], col[0] }, { row[1], col[1] });
                else if (h=="avg")
                    operator[]({i,j}) = new cell_formula(cell_formula::AVG, { row[0], col[0] }, { row[1], col[1] });
                else if (type_check=="=err")
                    operator[]({i,j}) = new cell_formula(cell_formula::ERR, { row[0], col[0] }, { row[1], col[1] });
            }
        }
    }
}
void Table::output_file(Table tabl) {
   ofstream file("result.txt");
   for (int i=0;i<50;i++) {
       for (int j=0;j<50;j++) {
           if (tabl[{i,j}]==nullptr)
               continue;
           else {
               abstr_cell *cell = tabl[{i,j}];
               if (cell && cell->is_float()) {
                 float value = static_cast<cell_float*>(cell)->get_value();
                 file<<i<<' '<<j<<' '<<value<<endl;
               }
               else if (cell && cell->is_formula()) {
                 float value = static_cast<cell_formula*>(cell)->calc_value();
                 file<<i<<' '<<j<<' '<<value<<endl;
               }
               if (cell && cell->is_string()) {
                   string str=static_cast<cell_string*>(cell)->get_string();
                   file<<i<<' '<<j<<' '<<str<<endl;
               }
           }
       }
   }
}

