#include "frequencytracker.h"
#include <QDebug>

frequencytracker::frequencytracker(dataManagement& dataMng)
    : dataMng(dataMng)
{
    connect(&dataMng, &dataManagement::rmsDataChanged,
            this, &frequencytracker::onRms);
}

void frequencytracker::start() {
    running = true;
    first = true;
    qDebug() << "[Tracker] 시작 ";
}


void frequencytracker::stop() {
    running =false;

    qDebug() << "[Tracker] 중지";
}


void frequencytracker::onRms(dataManagement::measure_data) {
    if(!running) return;

    auto ph =dataMng.calcPhasor();
    double phaseNow = qDegreesToRadians(ph.vPhaseDeg);

    if(first) { //첫번째의 경우에는 위상차를 구하지 말고 이전의 위상에 현재값을 넣고 리턴.
        prevPhaseRad = phaseNow;
        first = false;
        return;
    }

    double dphi = phaseNow - prevPhaseRad; //두번째부터 위상차를 구한다.
    if (dphi >  M_PI) dphi -= 2*M_PI;
    if (dphi < -M_PI) dphi += 2*M_PI;
    prevPhaseRad += dphi;

    // 창의 길이 = 1/ CPS
    double T_win = 1.0 / dataMng.getCyclePerSecond();
    double w_drift = dphi / T_win; // 위상차 / fs
    double df = w_drift / (2 * M_PI);

    // 샘플링 주파수 보정 (게인 적용)
    double fs_current = dataMng.getCyclePerSecond();
    const double alpha = 0.3;  // 보정 게인 (0.1~0.5 권장)
    double fs_new = fs_current + alpha * df;

    emit cpsChanged(fs_new);              // ★ UI에 알림 (트래커만 쏜다)

    // 소수점 한 자리로 반올림
    dataMng.setCyclePerSecond(fs_new);
    qDebug() << "[Tracker] fs 보정 ->" << fs_new;

}
