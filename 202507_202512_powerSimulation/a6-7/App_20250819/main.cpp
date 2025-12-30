#include "mainwindow.h"

#include <QApplication>
#include "dataManagement.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    dataManagement dataMng;
    MainWindow w(dataMng);
    w.show();
    return a.exec();
}
