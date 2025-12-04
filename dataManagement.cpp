#include "dataManagement.h"

#include <QObject>
#include <QTimer>
#include <deque>
#include <QDebug>
#include <complex>
#include <cmath>
#include <algorithm> // std::clamp
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
    if (data_queue.size() >= max_count) {
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


    //누적 RMS용 변수에 제곱합
    sumResidualV += residualV * residualV;
    sumResidualI += residualI * residualI;


    // 큐에 삽입
    data_queue.push_back({ vAVal, cAVal, vBVal, cBVal, vCVal, cCVal,vAValLL, vBValLL, vCValLL, simulationTime });

    // RMS 누적
    // --- A상 누적 ---
    fundSumAV += fundAV * fundAV;
    fundSumAC += fundAC * fundAC;
    harmSumAV += vHarmA   * vHarmA;
    harmSumAC += cHarmA   * cHarmA;
    sumAV     += vAVal   * vAVal;
    sumAI     += cAVal   * cAVal;
    sumP_A    += vAVal   * cAVal;


    // --- B상 누적 ---
    fundSumBV += fundBV * fundBV;
    fundSumBC += fundBC * fundBC;
    harmSumBV += vHarmB   * vHarmB;
    harmSumBC += cHarmB   * cHarmB;
    sumBV     += vBVal  * vBVal;
    sumBI     += cBVal  * cBVal;
    sumP_B    += vBVal  * cBVal;

    // --- C상 누적 ---
    fundSumCV += fundCV * fundCV;
    fundSumCC += fundCC * fundCC;
    sumCV     += vCVal  * vCVal;
    sumCI     += cCVal  * cCVal;
    sumP_C    += vCVal  * cCVal;

    //===== 선간전압 누적 =======
    fundSumAVLL += fundAVLL * fundAVLL;
    fundSumBVLL += fundBVLL * fundBVLL;
    fundSumCVLL += fundCVLL * fundCVLL;
    sumAVLL += vAValLL * vAValLL;
    sumBVLL += vBValLL * vBValLL;
    sumCVLL += vCValLL * vCValLL;
    harmSumAVLL += vHarmA_LL * vHarmA_LL;
    harmSumBVLL += vHarmB_LL * vHarmB_LL;
    harmSumCVLL += vHarmC_LL * vHarmC_LL;


    sampleCnt += 1;

    const int N = getSamplePerCycle();
    if (N > 0 && sampleCnt >= N) { // 한 주기(또는 그에 준하는 창)마다
        measure_data m;
        m.t = simulationTime;

        // 상별
        m.vFundA = std::sqrt(fundSumAV / sampleCnt);
        m.vFundB = std::sqrt(fundSumBV / sampleCnt);
        m.vFundC = std::sqrt(fundSumCV / sampleCnt);

        m.iFundA = std::sqrt(fundSumAC / sampleCnt);
        m.iFundB = std::sqrt(fundSumBC / sampleCnt);
        m.iFundC = std::sqrt(fundSumCC / sampleCnt);

        m.vRmsA  = std::sqrt(sumAV / sampleCnt);
        m.vRmsB  = std::sqrt(sumBV / sampleCnt);
        m.vRmsC  = std::sqrt(sumCV / sampleCnt);

        m.iRmsA  = std::sqrt(sumAI / sampleCnt);
        m.iRmsB  = std::sqrt(sumBI / sampleCnt);
        m.iRmsC  = std::sqrt(sumCI / sampleCnt);

        m.vHarmA = std::sqrt(harmSumAV/ sampleCnt);
        m.iHarmA = std::sqrt(harmSumAC / sampleCnt);
        m.vHarmB = std::sqrt(harmSumBV/ sampleCnt);
        m.iHarmB = std::sqrt(harmSumBC / sampleCnt);

        //============== 선간전압 ===================
        m.vFundALL = std::sqrt(fundSumAVLL/sampleCnt);
        m.vFundBLL = std::sqrt(fundSumBVLL/sampleCnt);
        m.vFundCLL = std::sqrt(fundSumCVLL/sampleCnt);
        m.vRmsALL = std::sqrt(sumAVLL/sampleCnt);
        m.vRmsBLL = std::sqrt(sumBVLL/sampleCnt);
        m.vRmsCLL = std::sqrt(sumCVLL/sampleCnt);
        m.vHarmALL = std::sqrt(harmSumAVLL/sampleCnt);
        m.vHarmBLL = std::sqrt(harmSumBVLL/sampleCnt);
        m.vHarmCLL = std::sqrt(harmSumCVLL/sampleCnt);

        // 전력
        double P_A = sumP_A / sampleCnt;
        double P_B = sumP_B / sampleCnt;
        double P_C = sumP_C / sampleCnt;
        double P_total = P_A + P_B + P_C;

        m.pA = P_A;  m.pB = P_B; m.pC = P_C;
        m.pTotal = P_total;

        m.residualSumV = sumResidualV;
        m.residualSumI = sumResidualI;

        measures.push_back(m);
        if (measures.size() > max_measures) measures.pop_front();
        emit rmsDataChanged(m);
        resetRMSAccum();
    }


    simulationTime += interval_ns;

    emit dataChanged();
}

