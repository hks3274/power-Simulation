#include "datamanagement.h"
#include <QDebug>

datamanagement::datamanagement(QObject *parent)
    : QObject{parent}
{
    t = new QChronoTimer(this);
    t->setTimerType(Qt::PreciseTimer);
    connect(t, &QChronoTimer::timeout, this, &datamanagement::intervalCapture);
    t->setInterval(interval);
    t->start();
}

//시간 간격 지정
void datamanagement::setInterval(nanoseconds time) {
    interval = time;
    t->setInterval(time * getTimeScale()); //타이머의 인터벌을 직접 조정
    qDebug()<< "시간간격: " <<interval;
}

//초당 사이클 수 지정 -> 동시에 인터벌 지정
void datamanagement::setCyclePerSecond(double cps) {
    cpsVal = cps;

    auto interval_ns = std::chrono::nanoseconds(
        static_cast<long long>(1e9 / (cps * getSamplePerCycle()))
        );

    setInterval(interval_ns); //인터벌 값을 조정
    qDebug() << "sampling의 초당 cycle 수 : " << cps;
}

//사이클당 샘플 수 지정 -> 동시에 인터벌 지정
void datamanagement::setSamplePerCycle(int spc) {
    spcVal = spc;

    auto interval_ns = std::chrono::nanoseconds(
        static_cast<long long>(1e9 / (getCyclePerSecond() * spc))
        );

    setInterval(interval_ns); //인터벌 값을 조정
    qDebug() << "cycle당 sample 수 : " << spc;
}

//시간 비율 지정 -> 동시에 시간 비율 조정
 void datamanagement::setTimeScale(int timeScaleVal){
    timeScale = timeScaleVal;

    t->setInterval(getInterval() * timeScaleVal); //timer의 인터벌을 직접 조정
    qDebug() << "시간비율 : " << timeScaleVal;
 }


void datamanagement::intervalCapture(){

    currentTime += interval; //누적된 시뮬레이션 시간

    double v = getVoltVal();
    double dt = std::chrono::duration<double>(currentTime).count();   // double

    double rad = getVoltDialVal() * (M_PI / 180.0); //라디안 변화

    double sinVal = v * sin(2 * M_PI * getFrequncy() * dt + rad);


    data.push_back(QPair<double, double>(sinVal, dt));

    emit dataChanged(data);
}
