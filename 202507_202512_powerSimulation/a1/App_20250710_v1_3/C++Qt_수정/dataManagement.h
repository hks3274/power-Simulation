#ifndef DATAMANAGEMENT_H
#define DATAMANAGEMENT_H

#include <QObject>
#include <QTimer>
#include <deque>
#include <chrono>

class dataManagement: public QObject
{
    Q_OBJECT
public:
    explicit dataManagement(QObject *parent = nullptr);

    void setInputValue(float value);
    void setTimeInterval(float interval);
    void setPhaseValue(int phase);

    int getPhaseValue() {return phase;}
    float getInputValue() {return value;}
    float getTimeInterval() {return interval_msec / 1000.f;}

    struct data { //struct => 구조체, 여러 데이터를 묶어서 하나의 새로운 타입을 생성
        float value; //실수형 변수 value
        std::chrono::system_clock::time_point t; //시간관련타입
    };

    auto& getData() {return data_queue;}

signals:
    void dataChanged();

private slots:
    void captureData();

private:
    QTimer t;
    float value = 100;
    int interval_msec = 100;
    int phase = 1;

    std::deque<data> data_queue;
};

#endif // DATAMANAGEMENT_H