dataManagement::data dataManagement::getLatestVoltageAndCurrent() {
    if (data_queue.empty()) return {0,0,0,0,0,0,0,0,0, std::chrono::nanoseconds{0}};
    const auto& s = data_queue.back();
    return {s.vAVal, s.cAVal, s.vBVal, s.cBVal, s.vCVal, s.cCVal, s.vAValLL, s.vBValLL, s.vCValLL, s.t}; // 변환 제거
}

void dataManagement::resetRMSAccum()
{
    sumAV=sumAI=fundSumAV=fundSumAC=0;
    sumBV=sumBI=fundSumBV=fundSumBC=0;
    sumCV=sumCI=fundSumCV=fundSumCC=0;
    harmSumAV=harmSumAC=0;
    //harmSumBV=harmSumBC=0;
    sumP_A=sumP_B=sumP_C=0;
    sampleCnt = 0;

    fundSumAVLL=fundSumBVLL=fundSumCVLL= 0;
    sumAVLL=sumBVLL=sumCVLL=0;
    harmSumAVLL=harmSumBVLL=harmSumCVLL =0;

    sumResidualV = sumResidualI = 0.0;
}

static inline double normDeg(double d){
    while (d > 180.0)  d -= 360.0;
    while (d <= -180.0) d += 360.0;
    return d;
}
static inline std::complex<double> rot90(const std::complex<double>& X){
    return std::complex<double>(-X.imag(), X.real());
}

