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
    connect(&dataMng, &dataManagement::rmsDataChanged,
            this, &frequencytracker::onRms);
}

void frequencytracker::start() {
    running = true;
    first = true;
    zcCount = 0;
    qDebug() << "[Tracker] 시작 ";
}

void frequencytracker::stop() {
    running = false;
    qDebug() << "[Tracker] 중지";
}

void frequencytracker::onRms(dataManagement::measure_data m) {
    if(!running) return;

    auto ph = dataMng.calcPhasor();
    double phaseNow = qDegreesToRadians(ph.vAPhaseDeg);

    if(first) { //첫번째에서는 위상을 구할 수 없기에 위상값을 넣고 리턴
        prevPhaseRad = phaseNow;
        first = false;
        return;
    }

    //주파수 추적 + 위상추적
    double dphi = phaseNow - prevPhaseRad;
    while (dphi >  M_PI) dphi -= 2*M_PI;
    while (dphi < -M_PI) dphi += 2*M_PI;
    prevPhaseRad = phaseNow;

    double freqNow = dataMng.getCyclePerSecond();

    const double Kp = 0.05;
    const double Kd = 0.01;

    double offsetP = dphi * Kp + phaseNow * Kd;

    double fs_new = freqNow * (1.0 + offsetP);
    emit cpsChanged(fs_new);
    dataMng.setCyclePerSecond(fs_new);


    qDebug() << "[Tracker] dphi:" << dphi << "phInt:" << phaseIntegral
             << "offset:" << offsetP << "fs:" << fs_new;
    qDebug() << "[Tracker] fs 보정 ->" << fs_new;
}
