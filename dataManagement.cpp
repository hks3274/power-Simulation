#include "dataManagement.h"

#include <QObject>
#include <QTimer>
#include <deque>
#include <QDebug>
#include <cmath>
using namespace std::chrono;

dataManagement::dataManagement(QObject *parent)
    : QObject{parent}
{
    t.setTimerType(Qt::PreciseTimer);
    connect(&t, &QChronoTimer::timeout, this, &dataManagement::captureData);
    t.setSingleShot(false);
    t.setInterval(interval_ns);
    t.start();
}

//3상 set
// =========== A상 =============
// A상 전압값 변경
void dataManagement::setVAValue(double vAValue)
{
    this->vAValue = vAValue;
    qDebug() << "[변경] A상 전압값 : " << vAValue;
}

// A상  전류값 변경
void dataManagement::setIAValue(double iAValue)
{
    this-> iAValue = iAValue;
    qDebug() << "[변경] A상 전류값 : " << iAValue;
}

// A상 전류 위상 값 변경
void dataManagement::setIAPhaseDeg(int iAPhaseDeg)
{
    this->iAPhaseDeg = iAPhaseDeg;
    qDebug() << "[변경] A상 전류 위상 값 : " << iAPhaseDeg;
}

//=========== B상 ============
//전압
void dataManagement::setVBValue(double vBValue)
{
    this->vBValue = vBValue;
    qDebug() << "[변경] B상 전압값 : " << vBValue;
}
//전압 위상
void dataManagement::setVBPhaseDeg(int vBPhaseDeg)
{
    this->vBPhaseDeg = vBPhaseDeg;
    qDebug() << "[변경] B상 전류 위상 값 : " << vBPhaseDeg;
}

//전류값 변경
void dataManagement::setIBValue(double iBValue)
{
    this-> iBValue = iBValue;
    qDebug() << "[변경] B상 전류값 : " << iBValue;
}

// 전류 위상 값 변경
void dataManagement::setIBPhaseDeg(int iBPhaseDeg)
{
    this->iBPhaseDeg = iBPhaseDeg;
    qDebug() << "[변경] B상 전류 위상 값 : " << iBPhaseDeg;
}

//=========== C상 ============
//전압
void dataManagement::setVCValue(double vCValue)
{
    this->vCValue = vCValue;
    qDebug() << "[변경] C상 전압값 : " << vCValue;
}
//전압 위상
void dataManagement::setVCPhaseDeg(int vCPhaseDeg)
{
    this->vCPhaseDeg = vCPhaseDeg;
    qDebug() << "[변경] C상 전류 위상 값 : " << vCPhaseDeg;
}

//전류값 변경
void dataManagement::setICValue(double iCValue)
{
    this-> iCValue = iCValue;
    qDebug() << "[변경] C상 전류값 : " << iCValue;
}

// 전류 위상 값 변경
void dataManagement::setICPhaseDeg(int iCPhaseDeg)
{
    this->iCPhaseDeg = iCPhaseDeg;
    qDebug() << "[변경] C상 전류 위상 값 : " << iCPhaseDeg;
}



// 시간간격 변경
void dataManagement::setTimeInterval(std::chrono::nanoseconds interval)
{
    this->interval_ns = interval;
    t.setInterval(interval_ns * timeScale);
    qDebug() << "[변경] 시간간격 : " << t.interval();
}

// 초당 사이클 수 변경
void dataManagement::setCyclePerSecond(double cyclePerSecond)
{
    this->cyclePerSecond = cyclePerSecond;
    qDebug() << "[변경] 초당 사이클 수 : " << cyclePerSecond;
}

// 사이클당 샘플 수 변경
void dataManagement::setSamplePerCycle(int samplePerCycle)
{
    this->samplePerCycle = samplePerCycle;
    qDebug() << "[변경] 사이클당 샘플 수 : " << samplePerCycle;
}

// 그래프폭 변경
void dataManagement::setGraphWidth(double gwidth)
{
    this->gWidth = gwidth;
    qDebug() << "[변경] 그래프폭 : " << gwidth;
}

// 주파수 값 변경
void dataManagement::setInputFreq(double newF)
{
    const double tSec = std::chrono::duration<double>(simulationTime).count();
    const double phiNow = phaseOffsetRad + 2.0 * M_PI * frequency * tSec;

    // 새 주파수에서도 현재 위상을 유지하도록 offset 재설정
    phaseOffsetRad = phiNow - 2.0 * M_PI * newF * tSec;

    auto norm = [](double x){
        const double twoPi = 2.0 * M_PI;
        x = std::fmod(x, twoPi);
        if (x < 0) x += twoPi;
        return x;
    };
    phaseOffsetRad = norm(phaseOffsetRad);

    this->frequency = newF;
    qDebug() << "[변경] 주파수 : " << newF;
}

