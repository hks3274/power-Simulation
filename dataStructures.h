#ifndef DATASTRUCTURES_H
#define DATASTRUCTURES_H

#include <QVector>
#include <QString>
#include <chrono>

struct oneSecTableData {
    double vFundA, vFundB, vFundC;
    double iFundA, iFundB, iFundC;
    double vPhA, vPhB, vPhC;
    double iPhA, iPhB, iPhC;
    double vHarm, iHarm;
    double vHph, iHph;
    double vTotalA, vTotalB, vTotalC;
    double iTotalA, iTotalB, iTotalC;
    double pA, pB, pC, pTotal;
    double sA, sB, sC, sSum;
    double qA, qB, qC, qSum;
    double pfA, pfB, pfC, pfSum;
    double energyWh;
    double thdVA, thdVB, thdVC;
    double thdIA, thdIB, thdIC;
    double vResidualRMS;
    double iResidualRMS;
    double Vpos_mag, Vneg_mag, Vzero_mag;
    double Ipos_mag, Ineg_mag, Izero_mag;
    double vA, vB, vUnbalance;
    double iA, iB, iUnbalance;
    double vU0, vU2;
    double iU0, iU2;
};

struct VoltageSet {
    QVector<double> RMS;                // [A, B, C, Average]
    QVector<double> RMS_MAX;
    QVector<QString> RMS_MAX_Time;
    QVector<double> RMS_MIN;
    QVector<QString> RMS_MIN_Time;

    QVector<double> RMS_LL;
    QVector<double> RMS_LL_MAX;
    QVector<QString> RMS_LL_MAX_Time;
    QVector<double> RMS_LL_MIN;
    QVector<QString> RMS_LL_MIN_Time;

    QVector<double> Fundamental;
    QVector<double> Fundamental_MAX;
    QVector<QString> Fundamental_MAX_Time;
    QVector<double> Fundamental_MIN;
    QVector<QString> Fundamental_MIN_Time;

    QVector<double> Fundamental_LL;
    QVector<double> Fundamental_LL_MAX;
    QVector<QString> Fundamental_LL_MAX_Time;
    QVector<double> Fundamental_LL_MIN;
    QVector<QString> Fundamental_LL_MIN_Time;

    QVector<double> THD;                // [A, B, C]
    QVector<double> THD_MAX;
    QVector<QString> THD_MAX_Time;

    QVector<double> THDLL;
    QVector<double> THDLL_MAX;
    QVector<QString> THDLL_MAX_Time;

    double Frequency = 0.0;
    double Frequency_MAX = 0.0;
    QString Frequency_MAX_Time;
    double Frequency_MIN = 0.0;
    QString Frequency_MIN_Time;

    double Residual = 0.0;
    double Residual_MAX;
    QString Residual_MAX_Time;
    double Residual_MIN;
    QString Residual_MIN_Time;
};

struct CurrentSet {
    QVector<double> RMS;                // [A, B, C, Average]
    QVector<double> RMS_MAX;
    QVector<QString> RMS_MAX_Time;
    QVector<double> RMS_MIN;
    QVector<QString> RMS_MIN_Time;

    QVector<double> Fundamental;
    QVector<double> Fundamental_MAX;
    QVector<QString> Fundamental_MAX_Time;
    QVector<double> Fundamental_MIN;
    QVector<QString> Fundamental_MIN_Time;

    QVector<double> THD;                // [A, B, C]
    QVector<double> THD_MAX;
    QVector<QString> THD_MAX_Time;

    double Residual = 0.0;
    double Residual_MAX;
    QString Residual_MAX_Time;
    double Residual_MIN;
    QString Residual_MIN_Time;
};

struct PowerSet {
    QVector<double> active;
    QVector<double> active_MAX;
    QVector<QString> active_MAX_Time;
    QVector<double> active_MIN;
    QVector<QString> active_MIN_Time;

    QVector<double> reactive;
    QVector<double> reactive_MAX;
    QVector<QString> reactive_MAX_Time;
    QVector<double> reactive_MIN;
    QVector<QString> reactive_MIN_Time;

    QVector<double> apparent;
    QVector<double> apparent_MAX;
    QVector<QString> apparent_MAX_Time;
    QVector<double> apparent_MIN;
    QVector<QString> apparent_MIN_Time;

    QVector<double> pf;
    QVector<double> pf_MAX;
    QVector<QString> pf_MAX_Time;
    QVector<double> pf_MIN;
    QVector<QString> pf_MIN_Time;

    double energy = 0.0;
};

