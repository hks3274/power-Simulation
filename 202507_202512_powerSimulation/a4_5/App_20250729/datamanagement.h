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
    explicit dataManagement(QObject *parent = nullptr);  //grapgWidget이 제거되면 자동으로 settingDial이 해제됨

    //setter
    void setInputValue(double value);
    void setTimeInterval(double interval);
    void setGraphWidth(double gWidth);
    void setInputFreq(int frequency);
    void setTimeScale(int timeScale);

    //getter
    double getInputValue() {return value;}
    double getTimeInterval() {return interval_msec;}
    double getGraphWidth() {return gWidth;}
    int getInputFreq() {return frequency;}
    int getTimeScale() {return timeScale;}

    struct data { //struct => 구조체, 여러 데이터를 묶어서 하나의 새로운 타입을 생성
        double value; //실수형 변수 value
        std::chrono::nanoseconds t; //시간관련타입
    };

    auto getData() {return data_queue;}
    QPair<QVector<double>, QVector<double>> getPlotDataInRangeSec(double rangeSec);

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
    std::chrono::nanoseconds simulationTime = std::chrono::nanoseconds(0);

    std::deque<data> data_queue;
};

#endif // DATAMANAGEMENT_H
