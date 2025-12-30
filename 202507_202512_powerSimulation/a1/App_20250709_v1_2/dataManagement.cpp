#include "dataManagement.h"

#include <QObject>
#include <QTimer>
#include <deque>
#include <chrono>
#define _USE_MATH_DEFINES
#include <cmath>

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
    if(interval < 0.1) {
        interval = 0.1;
    }

    this->interval_msec = interval * 1000;
    t.setInterval(interval_msec);
}

void dataManagement::setPhaseValue(int phase)
{
    this->phase = phase;
}

constexpr int max_count = 1000;

void dataManagement::captureData()
{
    if(data_queue.size() >= max_count){
        data_queue.pop_front();
    }

    //value 가공 -> 교류 파형으로 가공
    //파형의 일반식
    //v(t) = A * sin(2πft + Φ)
    // A : 진폭(Amplitude) = 파형의 최대 전압값
    // f : 주파수 , t : 시간, Φ : 위상
    // 우선 A, Φ는 입력값으로 하고 f와 t는 임의의 값으로 지정
    static float time = 0.0f;
    int frequency = 1;
    time += this->getTimeInterval();

    float sinVal = this->getInputValue() * sin(2 * M_PI * frequency * time + phase);

    data_queue.push_back({sinVal, std::chrono::system_clock::now()});

    emit dataChanged();
}