struct AnalysisSet {
    QVector<double> voltSymm;
    QVector<double> voltSymm_MAX;
    QVector<QString> voltSymm_MAX_Time;

    QVector<double> voltSymm_LL;
    QVector<double> voltSymm_LL_MAX;
    QVector<QString> voltSymm_LL_MAX_Time;

    QVector<double> voltUnbal;
    QVector<double> voltUnbal_MAX;
    QVector<QString> voltUnbal_MAX_Time;

    QVector<double> currsymm;
    QVector<double> currsymm_MAX;
    QVector<QString> currsymm_MAX_Time;

    QVector<double> currUnbal;
    QVector<double> currUnbal_MAX;
    QVector<QString> currUnbal_MAX_Time;
};

struct harmornicSet {
    double harmV;
    double harmA;
};

struct oneSecList {
    VoltageSet voltage;
    CurrentSet current;
    PowerSet power;
    AnalysisSet symmUnbal;
    harmornicSet harmonic;
};

//전압, 전류, 전력 구조체 생성 (전체, 기본파, 고조파 RMS + 전력)
struct measure_data {
    std::chrono::nanoseconds t;

    // 상별 RMS
    double vRmsA, vRmsB, vRmsC;
    double iRmsA, iRmsB, iRmsC;

    double vFundA, vFundB, vFundC;
    double iFundA, iFundB, iFundC;

    double vHarmA, vHarmB, vHarmC;
    double iHarmA, iHarmB, iHarmC;

    double pA, pB, pC;
    double pTotal;

    double vRmsALL, vRmsBLL, vRmsCLL;
    double vFundALL, vFundBLL, vFundCLL;
    double vHarmALL, vHarmBLL, vHarmCLL;

    double residualSumV, residualSumI;
};


// phasor 구조체
struct phasor_data {
    // ===== 기본파 Voltage (A/B/C) =====
    double vAReal = 0.0, vAImag = 0.0, vAMag = 0.0, vAPhaseDeg = 0.0;
    double vBReal = 0.0, vBImag = 0.0, vBMag = 0.0, vBPhaseDeg = 0.0;
    double vCReal = 0.0, vCImag = 0.0, vCMag = 0.0, vCPhaseDeg = 0.0;

    //===== 선간전압 voltage (A/B/C) =====
    double vALLReal = 0.0, vALLImag = 0.0, vALLMag = 0.0, vALLPhaseDeg = 0.0;
    double vBLLReal = 0.0, vBLLImag = 0.0, vBLLMag = 0.0, vBLLPhaseDeg = 0.0;
    double vCLLReal = 0.0, vCLLImag = 0.0, vCLLMag = 0.0, vCLLPhaseDeg = 0.0;

    // ===== 기본파 Current (A/B/C) =====
    double cAReal = 0.0, cAImag = 0.0, cAMag = 0.0, cAPhaseDeg = 0.0;
    double cBReal = 0.0, cBImag = 0.0, cBMag = 0.0, cBPhaseDeg = 0.0;
    double cCReal = 0.0, cCImag = 0.0, cCMag = 0.0, cCPhaseDeg = 0.0;

    // ===== 고조파(“한 상만”, A상 기준) =====
    int    vOrderUsed = 1; // 전압 고조파 차수 (예: 3,5,7...)
    double hvReal = 0.0, hvImag = 0.0, hvMag = 0.0, hvPhaseDeg = 0.0;

    int    cOrderUsed = 1; // 전류 고조파 차수
    double hcReal = 0.0, hcImag = 0.0, hcMag = 0.0, hcPhaseDeg = 0.0;
};


struct RMSAccumInput
{
    // LN 기본파
    double fundAV, fundBV, fundCV;
    double fundAC, fundBC, fundCC;

    // LN 합성
    double vAVal, vBVal, vCVal;
    double cAVal, cBVal, cCVal;

    // LN 고조파
    double harmAV, harmBV, harmCV;
    double harmAC, harmBC, harmCC;

    // LL 기본파
    double fundAVLL, fundBVLL, fundCVLL;

    // LL 합성
    double vAValLL, vBValLL, vCValLL;

    // LL 고조파
    double harmAVLL, harmBVLL, harmCVLL;

    // Residual
    double residualV, residualI;

    // Power
    double pA, pB, pC;
};
\
    //전압, 전류 값 구조체
struct data {
    double vAVal, cAVal, vBVal, cBVal, vCVal, cCVal;
    double vAValLL, vBValLL, vCValLL;
    std::chrono::nanoseconds t;
};


#endif // DATASTRUCTURES_H
