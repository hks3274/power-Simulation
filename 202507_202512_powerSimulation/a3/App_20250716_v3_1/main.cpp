#include "mainwindow.h"

#include <QApplication>
#include "dataManagement.h"
#include <QDir>
#include <QFile>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QDir::addSearchPath("icon", ":/icon/theme");

    QFile file(":/file/dark_teal.qss");
    file.open(QFile::ReadOnly);

    QString styleSheet { file.readAll() };
    a.setStyleSheet(styleSheet);

    dataManagement dataMng;
    MainWindow w(dataMng);
    w.show();
    return a.exec();
}
