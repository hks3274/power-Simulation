#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "input_widget.h"
#include "output_widget.h"
#include "datamanagement.h"
#include <QMainWindow>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(datamanagement& dataMng);
    ~MainWindow();
private:
    datamanagement& dataMng;

    input_widget* inputWidget;
    output_widget* outputWidget;

};
#endif // MAINWINDOW_H
