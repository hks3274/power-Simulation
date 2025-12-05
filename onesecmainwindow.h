#ifndef ONESECMAINWINDOW_H
#define ONESECMAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QGroupBox>
#include <QMenuBar>
#include "dataManagement.h"

class inputMainWindow;
class MainWindow;
class A3700N_GUI;

class oneSecMainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit oneSecMainWindow(dataManagement& dataMng, QWidget* parent = nullptr);
    ~oneSecMainWindow();

    void setMainWindow(MainWindow* mainWin);
    void setInputWindow(inputMainWindow* inputWin);
    void setA3700Window(A3700N_GUI* a3700Win);


signals:
    void oneSecChanged(oneSecList* osl);

private slots:
    void onRmsData(oneSecTableData* tableData, oneSecList* osl);

private:
    dataManagement& dataMng;

    oneSecList* oneSecData;
    oneSecTableData* tableData;

    // === 표 위젯들 ===
    QTableWidget *tableFund;
    QTableWidget *tableHarm;
    QTableWidget *tableTotal;
    QTableWidget *tablePower;
    QTableWidget *tableThd;
    QTableWidget *tableResidual;
    QTableWidget *tableSymmentic;
    QTableWidget *tableNemaUnbal;
    QTableWidget *tableUo_U2;

    inputMainWindow* inputWindow = nullptr;
    MainWindow* mainWindow = nullptr;
    A3700N_GUI* a3700Window = nullptr;

};

#endif // ONESECMAINWINDOW_H
