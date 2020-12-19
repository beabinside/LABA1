#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "table.h"
#include "cell.h"
#include <QTableWidgetItem>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE
class Table;
class abstr_cell;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionload_triggered();
    void on_actionsave_triggered();
   void on_actionclear_triggered();

    void on_table_ui_cellChanged(int row, int column);

private:
    Ui::MainWindow *ui;
    Table table;
};
#endif // MAINWINDOW_H
