#ifndef DATAMANAGEMENT_H
#define DATAMANAGEMENT_H

#include <QObject>
#include <QTimer>
#include <QElapsedTimer>
#include <deque>
#include <chrono>

class dataManagement: public QObject
{
    Q_OBJECT
public:
    explicit dataManagement(QObject *parent = nullptr);

    void setInputValue(double value);
    void setTimeInterval(double interval);
    void setGraphWidth(double gWidth);
    void setInputFreq(int frequency);
    void setTimeScale(int timeScale);

    double getInputValue() {return value;}
    double getTimeInterval() {return interval_msec;}
    double getGraphWidth() {return gWidth;}
    int getInputFreq() {return frequency;}
    int getTimeScale() {return timeScale;}

    struct data { //struct => 구조체, 여러 데이터를 묶어서 하나의 새로운 타입을 생성
        double value; //실수형 변수 value
        double t; //시간관련타입
    };

    auto getData() {return data_queue;}

signals:
    void dataChanged();

private slots:
    void captureData();

private:
    QTimer t;
    double value = 100;
    double interval_msec = 1000.0;
    double gWidth = 10.0;
    int frequency = 1;
    double  time = 0.0;
    int timeScale = 1;
    double simulationTime;

    std::deque<data> data_queue;
};

#endif // DATAMANAGEMENT_H
