#include "dataManagement.h"

#include <QObject>
#include <QTimer>
#include <deque>
#include <QDebug>

dataManagement::dataManagement(QObject *parent)
    : QObject{parent}
{
    connect(&t, &QTimer::timeout, this, &dataManagement::captureData);
    t.setSingleShot(false); // false면 타이머 반복 작업, true면 타이머가 한 번 작동하고 멈춤
    t.start(interval_msec * 1000);
    timer.start();
}


void dataManagement::setInputValue(float value)
{
    this->value = value;
}


void dataManagement::setTimeInterval(float interval_msec)
{
    if(interval_msec < 0.01) {
        interval_msec = 0.01;
    }

    this->interval_msec = interval_msec;
    t.setInterval(interval_msec * 1000);
}

void dataManagement::setGraphWidth(float gwidth)
{
    this->gWidth = gwidth;
}

void dataManagement::setInputAngle(int angle)
{
    this->angle = angle;
}

void dataManagement::setRpsValue(float rpsValue)
{
    this->rpsValue = rpsValue;
}

void dataManagement::setTimeScale(int timeScale)
{
    // 현재 시뮬레이션 시간을 계산해서 offset으로 저장
    qint64 currentRealTime = timer.elapsed();
    qint64 currentSimTime = timeOffset + (currentRealTime - realTimeOffset) / currentTimeScale;

    // 새로운 기준점 설정
    timeOffset = currentSimTime;
    realTimeOffset = currentRealTime;
    currentTimeScale = timeScale;

    // QTimer 간격도 조정
    this->timeScale = timeScale;
    float cycleTime_ms = 1000/ rpsValue;
    float samplig_ms = cycleTime_ms / 16;
    timeScale_ms = samplig_ms * timeScale;
    if (timeScale_ms < 1.0f)
        timeScale_ms = 1.0f;
    t.setInterval(timeScale_ms);
}

constexpr int max_count = 1000;

void dataManagement::captureData()
{
    if(data_queue.size() >= max_count){
        data_queue.pop_front();
    }

    float radians = angle * M_PI / 180.0f;
    float sinValue = value * sin(radians);

    qint64 currentRealTime = timer.elapsed();
    qint64 timer_ms = timeOffset + (currentRealTime - realTimeOffset) / currentTimeScale;

    data_queue.push_back({sinValue, timer_ms});

    emit dataChanged();
}