// 위상/크기 계산
dataManagement::phasor_data dataManagement::calcPhasor() const {
    phasor_data ph{};

    const int Nset = samplePerCycle;
    if (Nset <= 0 || data_queue.empty())
        return ph;

    const size_t Sz = data_queue.size();
    const int N = std::min<int>({ Nset, (int)Sz });
    const size_t start = Sz - N;

    // --- 기본파(ABC) 누적기 ---
    std::complex<double> vAX(0,0), vBX(0,0), vCX(0,0);
    std::complex<double> cAX(0,0), cBX(0,0), cCX(0,0);
    std::complex<double> vALLX(0,0), vBLLX(0,0), vCLLX(0,0);//선간전압

    // --- 고조파(‘A상만’) 누적기 ---
    std::complex<double> hvX(0,0), hcX(0,0);

    for (int n = 0; n < N; n++) { // 한주기만큼 값을 누적
        const double th1 = -2.0 * M_PI * n / N;
        const std::complex<double> W1(std::cos(th1), std::sin(th1));

        const auto& s = data_queue[start + n];

        // 기본파: 3상 동시 누적 → 상간 위상 일관성 유지
        vAX += s.vAVal * W1;  vBX += s.vBVal * W1;  vCX += s.vCVal * W1;
        cAX += s.cAVal * W1;  cBX += s.cBVal * W1;  cCX += s.cCVal * W1;
        vALLX +=s.vAValLL * W1; vBLLX += s.vBValLL * W1; vCLLX +=s.vCValLL * W1;//선간전압

        // 고조파: ‘한 상만(A상)’ 누적
        const double thV = -2.0 * M_PI * vHarmonicOrder * n / N;
        const double thC = -2.0 * M_PI * cHarmonicOrder * n / N;
        if (vHarmonicMag > 1e-9){
            hvX += s.vAVal * std::complex<double>(std::cos(thV), std::sin(thV));
        }
        if (cHarmonicMag > 1e-9){
            hcX += s.cAVal * std::complex<double>(std::cos(thC), std::sin(thC));
        }
    }

    const double iN = 1.0 / (double)N;
    vAX *= iN; vBX *= iN; vCX *= iN;
    cAX *= iN; cBX *= iN; cCX *= iN;
    vALLX *= iN; vBLLX *= iN; vCLLX *= iN;//선간전압
    hvX *= iN; hcX *= iN;

    auto rz  = [&](const std::complex<double>& z){ return rot90(z); };
    auto mag = [](double x, double y){ return std::hypot(x,y) * std::sqrt(2.0); };
    auto ang = [&](double y, double x){ return normDeg(std::atan2(y, x) * 180.0 / M_PI); };

    // rot90 보정
    const auto vAZ = rz(vAX), vBZ = rz(vBX), vCZ = rz(vCX);
    const auto cAZ = rz(cAX), cBZ = rz(cBX), cCZ = rz(cCX);
    const auto hvZ = rz(hvX), hcZ = rz(hcX);
    const auto vALLZ = rz(vALLX), vBLLZ = rz(vBLLX),vCLLZ = rz(vCLLX); //선간전압

    // ===== 기본파 Voltage =====
    ph.vAReal = vAZ.real(); ph.vAImag = vAZ.imag();
    ph.vBReal = vBZ.real(); ph.vBImag = vBZ.imag();
    ph.vCReal = vCZ.real(); ph.vCImag = vCZ.imag();
    ph.vAMag  = mag(ph.vAReal, ph.vAImag); ph.vAPhaseDeg = ang(ph.vAImag, ph.vAReal);
    ph.vBMag  = mag(ph.vBReal, ph.vBImag); ph.vBPhaseDeg = ang(ph.vBImag, ph.vBReal);
    ph.vCMag  = mag(ph.vCReal, ph.vCImag); ph.vCPhaseDeg = ang(ph.vCImag, ph.vCReal);

    //===== 선간전압 Voltage =======
    ph.vALLReal = vALLZ.real(); ph.vALLImag = vALLZ.imag();
    ph.vBLLReal = vBLLZ.real(); ph.vBLLImag = vBLLZ.imag();
    ph.vCLLReal = vCLLZ.real(); ph.vCLLImag = vCLLZ.imag();
    ph.vALLMag =  mag(ph.vALLReal, ph.vALLImag); ph.vALLPhaseDeg = ang(ph.vALLImag, ph.vALLReal);
    ph.vBLLMag =  mag(ph.vBLLReal, ph.vBLLImag); ph.vBLLPhaseDeg = ang(ph.vBLLImag, ph.vBLLReal);
    ph.vCLLMag =  mag(ph.vCLLReal, ph.vCLLImag); ph.vCLLPhaseDeg = ang(ph.vCLLImag, ph.vCLLReal);

    // ===== 기본파 Current =====
    ph.cAReal = cAZ.real(); ph.cAImag = cAZ.imag();
    ph.cBReal = cBZ.real(); ph.cBImag = cBZ.imag();
    ph.cCReal = cCZ.real(); ph.cCImag = cCZ.imag();
    ph.cAMag  = mag(ph.cAReal, ph.cAImag); ph.cAPhaseDeg = ang(ph.cAImag, ph.cAReal);
    ph.cBMag  = mag(ph.cBReal, ph.cBImag); ph.cBPhaseDeg = ang(ph.cBImag, ph.cBReal);
    ph.cCMag  = mag(ph.cCReal, ph.cCImag); ph.cCPhaseDeg = ang(ph.cCImag, ph.cCReal);

    // ===== 고조파: ‘A상’만 =====
    ph.hvReal = hvZ.real(); ph.hvImag = hvZ.imag();
    ph.hcReal = hcZ.real(); ph.hcImag = hcZ.imag();
    ph.hvMag  = mag(ph.hvReal, ph.hvImag);
    ph.hvPhaseDeg = ang(ph.hvImag, ph.hvReal);
    ph.hcMag  = mag(ph.hcReal, ph.hcImag); ph.hcPhaseDeg = ang(ph.hcImag, ph.hcReal);
    ph.vOrderUsed = vHarmonicOrder;
    ph.cOrderUsed = cHarmonicOrder;

    return ph;
}

