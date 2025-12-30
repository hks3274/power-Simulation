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
    void setCyclePerSecond(int cyclePerSecond);
    void setSamplePerCycle(int samplePerCycle);
    void setGraphWidth(double gWidth);
    void setInputFreq(int frequency);
    void setTimeScale(int timeScale);
    void setCurrentValue(double currentValue);
    void setCurrentPhaseDeg (int currentPhaseDeg);
    void setUpdateMode(updateMode mode);
    void setPaused(bool p);

    //getter
    double getInputValue() {return value;}
    double getTimeInterval() {return interval_msec;}
    int getCyclePerSecond() { return cyclePerSecond; }
    int getSamplePerCycle() { return samplePerCycle; }
    double getGraphWidth() {return gWidth;}
    int getInputFreq() {return frequency;}
    int getTimeScale() {return timeScale;}
    double getCurrentValue() {return currentValue;}
    int getCurrentPhaseDeg() {return currentPhaseDeg;}
    bool isPaused() const { return paused; }
    updateMode getUpdateMode() const;


    //샘플 구조체
    struct data { //struct => 구조체, 여러 데이터를 묶어서 하나의 새로운 타입을 생성
        double value; //실수형 변수 value
        std::chrono::nanoseconds t; //시간관련타입
    };


    //전압, 전류, 전력 구조체 생성
    struct measure_data {
        std::chrono::nanoseconds t;
        double voltageRMSVal;
        double currentRMSVal;
        double powerVal;
    };

    void resetRMSAccum();

    //auto getVoltageData() {return voltage_data_queue;}
    QPair<QPair<double, double>, double> getLatestVoltageAndCurrent();
    double sumTime = 0.0; //누적시간

    std::deque<measure_data> measures;

signals:
    void dataChanged();
    void rmsDataChanged(measure_data  m);
    void pausedChanged(bool paused);  // 일시정지, 시작 토글 신호

private slots:
    void captureData();

private:
    QTimer t;
    double value = 100; //전압값
    double interval_msec = 1000.0; //업데이트 주기
    int cyclePerSecond = 20;
    int samplePerCycle = 20;
    double gWidth = 1.0; //그래프폭
    int frequency = 60; //주파수
    int timeScale = 5; //시간비율
    double currentValue = 100.0; //전류값
    int currentPhaseDeg = 60.0; //전류위상값

    updateMode updateMode = updateMode::OneSample; //그래프 갱신주기

    //전압Rms, 전류Rms 전력 값 누적
    double sumV = 0.0; //전압 누적값
    double sumI = 0.0; //전류 누적값
    double sumP = 0.0; //전력 누적값
    int sampleCnt = 0; //누적 샘플 수


    size_t max_measures = 1024;


    //그래프 저장값
    std::chrono::nanoseconds simulationTime = std::chrono::nanoseconds(0);
    std::deque<data> voltage_data_queue; //전압 데이터
    std::deque<data> current_data_queue; //전류 데이터
    const size_t max_count = 10000;

    bool paused = false;              // 일시정지, 시작 확인
};

#endif // DATAMANAGEMENT_H
