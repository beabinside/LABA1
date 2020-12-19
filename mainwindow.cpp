#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <string>
#include <QTableWidget>
#include <QString>
#include "table.h"
#include <QTableWidgetItem>
#include <fstream>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setCentralWidget(ui->table_ui);
    ui->table_ui->setColumnCount(500);
    ui->table_ui->setRowCount(500);
    Table table;
}
MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionload_triggered()
{
    QString file_name = QFileDialog::getOpenFileName(this, "Open the file");
    string path=file_name.toStdString();
    table.file_input(path);
    ui->table_ui->setItem(5,5,new QTableWidgetItem(file_name));
    for (int i=0;i<500;i++) {
        for (int j=0;j<500;j++) {
            if (table[{i,j}]==nullptr)
                continue;
            else {
                ui->table_ui->setItem(i+6,5,new QTableWidgetItem(file_name));
                abstr_cell *cell = table[{i,j}];
                cout<<table[{i,j}]<<endl;
                if (cell && cell->is_float()) {
                  float value = static_cast<cell_float*>(cell)->get_value();
                  cout<<value<<"is"<<endl;
                  QString inter;
                  inter.setNum(value);
                  ui->table_ui->setItem(i,j,new QTableWidgetItem(inter));
                }
                else if (cell && cell->is_formula()) {
                  float value = static_cast<cell_formula*>(cell)->calc_value();

                  QString inter;
                  inter.setNum(value);
                  ui->table_ui->setItem(i,j,new QTableWidgetItem(inter));
                }
                if (cell && cell->is_string()) {
                    string str=static_cast<cell_string*>(cell)->get_string();
                    QString inter =  QString::fromStdString(str);
                    cout<<str;
                    ui->table_ui->setItem(i,j,new QTableWidgetItem(inter));
                }
            }
        }
    }
}

void MainWindow::on_actionsave_triggered() {
    table.output_file(table);
}


void MainWindow::on_actionclear_triggered() {
    for (int i=0;i<50;i++)
        for(int j=0;j<50;j++)
            table[{i,j}]=nullptr;
    ui->table_ui->clear();
}
void MainWindow::on_table_ui_cellChanged(int row, int column) {
    QString check;
    QTableWidgetItem *item_t = ui->table_ui->item(row,column);
    if (NULL != item_t) {
       check = item_t->text();
       string s=check.toStdString();
       bool is_digit = false;
       bool is_formula = false;
       bool is_str = false;
//       float type_switch;
//       int i, j;

       string h;
       int row1[2], col1[2];
       row1[0] = row1[1] = col1[1] = col1[0] = -1;
       if (s[0] == '=') {
           for (int i = 1; i < 4; i++)
               h += s[i];
           if ((h == "sum") || (h == "avg") || (h == "mul")) {
               bool dotcoma_counted = false;
               bool coma_counted = false;
               int j1 = 0;
               if ((s[4] == '(') && (s[s.length() - 1] == ')')) {
                   for (unsigned int i = 5; i < s.length() - 1; i++) {
                       if (!coma_counted)
                           if (s[i] >= '0' && s[i] <= '9') {
                               if (row1[j1] == -1)
                                   row1[j1] = 0;
                               row1[j1] = row1[j1] * 10 + (s[i] - '0');

                           }
                           else if (!(s[i] >= '0' && s[i] <= '9') && (s[i] == ',')) {
                               coma_counted = true;
                           }
                           else {
                               s = "=err";
                           }
                       else if (coma_counted) {
                           if (s[i] >= '0' && s[i] <= '9') {
                               if (col1[j1] == -1)
                                   col1[j1] = 0;
                               col1[j1] = col1[j1] * 10 + (s[i] - '0');
                           }
                           else if (!(s[i] >= '0' && s[i] <= '9') && (s[i] == ',')) {
                               coma_counted = true;
                           }
                           else if (!(s[i] >= '0' && s[i] <= '9') && !(s[i] == ',') && (s[i] == ';')) {
                               dotcoma_counted = true;
                               j1++;
                               coma_counted = false;
                           }
                           else {
                               s = "=err";
                               break;
                           }
                       }
                   }
               }
               else s = "=err";
           }
           else s = "=err";
           if ((row1[0] == -1) || (row1[1] == -1) || (col1[0] == -1) || (col1[1] == -1))
               s = "=err";

            std::cout << s << ' ' << row1[0] << ' ' << row1[1] << ' ' << col1[0] << ' ' << col1[1];
           is_formula = true;
       }
       else {
           bool dot_counted = false;
           for (unsigned int i = 0; i < s.length(); i++) {
               if ((s[i] == '.') && !(dot_counted))
                   dot_counted = true;
               else
                   if ((s[i] < '0') || (s[i] > '9') || ((s[i] == '.') && (dot_counted))) {
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
           for (unsigned long i=0;i<s.length();i++)
                {
               type_switch[i]=s[i];
           }
               cout<<"vivod"<<type_switch<<endl;
           table[{row,column}]=new cell_string(type_switch);
       }
       else if (is_digit) {
           char type_switch[256];
           for (unsigned int i=0;i<s.length();i++)
                {
               type_switch[i]=s[i];
           }
           table[{row,column}]=new cell_float(type_switch);
           abstr_cell* cell=table[{row,column}];
           float value = static_cast<cell_float*>(cell)->get_value();
           cout<<"vivod   "<<value<<' '<<row<<' '<<column<<endl;
       }
       else if (is_formula) {
           if (h=="sum") {
               table[{row,column}] = new cell_formula(cell_formula::SUM, { row1[0]-1, col1[0]-1 }, { row1[1]-1, col1[1]-1 });
               abstr_cell* cell=table[{row,column}];
               float value = static_cast<cell_formula*>(cell)->calc_value();
               QString inter;
               inter.setNum(value);
               ui->table_ui->setItem(row,column,new QTableWidgetItem(inter));
           }
               else if (h=="mul") {
               table[{row,column}] = new cell_formula(cell_formula::MUL, { row1[0]-1, col1[0]-1 }, { row1[1]-1, col1[1]-1 });
               abstr_cell* cell=table[{row,column}];
               float value = static_cast<cell_formula*>(cell)->calc_value();
               QString inter;
               inter.setNum(value);
               ui->table_ui->setItem(row,column,new QTableWidgetItem(inter));
           }
               else if (h=="avg") {
               table[{row,column}] = new cell_formula(cell_formula::AVG, { row1[0]-1, col1[0]-1 }, { row1[1]-1, col1[1]-1 });
               abstr_cell* cell=table[{row,column}];
               float value = static_cast<cell_formula*>(cell)->calc_value();
               QString inter;
               inter.setNum(value);
               ui->table_ui->setItem(row,column,new QTableWidgetItem(inter));
           }
               else if (s=="=err") {
                table[{row,column}] = new cell_formula(cell_formula::ERR, { row1[0]-1, col1[0]-1 }, { row1[1]-1, col1[1]-1 });
//                abstr_cell* cell=table[{row,column}];
//                float value = static_cast<cell_formula*>(cell)->calc_value();
                string str="NaN";
                QString inter = QString::fromStdString(str);
                ui->table_ui->setItem(row,column,new QTableWidgetItem(inter));
           }
       }
    }
}

