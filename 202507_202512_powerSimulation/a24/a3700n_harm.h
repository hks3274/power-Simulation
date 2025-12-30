#ifndef A37__N_HARM_H
#define A37__N_HARM_H

#include <QWidget>
#include <QPushButton>
#include <QComboBox>
#include <QCheckBox>
#include <QLabel>
#include "dataManagement.h"
#include "qcustomplot.h"

class a3700n_harm : public QWidget
{
    Q_OBJECT
public:
    a3700n_harm(dataManagement& dataMng);

public slots:
    void setVoltCheck(bool checked);
    void setCurrCheck(bool checked);
    void updateHarmData(dataManagement::oneSecList* osl);

private:
    dataManagement& dataMng;

    void loadQSS(const QString& listName);


    QCustomPlot* customPlot;

    QCPBars* bars1;
    QCPBars* bars2;
    QCPBars* bars3;

    QPushButton* autoBtn = nullptr;
    QPushButton* plusBtn = nullptr;
    QPushButton* minusBtn = nullptr;

    QComboBox* valueCombo;

    QCheckBox* fundCheck = nullptr;
    QComboBox* viewCombo = nullptr;
    QButtonGroup* abcGroup = nullptr;
    QCheckBox* ACheck = nullptr;
    QCheckBox* BCheck = nullptr;
    QCheckBox* CCheck = nullptr;

    QLabel* thdA = nullptr;
    QLabel* thdB = nullptr;
    QLabel* thdC = nullptr;

    QLabel* fundA = nullptr;
    QLabel* fundB = nullptr;
    QLabel* fundC = nullptr;

    QLabel* unitA = nullptr;
    QLabel* unitB = nullptr;
    QLabel* unitC = nullptr;

    bool voltageCheck = true;
    bool currentCheck = false;

    dataManagement::oneSecList* lastOsl = nullptr; //마지막 값 기억

    QVector<double> datax;
    QVector<double> datay1;
    QVector<double> datay2;
    QVector<double> datay3;

    bool plusCheck = false;
    bool minusCheck = false;
    bool changeCheck = false;

    int newRangeV = 0;
    int newRangeC = 0;

    QTableWidget* infoTable;

    void updateTable(int totalItems, QVector<double> itemValues );

};

#endif // A37__N_HARM_H