// 시간 비율 변경
void dataManagement::setTimeScale(int timeScale)
{
    this->timeScale = timeScale;
    t.setInterval(interval_ns * timeScale);
    qDebug() << "[변경] 시간비율 : " << timeScale;
}



// 그래프 갱신 주기 값 변경
void dataManagement::setUpdateMode(enum updateMode mode)
{
    updateMode = mode;
    qDebug() << "[변경] 그래프 갱신 주기 값 : " << static_cast<int>(mode);
}

// 일시정지/시작
void dataManagement::setPaused(bool p)
{
    if (paused == p) return;
    paused = p;
    if (paused) {
        t.stop();
        qDebug() << "[타이머] 일시정지";
    } else {
        t.start();
        qDebug() << "[타이머] 재개";
    }
    emit pausedChanged(paused);
}

updateMode dataManagement::getUpdateMode() const {
    return updateMode;
}

// 고조파 입력값
void dataManagement::setVHarmonicOrder(int order) {
    vHarmonicOrder = order;
    qDebug() << "[변경] 전압 고조파 차수 =" << order;
}
void dataManagement::setVHarmonicMag(double mag) {
    vHarmonicMag = mag;
    qDebug() << "[변경] 전압 고조파 크기 =" << mag;
}
void dataManagement::setVHarmonicPhase(int phase) {
    vHarmonicPhase = phase;
    qDebug() << "[변경] 전압 고조파 위상 =" << phase;
}
void dataManagement::setCHarmonicOrder(int order) {
    cHarmonicOrder = order;
    qDebug() << "[변경] 전류 고조파 차수 =" << order;
}
void dataManagement::setCHarmonicMag(double mag) {
    cHarmonicMag = mag;
    qDebug() << "[변경] 전류 고조파 크기 =" << mag;
}
void dataManagement::setCHarmonicPhase(int phase) {
    cHarmonicPhase = phase;
    qDebug() << "[변경] 전류 고조파 위상 =" << phase;
}

