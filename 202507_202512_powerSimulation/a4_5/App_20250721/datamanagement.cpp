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
    qDebug() << "[변경] 시간간격 : " << interval;
}

//그래프폭 변경
void dataManagement::setGraphWidth(double gwidth)
{
    this->gWidth = gwidth;
    qDebug() << "[변경] 그래프폭 : " << gwidth;
}

//주파수 값 변경
void dataManagement::setInputFreq(int frequency)
{
    this->frequency = frequency;
    qDebug() << "[변경] 주파수 : " << frequency;
}

//시간 비율 변경
void dataManagement::setTimeScale(int timeScale)
{
    this->timeScale = timeScale;

    t.setInterval(interval_msec * timeScale);
    qDebug() << "[변경] 시간비율 : " << timeScale;
}


int max_count = 1000;


//데이터 업데이트
void dataManagement::captureData()
{
    if(data_queue.size() >= max_count){
        data_queue.pop_front();
    }

    //시뮬레이션 시간 누적

    float angle = 360.0 * frequency * simulationTime;
    double sinValue = value * sin(angle * M_PI / 180.0);

    data_queue.push_back({sinValue, simulationTime});

    simulationTime += interval_msec / 1000.0;



    emit dataChanged();
}


