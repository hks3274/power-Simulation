#ifndef ONESECMAINWINDOW_H
#define ONESECMAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QGroupBox>
#include "dataManagement.h"

class oneSecMainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit oneSecMainWindow(dataManagement& dataMng, QWidget* parent = nullptr);
    ~oneSecMainWindow();

private slots:
    void onRmsData(dataManagement::measure_data m);

private:
    dataManagement& dataMng;



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

    // === 누적 계산 변수 ===
    double sumEnergy = 0.0;
    std::chrono::nanoseconds prev_sumTime{};
    int oneSecSampleCnt = 0;

    double sumFundV2_A = 0.0, sumFundV2_B = 0.0, sumFundV2_C = 0.0;
    double sumFundI2_A = 0.0, sumFundI2_B = 0.0, sumFundI2_C = 0.0;

    double sumHarmV2 = 0.0, sumHarmI2 = 0.0;

    double sumTotalV2_A = 0.0, sumTotalV2_B = 0.0, sumTotalV2_C = 0.0;
    double sumTotalI2_A = 0.0, sumTotalI2_B = 0.0, sumTotalI2_C = 0.0;

    double sumPowerA = 0.0, sumPowerB = 0.0, sumPowerC = 0.0;

    dataManagement::phasor_data lastPhasor;  // 마지막 페이저 정보 저장용

    double sumResidualV = 0.0;
    double sumResidualI = 0.0;
};

#endif // ONESECMAINWINDOW_H
