#include "mainwindow.h"
#include <QApplication>
#include <string>
#include <iostream>
#include <cassert>
#include <cstdint>
#include <map>
#include <vector>
#include <cmath>
#include "table.h"
#include "cell.h"
using namespace std;
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