// 데이터 업데이트 → 합성파 출력
void dataManagement::captureData()
{
    if (data_queue.size() >= 10000) {
        data_queue.pop_front();
    }

    // --- 공통 각 ---
    const double tSec  = std::chrono::duration<double>(simulationTime).count();
    const double base  = 2.0 * M_PI * frequency * tSec + phaseOffsetRad; // ωt + φ0

    // --- 상 위상 (전압) ---
    auto deg2rad = [](double d){ return d*M_PI/180.0; };
    const double phiA = 0.0;
    const double phiB = deg2rad(vBPhaseDeg);   // vBPhaseDeg: deg
    const double phiC = deg2rad(vCPhaseDeg);   // vCPhaseDeg: deg

    // --- 전류 '절대' 위상(도 → 라디안) ---
    const double iA_abs = deg2rad(iAPhaseDeg);
    const double iB_abs = deg2rad(iBPhaseDeg);
    const double iC_abs = deg2rad(iCPhaseDeg);

    // --- 기본파(1차) ---
    const double fundAV = vAValue * std::sin(base + phiA);
    const double fundBV = vBValue * std::sin(base + phiB);
    const double fundCV = vCValue * std::sin(base + phiC);

    const double fundAC = iAValue * std::sin(base + iA_abs);
    const double fundBC = iBValue * std::sin(base + iB_abs);
    const double fundCC = iCValue * std::sin(base + iC_abs);

    //----------기본파 선간전압----------------
    const double fundAVLL = fundAV - fundBV; // A-B
    const double fundBVLL = fundBV - fundCV; // B-C
    const double fundCVLL = fundCV - fundAV; // C-A

    // --- 고조파(차수 h). 상 위상은 h배! ---
    double vHarmA = 0.0, vHarmB = 0.0, vHarmC = 0.0;
    double cHarmA = 0.0, cHarmB = 0.0, cHarmC = 0.0;

    //====== 선간전압 고조파 ==================
    double vHarmA_LL = 0.0, vHarmB_LL = 0.0, vHarmC_LL = 0.0;

    // ===== 전압 고조파 =====
    if (vHarmonicOrder > 1 ) {
        const int vh = vHarmonicOrder;
        const double vHextra = 2.0 * M_PI * (double)vHarmonicPhase / 360.0;

        // // 상위상(h*phiX)을 제거 → 모든 상 동일한 위상
        // const double vHarmCommon = vHarmonicMag * std::sin(vh * base + phiA + vHextra);

        // 전압 고조파: h*phiX + 전역 오프셋
        vHarmA = vHarmonicMag * std::sin(vh*base + vh*phiA + vHextra);
        vHarmB = vHarmonicMag * std::sin(vh*base + vh*phiB + vHextra);
        vHarmC = vHarmonicMag * std::sin(vh*base + vh*phiC + vHextra);

        vHarmA_LL = vHarmA - vHarmB;
        vHarmB_LL = vHarmB - vHarmC;
        vHarmC_LL = vHarmC - vHarmA;

        // vHarmA = vHarmCommon;
        // vHarmB = vHarmCommon;
        // vHarmC = vHarmCommon;
    }

    // ===== 전류 고조파 =====
    if (cHarmonicOrder > 1 ) {
        const int ch = cHarmonicOrder;
        const double cHextra = 2.0 * M_PI * (double)cHarmonicPhase / 360.0;

        // 상 회전 제거 → 모든 상 동일
        //const double cHarmCommon = cHarmonicMag * std::sin(ch * base + cHextra);

        cHarmA = cHarmonicMag * std::sin(ch*base + ch*phiA + cHextra);
        cHarmB = cHarmonicMag * std::sin(ch*base + ch*phiB + cHextra);
        cHarmC = cHarmonicMag * std::sin(ch*base + ch*phiC + cHextra);

        // cHarmA = cHarmCommon;
        // cHarmB = cHarmCommon;
        // cHarmC = cHarmCommon;
    }

    // --- 최종 합성 ---
    const double vAVal = fundAV + vHarmA;
    const double vBVal = fundBV + vHarmB;
    const double vCVal = fundCV + vHarmC;

    const double cAVal = fundAC + cHarmA;
    const double cBVal = fundBC + cHarmB;
    const double cCVal = fundCC + cHarmC;


    //===== 합성파 선간전압 =====

    const double vAValLL = fundAVLL + vHarmA_LL;
    const double vBValLL = fundBVLL + vHarmB_LL;
    const double vCValLL = fundCVLL + vHarmC_LL;


    //Residual을 위한 연산
    double residualV = vAVal + vBVal + vCVal;
    double residualI = cAVal + cBVal + cCVal;

    // 큐에 삽입
    data_queue.push_back({ vAVal, cAVal, vBVal, cBVal, vCVal, cCVal,vAValLL, vBValLL, vCValLL, simulationTime });


    rmsAccIn.fundAV = fundAV; rmsAccIn.fundBV = fundBV; rmsAccIn.fundCV = fundCV;
    rmsAccIn.fundAC = fundAC; rmsAccIn.fundBC = fundBC; rmsAccIn.fundCC = fundCC;

    rmsAccIn.vAVal = vAVal; rmsAccIn.vBVal = vBVal; rmsAccIn.vCVal = vCVal;
    rmsAccIn.cAVal = cAVal; rmsAccIn.cBVal = cBVal; rmsAccIn.cCVal = cCVal;

    rmsAccIn.harmAV = vHarmA; rmsAccIn.harmBV = vHarmB; rmsAccIn.harmCV = vHarmC;
    rmsAccIn.harmAC = cHarmA; rmsAccIn.harmBC = cHarmB; rmsAccIn.harmCC = cHarmC;

    rmsAccIn.fundAVLL = fundAVLL; rmsAccIn.fundBVLL = fundBVLL; rmsAccIn.fundCVLL = fundCVLL;
    rmsAccIn.vAValLL = vAValLL; rmsAccIn.vBValLL = vBValLL; rmsAccIn.vCValLL = vCValLL;

    rmsAccIn.harmAVLL = vHarmA_LL; rmsAccIn.harmBVLL = vHarmB_LL; rmsAccIn.harmCVLL = vHarmC_LL;

    rmsAccIn.residualV = residualV;
    rmsAccIn.residualI = residualI;

    rmsAccIn.pA = vAVal * cAVal;
    rmsAccIn.pB = vBVal * cBVal;
    rmsAccIn.pC = vCVal * cCVal;

    rmsAccum.accumulate(rmsAccIn);

    // RMS 누적
    const int N = getSamplePerCycle();
    if (rmsAccum.isFull(N)) {
        auto m = rmsAccum.compute(simulationTime, N);
        emit rmsDataChanged(m);
        rmsAccum.reset();
    }

    simulationTime += interval_ns;

    emit dataChanged();
}

