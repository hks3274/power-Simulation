#include "mainwindow.h"

#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include "dataManagement.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    dataManagement dataMng;
    MainWindow w(dataMng);
    w.show();
    return app.exec();
}
