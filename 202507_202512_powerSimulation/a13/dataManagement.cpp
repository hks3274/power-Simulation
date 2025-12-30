#include "dataManagement.h"

#include <QObject>
#include <QTimer>
#include <deque>
#include <QDebug>
#include <complex>
#include <cmath>
using namespace std::chrono;

dataManagement::dataManagement(QObject *parent)
    : QObject{parent}
{
    t.setTimerType(Qt::PreciseTimer); //더욱 정확한 TimerType
    connect(&t, &QTimer::timeout, this, &dataManagement::captureData);
    t.setSingleShot(false); // false면 타이머 반복 작업, true면 타이머가 한 번 작동하고 멈춤
    t.start(interval_msec);
}

//전압값 변경
void dataManagement::setInputValue(double value)
{
    this->value = value;
    qDebug() << "[변경] 전압값 : " << value;
}

//시간간격 변경
void dataManagement::setTimeInterval(double interval)
{
    this->interval_msec = interval;
    t.setInterval(interval_msec * timeScale);
    sumTime = 0.0;
    qDebug() << "[변경] 시간간격 : " << interval;
}

//초당 사이클 수 변경
void dataManagement::setCyclePerSecond(double cyclePerSecond)
{
    this->cyclePerSecond = cyclePerSecond;
    sumTime = 0.0;
    qDebug() << "[변경] 초당 사이클 수 : " << cyclePerSecond;
}

//사이클당 샘플 수 변경
void dataManagement::setSamplePerCycle(int samplePerCycle)
{
    this->samplePerCycle = samplePerCycle;
    sumTime = 0.0;
    qDebug() << "[변경] 사이클당 샘플 수 : " << samplePerCycle;
}


//그래프폭 변경
void dataManagement::setGraphWidth(double gwidth)
{
    this->gWidth = gwidth;
    qDebug() << "[변경] 그래프폭 : " << gwidth;
}

