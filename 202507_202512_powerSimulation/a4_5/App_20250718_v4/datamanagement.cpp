#include "dataManagement.h"

#include <QObject>
#include <QTimer>
#include <deque>

dataManagement::dataManagement(QObject *parent)
    : QObject{parent}
{
    connect(&t, &QTimer::timeout, this, &dataManagement::captureData);
    t.setSingleShot(false); // false면 타이머 반복 작업, true면 타이머가 한 번 작동하고 멈춤
    t.start(interval_msec);
}


void dataManagement::setInputValue(double value)
{
    this->value = value;
}


void dataManagement::setTimeInterval(double interval)
{
    this->interval_msec = interval;
    t.setInterval(interval_msec);
}

void dataManagement::setGraphWidth(double gwidth)
{
    this->gWidth = gwidth;
}


void dataManagement::setInputFreq(int frequency)
{
    this->frequency = frequency;
}

void dataManagement::setTimeScale(int timeScale)
{
    this->timeScale = timeScale;

    //t.setInterval(interval_msec * timeScale);
}


constexpr int max_count = 1000;



void dataManagement::captureData()
{
    if(data_queue.size() >= max_count){
        data_queue.pop_front();
    }

    //시뮬레이션 시간 누적 -> timer는 captuer시간으로 냅두기..

    float angle = 360.0 * frequency * simulationTime;
    double sinValue = value * sin(angle * M_PI / 180.0);

    data_queue.push_back({sinValue, simulationTime});

    simulationTime += interval_msec / 1000.0 /timeScale;



    emit dataChanged();
}


