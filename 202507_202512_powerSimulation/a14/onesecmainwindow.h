#ifndef ONESECMAINWINDOW_H
#define ONESECMAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include "dataManagement.h"

class oneSecMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit oneSecMainWindow(dataManagement& dataMng, QWidget* parent = nullptr);
    ~oneSecMainWindow();

private slots:
    void onRmsData(dataManagement::measure_data m);

private:
    dataManagement& dataMng;

    // 누적 변수
    int oneSecSampleCnt = 0;
    std::chrono::nanoseconds prev_sumTime = std::chrono::nanoseconds(0);
    double sumFundV2 = 0.0, sumFundI2 = 0.0;
    double sumHarmV2 = 0.0, sumHarmI2 = 0.0;
    double sumTotalV2 = 0.0, sumTotalI2 = 0.0;
    double sumEnergy = 0.0, sumPower = 0.0;

    dataManagement::phasor_data lastPhasor;

    // 제어 UI
    QLabel* statusLabel;
    QPushButton* startBtn;
    QPushButton* stopBtn;

    // RMS 값 표시 라벨들
    QLabel* rmsBasicV;      // 기본파 전압
    QLabel* rmsBasicI;      // 기본파 전류
    QLabel* rmsHarmonicV;   // 고조파 전압
    QLabel* rmsHarmonicI;   // 고조파 전류
    QLabel* rmsTotalV;      // 합성파 전압
    QLabel* rmsTotalI;      // 합성파 전류

    // Phasor 값 표시 라벨들
    QLabel* phasorBasicV;     // 기본파 전압 위상
    QLabel* phasorBasicI;     // 기본파 전류 위상
    QLabel* phasorHarmonicV;  // 고조파 전압 위상
    QLabel* phasorHarmonicI;  // 고조파 전류 위상

    // 전력 값 표시 라벨들
    QLabel* avgPower;       // 평균 전력
    QLabel* powerEnergy;    // 전력량


    // 데이터 업데이트 메소드들
    void updateRMSValues(double basicV, double basicI, double harmonicV,
                         double harmonicI, double totalV, double totalI);
    void updatePhasorValues(double basicVPhase, double basicIPhase,
                            double harmonicVPhase, double harmonicIPhase);
    void updatePowerValues(double avgPowerVal, double energyVal);
};

#endif // ONESECMAINWINDOW_H
