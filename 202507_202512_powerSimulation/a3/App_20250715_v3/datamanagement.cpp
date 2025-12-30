#include "dataManagement.h"

#include <QObject>
#include <QTimer>
#include <deque>
#include <chrono>

dataManagement::dataManagement(QObject *parent)
    : QObject{parent}
{
    connect(&t, &QTimer::timeout, this, &dataManagement::captureData);
    t.setSingleShot(false); // false면 타이머 반복 작업, true면 타이머가 한 번 작동하고 멈춤
    t.start(interval_msec);
}


void dataManagement::setInputValue(float value)
{
    this->value = value;
}


void dataManagement::setTimeInterval(float interval)
{
    if(interval < 100) {
        interval = 100;
    }

    this->interval_msec = interval;
    t.setInterval(interval_msec);
}

void dataManagement::setGraphWidth(int gwidth)
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

constexpr int max_count = 300;

void dataManagement::captureData()
{
    if(data_queue.size() >= max_count){
        data_queue.pop_front();
    }

    float radians = angle * M_PI / 180.0f;
    float sinValue = value * sin(radians);

    data_queue.push_back({sinValue, std::chrono::system_clock::now()});

    emit dataChanged();
}


