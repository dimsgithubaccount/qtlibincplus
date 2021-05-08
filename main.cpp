#include "mainwindow.h"

#include <QApplication>
#include "test.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    CTest test;
    test.test();
    return a.exec();
}
