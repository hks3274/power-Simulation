#ifndef DATAMANAGEMENT_H
#define DATAMANAGEMENT_H

#include <QObject>
#include <QTimer>
#include <deque>
#include <chrono>
#include <QElapsedTimer>


class dataManagement: public QObject
{
    Q_OBJECT
public:
    explicit dataManagement(QObject *parent = nullptr);

    void setInputValue(float value);
    void setTimeInterval(float interval_msec);
    void setGraphWidth(float gWidth);
    void setInputAngle(int angle);
    void setRpsValue(float rpsValue);
    void setTimeScale(int timeScale);

    float getInputValue() {return value;}
    float getTimeInterval() {return interval_msec;}
    float getGraphWidth() {return gWidth;}
    int getInputAngle() {return angle;}
    float getRpsValue() {return rpsValue;}
    int getTimeScale() {return timeScale;}

    struct data { //struct => 구조체, 여러 데이터를 묶어서 하나의 새로운 타입을 생성
        float value; //실수형 변수 value
        qint64 t; //시간관련타입
    };

    auto getData() {return data_queue;}

signals:
    void dataChanged();

private slots:
    void captureData();

private:
    QTimer t;
    QElapsedTimer timer;

    qint64 timeOffset = 0;          // 스케일 변경 시점의 누적 시뮬레이션 시간
    qint64 realTimeOffset = 0;      // 스케일 변경 시점의 현실 시간
    int currentTimeScale = 1;       // 현재 적용 중인 timeScale


    float value = 100;
    float interval_msec = 0.1;
    float gWidth = 10.0;
    int angle = 0;
    float rpsValue = 1.0;
    int timeScale = 1;
    float timeScale_ms;

    std::deque<data> data_queue;
};

#endif // DATAMANAGEMENT_H