//주파수 값 변경
void dataManagement::setInputFreq(double newF)
{

    const double tSec = std::chrono::duration<double>(simulationTime).count();
    const double phiNow = phaseOffsetRad + 2.0 *M_PI * frequency * tSec;

    //새주파수로도 같은 위상이 되도록 offset 재설정
    phaseOffsetRad = phiNow - 2.0 * M_PI * newF * tSec;

    // 2π 정규화
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

//시간 비율 변경
void dataManagement::setTimeScale(int timeScale)
{
    this->timeScale = timeScale;

    t.setInterval(interval_msec * timeScale);
    qDebug() << "[변경] 시간비율 : " << timeScale;
}


//전류값 변경
void dataManagement::setCurrentValue(double currentValue)
{
    this-> currentValue = currentValue;
    qDebug() << "[변경] 전류값 : " << currentValue;
}

//전류 위상 값 변경
void dataManagement::setCurrentPhaseDeg(int currentPhaseDeg)
{
    this->currentPhaseDeg = currentPhaseDeg;
    qDebug() << "[변경] 전류 위상 값 : " << currentPhaseDeg;
}


//그래프 갱신 주기 값 변경
void dataManagement::setUpdateMode(enum updateMode mode)
{
    updateMode = mode;
    sumTime = 0.0;
    qDebug() << "[변경] 그래프 갱신 주기 값 : " << static_cast<int>(mode);
}

//  일시정지, 시작 버튼 클릭
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


//그래프 갱신 주기 값 전달
updateMode dataManagement::getUpdateMode() const {
    return updateMode;
}

//고조파 입력값
void dataManagement::setVHarmonicOrder(int order) {
    vHarmonicOrder = order;
    qDebug() << "[변경] 전압 고조파 차수 =" << order;
}
void dataManagement::setVHarmonicMag(int mag) {
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
void dataManagement::setCHarmonicMag(int mag) {
    cHarmonicMag = mag;
    qDebug() << "[변경] 전류 고조파 크기 =" << mag;
}
void dataManagement::setCHarmonicPhase(int phase) {
    cHarmonicPhase = phase;
    qDebug() << "[변경] 전류 고조파 위상 =" << phase;
}


//데이터 업데이트 -> 합성파 출력
void dataManagement::captureData()
{
    if(voltage_data_queue.size() >= max_count){
        voltage_data_queue.pop_front();
    }

    if(current_data_queue.size() >= max_count){
        current_data_queue.pop_front();
    }

    //시뮬레이션 시간과 위상 계산
    //기본파
    const double tSec = std::chrono::duration<double>(simulationTime).count();
    const  double phase = phaseOffsetRad + 2.0 * M_PI * frequency * tSec; //phaseOffsetRad -> 위상보정값


    //전압 고조파 계산
    const double omega = 2.0 * M_PI * frequency;
    const double vHarm =
        vHarmonicMag * std::sqrt(2.0) *
        std::sin( vHarmonicOrder * (omega * tSec + phaseOffsetRad) + 2.0 * M_PI * (double)vHarmonicPhase / 360.0 );

    //전류 고조파 계산
    const double cHarm =
        cHarmonicMag * std::sqrt(2.0) *
        std::sin( cHarmonicOrder * (omega * tSec + phaseOffsetRad) + 2.0 * M_PI * (double)cHarmonicPhase / 360.0 );


    //sin 값을 계산
    double fundV = value * std::sin(phase);   // 전압 기본파
    double fundC = currentValue * std::sin(phase - currentPhaseDeg * M_PI / 180.0); //전류 기본파
    double voltageVal   = fundV + vHarm;      //전압 합성 신호
    double currentVal   =  fundC + cHarm;      //전류 합성 신호

    //구조체 data에 맞게 큐에 넣기
    voltage_data_queue.push_back({ voltageVal, simulationTime });
    current_data_queue.push_back({ currentVal, simulationTime });
    voltage_fund_data_queue.push_back({fundV, simulationTime}); // ZC 검출을 위해 구조체 생성

    //-------------DFT--------------------
    //샘플 누적
    fundSumV += fundV * fundV;
    fundSumC += fundC * fundC;
    harmonicSumV += vHarm * vHarm;
    harmonicSumC += cHarm * cHarm;
    sumV += voltageVal * voltageVal;
    sumI += currentVal * currentVal;
    sumP += voltageVal * currentVal;
    sampleCnt += 1;

    const int N = getSamplePerCycle();
    if(N > 0 && sampleCnt >= N) { // 한 주기
        measure_data m;
        m.t = simulationTime;

        //기본파 RMS 추가 하기 (masure에 있음 _> 확인)
        m.vFundRMSVal = std::sqrt(fundSumV/sampleCnt);
        m.cFundRMSVal = std::sqrt(fundSumC/sampleCnt);
        m.vHarmonicRMSVal = std::sqrt(harmonicSumV/sampleCnt);
        m.cHarmonicRMSVal = std::sqrt(harmonicSumC/sampleCnt);
        m.voltageRMSVal = std::sqrt(sumV /sampleCnt); //계산
        m.currentRMSVal = std::sqrt(sumI /sampleCnt); //계산
        m.powerVal = sumP / sampleCnt;


        // 막 끝난 1주기 범위에서 한 번만 ZC 검사(기본파 기준) -> 합성파 이후 ZC 검출
        if (!voltage_fund_data_queue.empty()) {
            const size_t vSz   = voltage_fund_data_queue.size();
            const size_t start = (vSz >= static_cast<size_t>(N)) ? (vSz - N) : 0;

            for (size_t i = start + 1; i < vSz; ++i) {
                const double v0 = voltage_fund_data_queue[i-1].value;
                const double v1 = voltage_fund_data_queue[i].value;

                if (v0 < 0.0 && v1 >= 0.0) {
                    // 시간 보간
                    const double t0 = std::chrono::duration<double>(voltage_fund_data_queue[i-1].t).count();
                    const double t1 = std::chrono::duration<double>(voltage_fund_data_queue[i].t).count();
                    const double dv = (v1 - v0);
                    double tZC = t1; // fallback
                    if (std::fabs(dv) > 1e-12) {
                        const double frac = (-v0) / dv; // [0,1] 근방
                        tZC = t0 + std::clamp(frac, 0.0, 1.0) * (t1 - t0);
                    }
                    lastZcSec = tZC;   // “이 주기에서 탐지한 ZC 시각”을 기록
                    break;             // 한 번만 찾으면 끝
                }
            }
        }

        measures.push_back(m);
        if(measures.size() > max_measures) measures.pop_front();

        emit rmsDataChanged(m);

        resetRMSAccum();
    }

    //---------------------------------------
    //시간 누적 (interval_msec은 밀리초 단위)
    simulationTime += duration_cast<nanoseconds>(duration<double, std::milli>(interval_msec)); //duration_cast는 같은 duratuion타입만 변환 가능 그래서 우선, interval을 milli 로 duration후 타입 변환


    emit dataChanged();
}

//qcustomplot에 맞춘 데이터 가공처리 std::deque<data> -> QVector<bouble> 형태로 변경,
//why? qcustomplot의 addData(bouble, bouble) 형태로 들어가기 때문.
QPair<QPair<double, double>, double> dataManagement::getLatestVoltageAndCurrent() {
    if (voltage_data_queue.empty() || current_data_queue.empty()) return { {0.0, 0.0}, 0.0 };

    const auto& v = voltage_data_queue.back();
    const auto& c = current_data_queue.back();

    double t = duration<double>(v.t).count();
    return { {t, v.value}, c.value };
}

// RMS 값 전력값 샘플 누적값 초기화
void dataManagement::resetRMSAccum()
{
    sumV = 0.0;
    sumI = 0.0;
    sumP = 0.0;
    fundSumV = 0.0;
    fundSumC = 0.0;
    harmonicSumV = 0.0;
    harmonicSumC = 0.0;
    sampleCnt = 0;
}

static inline double normDeg(double d){
    while (d > 180.0)  d -= 360.0;
    while (d <= -180.0) d += 360.0;
    return d;
}
static inline std::complex<double> rot90(const std::complex<double>& X){
    // +90° 회전(j 곱) = (-Im, Re)
    return std::complex<double>(-X.imag(), X.real());
}


//위상 계산 -> 기본파, 고조파 분리 -> phasorWidget에서 사용
dataManagement::phasor_data dataManagement::calcPhasor() const {
    phasor_data ph{};

    const int Nset = samplePerCycle;
    if (Nset <= 0 || voltage_data_queue.empty() || current_data_queue.empty())
        return ph;

    const size_t vSz = voltage_data_queue.size();
    const size_t cSz = current_data_queue.size();
    const int N = std::min<int>({ Nset, (int)vSz, (int)cSz }); // 세개의 값 중 가장 작은 값을 사용

    const size_t vStart = vSz - N; //가장 최근의 값
    const size_t cStart = cSz - N; //가장 최근의 값

    std::complex<double> vX(0,0), cX(0,0);       // k=1 (기본파)
    std::complex<double> hvX(0,0), hcX(0,0);     // k=hV, k=hC (고조파)

    // 동일한 n(0..N-1)로 두 신호 동시 누적 → 위상차 일관성
    for (int n = 0; n < N; ++n) {
        // --- 기본파 k=1 ---
        const double th1 = -2.0 * M_PI * n / N;
        const std::complex<double> W1(std::cos(th1), std::sin(th1));
        vX += voltage_data_queue[vStart + n].value * W1;
        cX += current_data_queue[cStart + n].value * W1;

        // --- 고조파 k=hV / k=hC ---
        //나중에는 테이블을 만들어서 적용하는 것으로 최적화하기
        const double thV = -2.0 * M_PI * vHarmonicOrder * n / N;
        const double thC = -2.0 * M_PI * cHarmonicOrder * n / N;
        hvX += voltage_data_queue[vStart + n].value * std::complex<double>(std::cos(thV), std::sin(thV));
        hcX += current_data_queue[cStart + n].value * std::complex<double>(std::cos(thC), std::sin(thC));
    }

    if (N > 0) {
        // 정규화
        vX  /= (double)N;  cX  /= (double)N;
        hvX /= (double)N;  hcX /= (double)N;

        // sin 생성 → cos 기준 보정(+90°)
        const auto vZ  = rot90(vX);
        const auto cZ  = rot90(cX);
        const auto hvZ = rot90(hvX);
        const auto hcZ = rot90(hcX);

        // === 기본파 채우기 (기존 calcPhasor와 동일 스케일 유지: *sqrt(2)) ===
        ph.vReal = vZ.real();  ph.vImag = vZ.imag();
        ph.cReal = cZ.real();  ph.cImag = cZ.imag();

        ph.vMag  = std::hypot(ph.vReal, ph.vImag) * std::sqrt(2.0);
        ph.cMag  = std::hypot(ph.cReal, ph.cImag) * std::sqrt(2.0);

        ph.vPhaseDeg = normDeg(std::atan2(ph.vImag, ph.vReal) * 180.0 / M_PI);
        ph.cPhaseDeg = normDeg(std::atan2(ph.cImag, ph.cReal) * 180.0 / M_PI);

        // === 고조파 채우기 (동일 스케일) ===
        ph.hvReal = hvZ.real(); ph.hvImag = hvZ.imag();
        ph.hcReal = hcZ.real(); ph.hcImag = hcZ.imag();

        ph.hvMag  = std::hypot(ph.hvReal, ph.hvImag) * std::sqrt(2.0);
        ph.hcMag  = std::hypot(ph.hcReal, ph.hcImag) * std::sqrt(2.0);

        ph.hvPhaseDeg = normDeg(std::atan2(ph.hvImag, ph.hvReal) * 180.0 / M_PI);
        ph.hcPhaseDeg = normDeg(std::atan2(ph.hcImag, ph.hcReal) * 180.0 / M_PI);

    }


    return ph;
}
