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
    void oneSecChanged(dataManagement::oneSecList* osl);
    void oneSecChangeSignal();

private slots:
    void onRmsData(dataManagement::measure_data m);

private:
    dataManagement& dataMng;

    dataManagement::oneSecList* oneSecData;

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

    //선간전압
    double sumFundV2_ALL = 0.0, sumFundV2_BLL = 0.0, sumFundV2_CLL = 0.0;
    double sumTotalV2_ALL = 0.0, sumTotalV2_BLL = 0.0, sumTotalV2_CLL = 0.0;
    double sumHarmAV2_LL = 0.0, sumHarmBV2_LL = 0.0, sumHarmCV2_LL = 0.0;

    double sumHarmV2 = 0.0, sumHarmI2 = 0.0;

    double sumTotalV2_A = 0.0, sumTotalV2_B = 0.0, sumTotalV2_C = 0.0;
    double sumTotalI2_A = 0.0, sumTotalI2_B = 0.0, sumTotalI2_C = 0.0;

    double sumPowerA = 0.0, sumPowerB = 0.0, sumPowerC = 0.0;

    dataManagement::phasor_data lastPhasor;  // 마지막 페이저 정보 저장용

    double sumResidualV = 0.0;
    double sumResidualI = 0.0;

    inputMainWindow* inputWindow = nullptr;
    MainWindow* mainWindow = nullptr;
    A3700N_GUI* a3700Window = nullptr;

    //===상 전압 합성 RMS MIN/MIX ===
    double vTotalAMax  = std::numeric_limits<double>::lowest();
    double vTotalBMax  = std::numeric_limits<double>::lowest();
    double vTotalCMax  = std::numeric_limits<double>::lowest();
    double vTotalAvgMax = std::numeric_limits<double>::lowest();

    QString vTotalAMaxTime;
    QString vTotalBMaxTime;
    QString vTotalCMaxTime;
    QString vTotalAvgMaxTime;

    double vTotalAMin  = std::numeric_limits<double>::max();
    double vTotalBMin  = std::numeric_limits<double>::max();
    double vTotalCMin  = std::numeric_limits<double>::max();
    double vTotalAvgMin = std::numeric_limits<double>::max();

    QString vTotalAMinTime;
    QString vTotalBMinTime;
    QString vTotalCMinTime;
    QString vTotalAvgMinTime;

    //===선간 전압 합성 RMS MIN/MIX ===
    double vTotalALLMax  = std::numeric_limits<double>::lowest();
    double vTotalBLLMax  = std::numeric_limits<double>::lowest();
    double vTotalCLLMax  = std::numeric_limits<double>::lowest();
    double vTotalAvgLLMax = std::numeric_limits<double>::lowest();

    QString vTotalALLMaxTime;
    QString vTotalBLLMaxTime;
    QString vTotalCLLMaxTime;
    QString vTotalAvgLLMaxTime;

    double vTotalALLMin  = std::numeric_limits<double>::max();
    double vTotalBLLMin  = std::numeric_limits<double>::max();
    double vTotalCLLMin  = std::numeric_limits<double>::max();
    double vTotalAvgLLMin = std::numeric_limits<double>::max();

    QString vTotalALLMinTime;
    QString vTotalBLLMinTime;
    QString vTotalCLLMinTime;
    QString vTotalAvgLLMinTime;

    //===상전압(LN) 기본파 MIN/MIX ===
    double vFundAMax  = std::numeric_limits<double>::lowest();
    double vFundBMax  = std::numeric_limits<double>::lowest();
    double vFundCMax  = std::numeric_limits<double>::lowest();
    double vFundAvgMax  = std::numeric_limits<double>::lowest();

    QString vFundAMaxTime;
    QString vFundBMaxTime;
    QString vFundCMaxTime;
    QString vFundAvgMaxTime;

    double vFundAMin  = std::numeric_limits<double>::max();
    double vFundBMin  = std::numeric_limits<double>::max();
    double vFundCMin  = std::numeric_limits<double>::max();
    double vFundAvgMin  = std::numeric_limits<double>::max();

    QString vFundAMinTime;
    QString vFundBMinTime;
    QString vFundCMinTime;
    QString vFundAvgMinTime;

    //===선간전압(LL) 기본파 MIN/MIX ===
    double vFundALLMax  = std::numeric_limits<double>::lowest();
    double vFundBLLMax  = std::numeric_limits<double>::lowest();
    double vFundCLLMax  = std::numeric_limits<double>::lowest();
    double vFundAvgLLMax  = std::numeric_limits<double>::lowest();

    QString vFundALLMaxTime;
    QString vFundBLLMaxTime;
    QString vFundCLLMaxTime;
    QString vFundAvgLLMaxTime;

    double vFundALLMin  = std::numeric_limits<double>::max();
    double vFundBLLMin  = std::numeric_limits<double>::max();
    double vFundCLLMin  = std::numeric_limits<double>::max();
    double vFundAvgLLMin  = std::numeric_limits<double>::max();

    QString vFundALLMinTime;
    QString vFundBLLMinTime;
    QString vFundCLLMinTime;
    QString vFundAvgLLMinTime;


    //===== 상전압(LN) THD MAX ========
    double thdVAMax = std::numeric_limits<double>::lowest();
    double thdVBMax = std::numeric_limits<double>::lowest();
    double thdVCMax = std::numeric_limits<double>::lowest();

    QString thdVAMaxTime;
    QString thdVBMaxTime;
    QString thdVCMaxTime;

    //===== 선간전압(LL) THD MAX ========
    double thdVALLMax = std::numeric_limits<double>::lowest();
    double thdVBLLMax = std::numeric_limits<double>::lowest();
    double thdVCLLMax = std::numeric_limits<double>::lowest();

    QString thdVALLMaxTime;
    QString thdVBLLMaxTime;
    QString thdVCLLMaxTime;

    // ==== frequency MAX/MIN ===========
    double freqMax = std::numeric_limits<double>::lowest();
    QString freqMaxTime;
    double freqMin = std::numeric_limits<double>::max();
    QString freqMinTime;

    //==== Voltage Residual MIN/MAX 값 ============
    double vResidualRMSMax = std::numeric_limits<double>::lowest();
    QString vResidualRMSMaxTime;
    double vResidualRMSMin = std::numeric_limits<double>::max();
    QString vResidualRMSMinTime;


    //====CURRENT =================
    //=== RMS Total MIN/MAX 값 =============
    double iTotalAMax = std::numeric_limits<double>::lowest();
    double iTotalBMax = std::numeric_limits<double>::lowest();
    double iTotalCMax = std::numeric_limits<double>::lowest();
    double iTotalAvgMax = std::numeric_limits<double>::lowest();

    QString iTotalAMaxTime;
    QString iTotalBMaxTime;
    QString iTotalCMaxTime;
    QString iTotalAvgMaxTime;

    double iTotalAMin = std::numeric_limits<double>::max();
    double iTotalBMin = std::numeric_limits<double>::max();
    double iTotalCMin = std::numeric_limits<double>::max();
    double iTotalAvgMin = std::numeric_limits<double>::max();

    QString iTotalAMinTime;
    QString iTotalBMinTime;
    QString iTotalCMinTime;
    QString iTotalAvgMinTime;

    //=== Fundamental MIN/MAX 값 =============
    double iFundAMax = std::numeric_limits<double>::lowest();
    double iFundBMax = std::numeric_limits<double>::lowest();
    double iFundCMax = std::numeric_limits<double>::lowest();
    double iFundAvgMax = std::numeric_limits<double>::lowest();

    QString iFundAMaxTime;
    QString iFundBMaxTime;
    QString iFundCMaxTime;
    QString iFundAvgMaxTime;

    double iFundAMin = std::numeric_limits<double>::max();
    double iFundBMin = std::numeric_limits<double>::max();
    double iFundCMin = std::numeric_limits<double>::max();
    double iFundAvgMin = std::numeric_limits<double>::max();

    QString iFundAMinTime;
    QString iFundBMinTime;
    QString iFundCMinTime;
    QString iFundAvgMinTime;

    //========THD MAX 값 ========================
    double thdIAMax = std::numeric_limits<double>::lowest();
    double thdIBMax = std::numeric_limits<double>::lowest();
    double thdICMax = std::numeric_limits<double>::lowest();

    QString thdIAMaxTime;
    QString thdIBMaxTime;
    QString thdICMaxTime;

    //========residual min/max 값 =================
    double iResidualRMSMax = std::numeric_limits<double>::lowest();
    QString iResidualRMSMaxTime;
    double iResidualRMSMin = std::numeric_limits<double>::max();
    QString iResidualRMSMinTime;

    //==================== Power =======================
    //================ Active Min/Max ==================
    double pAMax = std::numeric_limits<double>::lowest();
    double pBMax = std::numeric_limits<double>::lowest();
    double pCMax = std::numeric_limits<double>::lowest();
    double pTotalMax = std::numeric_limits<double>::lowest();

    QString pAMaxTime;
    QString pBMaxTime;
    QString pCMaxTime;
    QString pTotalMaxTime;

    double pAMin = std::numeric_limits<double>::max();
    double pBMin = std::numeric_limits<double>::max();
    double pCMin = std::numeric_limits<double>::max();
    double pTotalMin = std::numeric_limits<double>::max();

    QString pAMinTime;
    QString pBMinTime;
    QString pCMinTime;
    QString pTotalMinTime;

    //================ ReActive Min/Max ==================
    double qAMax = std::numeric_limits<double>::lowest();
    double qBMax = std::numeric_limits<double>::lowest();
    double qCMax = std::numeric_limits<double>::lowest();
    double qSumMax = std::numeric_limits<double>::lowest();

    QString qAMaxTime;
    QString qBMaxTime;
    QString qCMaxTime;
    QString qSumMaxTime;

    double qAMin = std::numeric_limits<double>::max();
    double qBMin = std::numeric_limits<double>::max();
    double qCMin = std::numeric_limits<double>::max();
    double qSumMin = std::numeric_limits<double>::max();

    QString qAMinTime;
    QString qBMinTime;
    QString qCMinTime;
    QString qSumMinTime;

    //=========Apparen(S) MIN/MAX =========================
    double sAMax = std::numeric_limits<double>::lowest();
    double sBMax = std::numeric_limits<double>::lowest();
    double sCMax = std::numeric_limits<double>::lowest();
    double sSumMax = std::numeric_limits<double>::lowest();

    QString sAMaxTime;
    QString sBMaxTime;
    QString sCMaxTime;
    QString sSumMaxTime;

    double sAMin = std::numeric_limits<double>::max();
    double sBMin = std::numeric_limits<double>::max();
    double sCMin = std::numeric_limits<double>::max();
    double sSumMin = std::numeric_limits<double>::max();

    QString sAMinTime;
    QString sBMinTime;
    QString sCMinTime;
    QString sSumMinTime;

    //======= PF MIN/MAX =====================================
    double pfAMax = std::numeric_limits<double>::lowest();
    double pfBMax = std::numeric_limits<double>::lowest();
    double pfCMax = std::numeric_limits<double>::lowest();
    double pfSumMax = std::numeric_limits<double>::lowest();

    QString pfAMaxTime;
    QString pfBMaxTime;
    QString pfCMaxTime;
    QString pfSumMaxTime;

    double pfAMin = std::numeric_limits<double>::max();
    double pfBMin = std::numeric_limits<double>::max();
    double pfCMin = std::numeric_limits<double>::max();
    double pfSumMin = std::numeric_limits<double>::max();

    QString pfAMinTime;
    QString pfBMinTime;
    QString pfCMinTime;
    QString pfSumMinTime;

    //============= Analysis ===============================
    //=========== LN voltage symmetrical========================
    double Vpos_magMax = std::numeric_limits<double>::lowest();
    double Vneg_magMax = std::numeric_limits<double>::lowest();
    double Vzero_magMax = std::numeric_limits<double>::lowest();

    QString Vpos_magMaxTime;
    QString Vneg_magMaxTime;
    QString Vzero_magMaxTime;

    //=========== LL voltage Symmetrical ========================
    double Vpos_mag_LLMax = std::numeric_limits<double>::lowest();
    double Vneg_mag_LLMax = std::numeric_limits<double>::lowest();
    double Vzero_mag_LLMax = std::numeric_limits<double>::lowest();

    QString Vpos_mag_LLMaxTime;
    QString Vneg_mag_LLMaxTime;
    QString Vzero_mag_LLMaxTime;

    // ========== currnet symmetrical ===========================
    double Ipos_magMax = std::numeric_limits<double>::lowest();
    double Ineg_magMax = std::numeric_limits<double>::lowest();
    double Izero_magMax = std::numeric_limits<double>::lowest();

    QString Ipos_magMaxTime;
    QString Ineg_magMaxTime;
    QString Izero_magMaxTime;

    //========== voltage unbalance ==============================
    double vLLUnbalanceMax = std::numeric_limits<double>::lowest();
    double vUnbalanceMax = std::numeric_limits<double>::lowest();
    double vU2Max = std::numeric_limits<double>::lowest();
    double vU0Max = std::numeric_limits<double>::lowest();

    QString vLLUnbalanceMaxTime;
    QString vUnbalanceMaxTime;
    QString vU2MaxTime;
    QString vU0MaxTime;

    // =========== current Unbalance =============================
    double iUnbalanceMax = std::numeric_limits<double>::lowest();
    double iU2Max = std::numeric_limits<double>::lowest();
    double iU0Max = std::numeric_limits<double>::lowest();

    QString iUnbalanceMaxTime;
    QString iU2MaxTime;
    QString iU0MaxTime;


    void updateMax(double value, double& valueMax, QString nowTime, QString& valueMaxTime);
    void updateMin(double value, double& valueMin, QString nowTime, QString& valueMinTime);

};

#endif // ONESECMAINWINDOW_H
