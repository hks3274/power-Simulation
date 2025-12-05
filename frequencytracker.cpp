#include "frequencytracker.h"
#include <QDebug>
#include <cmath>
#include <algorithm>

namespace {
inline double clampd(double v, double lo, double hi) {
    return std::min(hi, std::max(lo, v));
}
inline double wrap_pi(double x) {
    while (x >  M_PI) x -= 2.0*M_PI;
    while (x <= -M_PI) x += 2.0*M_PI;
    return x;
}
}

frequencytracker::frequencytracker(dataManagement& dataMng)
    : dataMng(dataMng)
{
    phasorCalc = new phasorCalculator(dataMng);
    connect(&dataMng, &dataManagement::rmsDataChanged,
            this, &frequencytracker::onRms);
}

void frequencytracker::start() {
    running = true;
    first = true;
    qDebug() << "[Tracker] 시작 ";
}

void frequencytracker::stop() {
    running = false;
    qDebug() << "[Tracker] 중지";
}

void frequencytracker::onRms(measure_data m) {
    if(!running) return;

    auto ph = phasorCalc->calcPhasor();
    double phaseNow = qDegreesToRadians(ph.vAPhaseDeg);

    // --- 초기화 ---
    if(first) {
        prevPhaseRad = phaseNow;
        phaseIntegral = 0.0;
        first = false;
        return;
    }

    // --- 위상 차 계산 ---
    double dphi = phaseNow - prevPhaseRad;
    while (dphi >  M_PI) dphi -= 2*M_PI;
    while (dphi < -M_PI) dphi += 2*M_PI;
    prevPhaseRad = phaseNow;

    // --- 현재 주파수 ---
    double freqNow = dataMng.getCyclePerSecond();

    // --- 제어 게인 ---
    const double Kp = 0.05;   // 비례 이득
    const double Ki = 0.02;  // 적분 이득 (아주 작게)
    const double Kd = 0.01;   // D 항 (선택사항)

    // --- 비례 + D ---
    double offsetP = dphi * Kp + phaseNow * Kd;

    // --- 적분항: 거의 수렴했을 때만 활성화 ---
    // if (std::abs(dphi) < 0.02)  // 위상오차가 0.05rad (~3°) 이하면
    //     phaseIntegral += dphi * Ki; // 누적
    // else
    phaseIntegral *= 0.95; // drift 방지를 위해 감쇠

    // --- 전체 보정 ---
    double offsetTotal = offsetP + phaseIntegral;

    // --- 새로운 주파수 계산 ---
    double fs_new = freqNow * (1.0 + offsetTotal);
    emit cpsChanged(fs_new);
    dataMng.setCyclePerSecond(fs_new);

    qDebug() << "[Tracker] dphi:" << dphi
             << "Iterm:" << phaseIntegral
             << "offsetTotal:" << offsetTotal
             << "fs_new:" << fs_new;
}
