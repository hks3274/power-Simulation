#include "dataManagement.h"

#include <QObject>
#include <QTimer>
#include <deque>
#include <QDebug>
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
    // 1. simulationTime은 std::chrono::nanoseconds 타입
    // 2. angle 계산을 위해 초 단위로 변환
    double angle = fmod(360.0 * frequency *
                  duration_cast<duration<double>>(simulationTime).count(), 360.0); //angle값이 360도를 넘지 않도록 조정한다.
    //다시 angle을 계산할때는 nanoseconds를 초 단위로 바꿔야 하므로 duration_cast를 double로 바꾼다.

    // 3. sin 값을 계산
    double sinValue = value * sin(angle * M_PI / 180.0);

    // 4. 구조체 data에 맞게 큐에 넣기
    data_queue.push_back({ sinValue, simulationTime });

    // 5. 시간 누적 (interval_msec이 밀리초 단위라고 가정)
    simulationTime += duration_cast<nanoseconds>(duration<double, std::milli>(interval_msec)); //duration_cast는 같은 duratuion타입만 변환 가능 그래서 우선, interval을 milli 로 duration후 타입 변환


    emit dataChanged();
}


