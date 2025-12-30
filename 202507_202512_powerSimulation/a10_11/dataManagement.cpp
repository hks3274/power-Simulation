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

    resetRMSAccum();
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

    resetRMSAccum();
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

    // resetRMSAccum();
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


//데이터 업데이트
void dataManagement::captureData()
{
    if(voltage_data_queue.size() >= max_count){
        voltage_data_queue.pop_front();
    }

    if(current_data_queue.size() >= max_count){
        current_data_queue.pop_front();
    }

    //시뮬레이션 시간과 위상 계산
    const double tSec = std::chrono::duration<double>(simulationTime).count();
    const  double phase = phaseOffsetRad + 2.0 * M_PI * frequency * tSec; //phaseOffsetRad -> 위상보정값


    //sin 값을 계산
    double voltageVal = value * std::sin(phase);
    double currentVal = currentValue * std::sin(phase - currentPhaseDeg * M_PI / 180.0);

    //구조체 data에 맞게 큐에 넣기
    voltage_data_queue.push_back({ voltageVal, simulationTime });
    current_data_queue.push_back({ currentVal, simulationTime });



    //DFT
    //---------------------------------
    //샘플 누적
    sumV += voltageVal * voltageVal;
    sumI += currentVal * currentVal;
    sumP += voltageVal * currentVal;
    sampleCnt += 1;

    const int N = getSamplePerCycle();
    if(N > 0 && sampleCnt >= N) {
        measure_data m;
        m.t = simulationTime;
        m.voltageRMSVal = std::sqrt(sumV /sampleCnt); //계산
        m.currentRMSVal = std::sqrt(sumI /sampleCnt); //계산
        m.powerVal = sumP / sampleCnt;

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
    sampleCnt = 0;
}

dataManagement::phasor_data dataManagement::calcPhasor() const {
    phasor_data ph{};

    const int Nset = samplePerCycle;
    if (Nset <= 0 || voltage_data_queue.empty() || current_data_queue.empty())
        return ph;

    const size_t vSz = voltage_data_queue.size();
    const size_t cSz = current_data_queue.size();
    const int N = std::min<int>({ Nset, (int)vSz, (int)cSz }); // 세개의 값 중 가장 작은 값을 사용

    const double Fs   = cyclePerSecond * samplePerCycle; // 샘플링 주파수
    const double freq = frequency;
    const int k = (int)std::round(N * freq / Fs);        // 분석 bin

    const size_t vStart = vSz - N; //가장 최근의 값
    const size_t cStart = cSz - N; //가장 최근의 값

    std::complex<double> vX(0.0, 0.0), cX(0.0, 0.0);

    // 동일한 n(0..N-1)로 두 신호 동시 누적 → 위상차 일관성
    for (int n = 0; n < N; ++n) {
        const double theta = -2.0 * M_PI * k * n / N;
        const std::complex<double> Wn(std::cos(theta), std::sin(theta));
        vX += voltage_data_queue[vStart + n].value * Wn;
        cX +=  current_data_queue[cStart + n].value * Wn;
    }

    if (N > 0) {
        vX /= (double)N;
        cX /= (double)N;

        // sin 생성 → cos 기준 보정(+90° 회전: j 곱)
        const std::complex<double> vZ(-vX.imag(), vX.real());
        const std::complex<double> cZ(-cX.imag(), cX.real());

        ph.vReal = vZ.real();  ph.vImag = vZ.imag();
        ph.cReal = cZ.real();  ph.cImag = cZ.imag();

        // 크기/위상 추가
        ph.vMag = std::hypot(ph.vReal, ph.vImag);
        ph.cMag = std::hypot(ph.cReal, ph.cImag);

        ph.vPhaseDeg = std::atan2(ph.vImag, ph.vReal) * 180.0 / M_PI;
        ph.cPhaseDeg = std::atan2(ph.cImag, ph.cReal) * 180.0 / M_PI;


        // 위상 범위 정규화: (-180, 180]
        auto norm = [](double d){
            while (d > 180.0)  d -= 360.0;
            while (d <= -180.0) d += 360.0;
            return d;
        };

        ph.vPhaseDeg = norm(ph.vPhaseDeg);
        ph.cPhaseDeg = norm(ph.cPhaseDeg);
    }

    return ph;
}
