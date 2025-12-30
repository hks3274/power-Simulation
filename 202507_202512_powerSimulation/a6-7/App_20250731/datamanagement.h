#ifndef DATAMANAGEMENT_H
#define DATAMANAGEMENT_H

#include <QObject>
#include <QTimer>
#include <QElapsedTimer>
#include <deque>
#include <chrono>

//그래프 갱신주기
enum class updateMode {
    OneSample,
    HalfCycle,
    FullCycle
};

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
    void setCurrentValue(double currentValue);
    void setCurrentPhaseDeg (int currentPhaseDeg);
    void setUpdateMode(updateMode mode);

    //getter
    double getInputValue() {return value;}
    double getTimeInterval() {return interval_msec;}
    double getGraphWidth() {return gWidth;}
    int getInputFreq() {return frequency;}
    int getTimeScale() {return timeScale;}
    double getCurrentValue() {return currentValue;}
    int getCurrentPhaseDeg() {return currentPhaseDeg;}
    updateMode getUpdateMode() const;

    struct data { //struct => 구조체, 여러 데이터를 묶어서 하나의 새로운 타입을 생성
        double value; //실수형 변수 value
        std::chrono::nanoseconds t; //시간관련타입
    };

    //auto getVoltageData() {return voltage_data_queue;}
    QPair<QPair<double, double>, double> getLatestVoltageAndCurrent();
    double sumTime = 0.0; //누적시간

signals:
    void dataChanged();

private slots:
    void captureData();

private:
    QTimer t;
    double value = 100; //전압값
    double interval_msec = 1000.0; //업데이트 주기
    double gWidth = 10.0; //그래프폭
    int frequency = 1; //주파수
    int timeScale = 1; //시간비율
    double currentValue = 100.0; //전류값
    int currentPhaseDeg = 60.0; //전류위상값

    updateMode updateMode = updateMode::OneSample; //그래프 갱신주기


    //그래프 저장값
    std::chrono::nanoseconds simulationTime = std::chrono::nanoseconds(0);
    std::deque<data> voltage_data_queue; //전압 데이터
    std::deque<data> current_data_queue; //전류 데이터
    const size_t max_count = 10000;
};

#endif // DATAMANAGEMENT_H
