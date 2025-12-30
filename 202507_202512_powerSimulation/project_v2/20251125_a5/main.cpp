#include "mainwindow.h"

#include <QApplication>
#include "datamanagement.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    datamanagement dataMng;
    MainWindow w(dataMng);
    w.show();
    return a.exec();
}
