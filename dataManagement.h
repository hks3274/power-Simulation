#ifndef DATAMANAGEMENT_H
#define DATAMANAGEMENT_H

#include "dataStructures.h"
#include "rmsaccum.h"
#include <QObject>
#include <QTimer>
#include <QElapsedTimer>
#include <deque>
#include <chrono>
#include <Qchronotimer>
#include <QMap>

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
    explicit dataManagement(QObject *parent = nullptr);

    //setter
    void setTimeInterval(std::chrono::nanoseconds  interval);
    void setCyclePerSecond(double cyclePerSecond);
    void setSamplePerCycle(int samplePerCycle);
    void setGraphWidth(double gWidth);
    void setInputFreq(double frequency);
    void setTimeScale(int timeScale);
    void setVHarmonicOrder(int vHarmonicOrder);
    void setVHarmonicMag(double vHarmonicMag);
    void setVHarmonicPhase(int vHarmonicPhase);
    void setCHarmonicOrder(int cHarmonicOrder);
    void setCHarmonicMag(double cHarmonicMag);
    void setCHarmonicPhase(int cHarmonicPhase);
    void setUpdateMode(updateMode mode);
    void setPaused(bool p);


    //getter
    std::chrono::nanoseconds getTimeInterval() const{return interval_ns;}
    double getCyclePerSecond() const { return cyclePerSecond; }
    int getSamplePerCycle() const { return samplePerCycle; }
    double getGraphWidth() const {return gWidth;}
    double getInputFreq() const {return frequency;}
    int getTimeScale() const {return timeScale;}
    int getVHarmonicOrder() const {return vHarmonicOrder;}
    double getVHarmonicMag() const {return vHarmonicMag;}
    int getVHarmonicPhase() const {return vHarmonicPhase;}
    int getCHarmonicOrder() const {return cHarmonicOrder;}
    double getCHarmonicMag() const {return cHarmonicMag;}
    int getCHarmonicPhase() const {return cHarmonicPhase;}
    bool isPaused() const { return paused; }
    updateMode getUpdateMode() const;


    //3상 전압.전류 set,get
    //A상
    void setVAValue(double vAValue);
    void setIAValue(double iAValue);
    void setIAPhaseDeg (int iAPhaseDeg);

    double getVAValue() const {return vAValue;}
    double getIAValue() const {return iAValue;}
    int getIAPhaseDeg() const {return iAPhaseDeg;}

    //B상
    void setVBValue(double vBValue);
    void setVBPhaseDeg (int vBPhaseDeg);
    void setIBValue(double iBValue);
    void setIBPhaseDeg (int iBPhaseDeg);

    double getVBValue() const {return vBValue;}
    int getVBPhaseDeg() const {return vBPhaseDeg;}
    double getIBValue() const {return iBValue;}
    int getIBPhaseDeg() const {return iBPhaseDeg;}

    //C상
    void setVCValue(double vCValue);
    void setVCPhaseDeg (int vCPhaseDeg);
    void setICValue(double iCValue);
    void setICPhaseDeg (int iCPhaseDeg);

    double getVCValue() const {return vCValue;}
    int getVCPhaseDeg() const {return vCPhaseDeg;}
    double getICValue() const {return iCValue;}
    int getICPhaseDeg() const {return iCPhaseDeg;}

    std::deque<data> data_queue; //전압 데이터

signals:
    void dataChanged(); //한 샘플마다 graphWidget에게 보냄
    void rmsDataChanged(measure_data  m); //한 주기마다 보냄 -> rms, oneSec 근원
    void pausedChanged(bool paused);  // 일시정지, 시작 토글 신호

private slots:
    void captureData();

private:
    QChronoTimer  t;
    std::chrono::nanoseconds interval_ns = std::chrono::nanoseconds(0);      //업데이트 주기
    std::chrono::nanoseconds simulationTime = std::chrono::nanoseconds(0); //그래프 저장값


    //Setting 관련 변수들
    double cyclePerSecond = 20.0;       //시간당 사이클 수
    int samplePerCycle = 20;            //사이클당 샘플 수
    double gWidth = 1.0;                //그래프폭
    double frequency = 60.0;            //주파수
    int timeScale = 5;                  //시간비율
    double phaseOffsetRad = 0.0;        //위상 보정항(라디안)

    //고조파 관련 변수들
    int vHarmonicOrder = 1;             //전압 고조파 차수
    double vHarmonicMag = 0;            //전압 고조파 크기
    int vHarmonicPhase = 0;             //전압 고조파 위상
    int cHarmonicOrder = 1;             //전류 고조파 차수
    double cHarmonicMag = 0;            //전류 고조파 크기
    int cHarmonicPhase = 0;             //전류 고조파 위상

    //3상 전압, 전류 입력 값 변수
    double vAValue = 100.0;              //A상 전압
    double iAValue = 20.0;              //A상 전류
    int iAPhaseDeg = 60;                 //A상 전류위상
    double vBValue = 100.0;              //B상 전압
    int vBPhaseDeg = 120;                //B상 전압위상
    double iBValue = 10.0;              //B상 전류
    int iBPhaseDeg = iAPhaseDeg + 120;   //B상 전류위상
    double vCValue = 100.0;              //C상 전압
    int vCPhaseDeg = -120;               //C상 전압위상
    double iCValue = 15.0;              //C상 전류
    int iCPhaseDeg = iAPhaseDeg - 120;   //C상 전류위상

    updateMode updateMode = updateMode::OneSample; //그래프 갱신주기
    bool paused = false;              // 일시정지, 시작 확인

    RMSAccum rmsAccum;
    RMSAccumInput rmsAccIn;

};

#endif // DATAMANAGEMENT_H
