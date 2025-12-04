#ifndef DATAMANAGEMENT_H
#define DATAMANAGEMENT_H

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
    explicit dataManagement(QObject *parent = nullptr);  //grapgWidget이 제거되면 자동으로 settingDial이 해제됨

    //setter
    void setTimeInterval(std::chrono::nanoseconds  interval);
    void setCyclePerSecond(double cyclePerSecond);
    void setSamplePerCycle(int samplePerCycle);
    void setGraphWidth(double gWidth);
    void setInputFreq(double frequency);
    void setTimeScale(int timeScale);
    void setUpdateMode(updateMode mode);
    void setPaused(bool p);
    void setVHarmonicOrder(int vHarmonicOrder);
    void setVHarmonicMag(double vHarmonicMag);
    void setVHarmonicPhase(int vHarmonicPhase);
    void setCHarmonicOrder(int cHarmonicOrder);
    void setCHarmonicMag(double cHarmonicMag);
    void setCHarmonicPhase(int cHarmonicPhase);



    //getter
    std::chrono::nanoseconds getTimeInterval() const{return interval_ns;}
    double getCyclePerSecond() const { return cyclePerSecond; }
    int getSamplePerCycle() const { return samplePerCycle; }
    double getGraphWidth() const {return gWidth;}
    double getInputFreq() const {return frequency;}
    int getTimeScale() const {return timeScale;}
    bool isPaused() const { return paused; }
    updateMode getUpdateMode() const;
    int getVHarmonicOrder() const {return vHarmonicOrder;}
    double getVHarmonicMag() const {return vHarmonicMag;}
    int getVHarmonicPhase() const {return vHarmonicPhase;}
    int getCHarmonicOrder() const {return cHarmonicOrder;}
    double getCHarmonicMag() const {return cHarmonicMag;}
    int getCHarmonicPhase() const {return cHarmonicPhase;}


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

    //전압, 전류 값 구조체
    struct data {
        double vAVal, cAVal, vBVal, cBVal, vCVal, cCVal;
        double vAValLL, vBValLL, vCValLL;
        std::chrono::nanoseconds t;
    };


    //전압, 전류, 전력 구조체 생성 (전체, 기본파, 고조파 RMS + 전력)
    struct measure_data {
        std::chrono::nanoseconds t;

        // 상별 RMS
        double vRmsA, vRmsB, vRmsC;
        double iRmsA, iRmsB, iRmsC;

        double vFundA, vFundB, vFundC;
        double iFundA, iFundB, iFundC;

        double vHarmA, vHarmB, vHarmC;
        double iHarmA, iHarmB, iHarmC;

        double pA, pB, pC;
        double pTotal;

        double vRmsALL, vRmsBLL, vRmsCLL;
        double vFundALL, vFundBLL, vFundCLL;
        double vHarmALL, vHarmBLL, vHarmCLL;

        double residualSumV, residualSumI;
    };

    void resetRMSAccum();

    // phasor 구조체
    struct phasor_data {
        // ===== 기본파 Voltage (A/B/C) =====
        double vAReal = 0.0, vAImag = 0.0, vAMag = 0.0, vAPhaseDeg = 0.0;
        double vBReal = 0.0, vBImag = 0.0, vBMag = 0.0, vBPhaseDeg = 0.0;
        double vCReal = 0.0, vCImag = 0.0, vCMag = 0.0, vCPhaseDeg = 0.0;

        //===== 선간전압 voltage (A/B/C) =====
        double vALLReal = 0.0, vALLImag = 0.0, vALLMag = 0.0, vALLPhaseDeg = 0.0;
        double vBLLReal = 0.0, vBLLImag = 0.0, vBLLMag = 0.0, vBLLPhaseDeg = 0.0;
        double vCLLReal = 0.0, vCLLImag = 0.0, vCLLMag = 0.0, vCLLPhaseDeg = 0.0;

        // ===== 기본파 Current (A/B/C) =====
        double cAReal = 0.0, cAImag = 0.0, cAMag = 0.0, cAPhaseDeg = 0.0;
        double cBReal = 0.0, cBImag = 0.0, cBMag = 0.0, cBPhaseDeg = 0.0;
        double cCReal = 0.0, cCImag = 0.0, cCMag = 0.0, cCPhaseDeg = 0.0;

        // ===== 고조파(“한 상만”, A상 기준) =====
        int    vOrderUsed = 1; // 전압 고조파 차수 (예: 3,5,7...)
        double hvReal = 0.0, hvImag = 0.0, hvMag = 0.0, hvPhaseDeg = 0.0;

        int    cOrderUsed = 1; // 전류 고조파 차수
        double hcReal = 0.0, hcImag = 0.0, hcMag = 0.0, hcPhaseDeg = 0.0;
    };


    //페이저 계산 함수
    phasor_data calcPhasor() const;

    //auto getVoltageData() {return voltage_data_queue;}
    data getLatestVoltageAndCurrent();
    double sumTime = 0.0; //누적시간

    std::deque<measure_data> measures;

    // 위상 오프셋 접근자
    double getPhaseOffset() const { return phaseOffsetRad; }
    void   setPhaseOffset(double rad) { phaseOffsetRad = rad; }


    void resetTick();
    void advanceTick();
    double getTickTimeSec() const;

    struct VoltageSet {
        QVector<double> RMS;                // [A, B, C, Average]
        QVector<double> RMS_MAX;
        QVector<QString> RMS_MAX_Time;
        QVector<double> RMS_MIN;
        QVector<QString> RMS_MIN_Time;

        QVector<double> RMS_LL;
        QVector<double> RMS_LL_MAX;
        QVector<QString> RMS_LL_MAX_Time;
        QVector<double> RMS_LL_MIN;
        QVector<QString> RMS_LL_MIN_Time;

        QVector<double> Fundamental;
        QVector<double> Fundamental_MAX;
        QVector<QString> Fundamental_MAX_Time;
        QVector<double> Fundamental_MIN;
        QVector<QString> Fundamental_MIN_Time;

        QVector<double> Fundamental_LL;
        QVector<double> Fundamental_LL_MAX;
        QVector<QString> Fundamental_LL_MAX_Time;
        QVector<double> Fundamental_LL_MIN;
        QVector<QString> Fundamental_LL_MIN_Time;

        QVector<double> THD;                // [A, B, C]
        QVector<double> THD_MAX;
        QVector<QString> THD_MAX_Time;

        QVector<double> THDLL;
        QVector<double> THDLL_MAX;
        QVector<QString> THDLL_MAX_Time;

        double Frequency = 0.0;
        double Frequency_MAX = 0.0;
        QString Frequency_MAX_Time;
        double Frequency_MIN = 0.0;
        QString Frequency_MIN_Time;

        double Residual = 0.0;
        double Residual_MAX;
        QString Residual_MAX_Time;
        double Residual_MIN;
        QString Residual_MIN_Time;
    };

    struct CurrentSet {
        QVector<double> RMS;                // [A, B, C, Average]
        QVector<double> RMS_MAX;
        QVector<QString> RMS_MAX_Time;
        QVector<double> RMS_MIN;
        QVector<QString> RMS_MIN_Time;

        QVector<double> Fundamental;
        QVector<double> Fundamental_MAX;
        QVector<QString> Fundamental_MAX_Time;
        QVector<double> Fundamental_MIN;
        QVector<QString> Fundamental_MIN_Time;

        QVector<double> THD;                // [A, B, C]
        QVector<double> THD_MAX;
        QVector<QString> THD_MAX_Time;

        double Residual = 0.0;
        double Residual_MAX;
        QString Residual_MAX_Time;
        double Residual_MIN;
        QString Residual_MIN_Time;
    };

    struct PowerSet {
        QVector<double> active;
        QVector<double> active_MAX;
        QVector<QString> active_MAX_Time;
        QVector<double> active_MIN;
        QVector<QString> active_MIN_Time;

        QVector<double> reactive;
        QVector<double> reactive_MAX;
        QVector<QString> reactive_MAX_Time;
        QVector<double> reactive_MIN;
        QVector<QString> reactive_MIN_Time;

        QVector<double> apparent;
        QVector<double> apparent_MAX;
        QVector<QString> apparent_MAX_Time;
        QVector<double> apparent_MIN;
        QVector<QString> apparent_MIN_Time;

        QVector<double> pf;
        QVector<double> pf_MAX;
        QVector<QString> pf_MAX_Time;
        QVector<double> pf_MIN;
        QVector<QString> pf_MIN_Time;

        double energy = 0.0;
    };

    struct AnalysisSet {
        QVector<double> voltSymm;
        QVector<double> voltSymm_MAX;
        QVector<QString> voltSymm_MAX_Time;

        QVector<double> voltSymm_LL;
        QVector<double> voltSymm_LL_MAX;
        QVector<QString> voltSymm_LL_MAX_Time;

        QVector<double> voltUnbal;
        QVector<double> voltUnbal_MAX;
        QVector<QString> voltUnbal_MAX_Time;

        QVector<double> currsymm;
        QVector<double> currsymm_MAX;
        QVector<QString> currsymm_MAX_Time;

        QVector<double> currUnbal;
        QVector<double> currUnbal_MAX;
        QVector<QString> currUnbal_MAX_Time;
    };

    struct harmornicSet {
        double harmV;
        double harmA;
    };

    struct oneSecList {
        VoltageSet voltage;
        CurrentSet current;
        PowerSet power;
        AnalysisSet symmUnbal;
        harmornicSet harmonic;
    };

signals:
    void dataChanged();
    void rmsDataChanged(measure_data  m);
    void pausedChanged(bool paused);  // 일시정지, 시작 토글 신호

private slots:
    void captureData();

private:
    QChronoTimer  t;
    std::chrono::nanoseconds interval_ns = std::chrono::nanoseconds(0);      //업데이트 주기
    double dt_sec = 0.0;                //QTimer가 실제로 도는 틱 간격
    double cyclePerSecond = 20.0;       //시간당 사이클 수
    int samplePerCycle = 20;            //사이클당 샘플 수
    double gWidth = 1.0;                //그래프폭
    double frequency = 60.0;            //주파수
    int timeScale = 5;                  //시간비율
    double phaseOffsetRad = 0.0;        //위상 보정항(라디안)
    int vHarmonicOrder = 1;             //전압 고조파 차수
    double vHarmonicMag = 0;            //전압 고조파 크기
    int vHarmonicPhase = 0;             //전압 고조파 위상
    int cHarmonicOrder = 1;             //전류 고조파 차수
    double cHarmonicMag = 0;            //전류 고조파 크기
    int cHarmonicPhase = 0;             //전류 고조파 위상

    //3상 전압, 전류
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

    //전압Rms, 전류Rms 전력 값 누적
    double sumAV = 0.0;          //A상 전압 누적값
    double sumAI = 0.0;          //A상 전류 누적값
    double sumBV = 0.0;          //B상 전압 누적값
    double sumBI = 0.0;          //B상 전류 누적값
    double sumCV = 0.0;          //C상 전압 누적값
    double sumCI = 0.0;          //C상 전류 누적값
    double fundSumAV = 0.0;      //A상 전압 기본파 누적값
    double fundSumAC = 0.0;      //A상 전류 기본파 누적값
    double fundSumBV = 0.0;      //B상 전압 기본파 누적값
    double fundSumBC = 0.0;      //B상 전류 기본파 누적값
    double fundSumCV = 0.0;      //C상 전압 기본파 누적값
    double fundSumCC = 0.0;      //C상 전류 기본파 누적값
    double harmSumAV = 0.0;  //전압 고조파 누적값
    double harmSumAC = 0.0;  //전류 고조파 누적값
    double harmSumBV = 0.0;  //전압 고조파 누적값
    double harmSumBC = 0.0;  //전류 고조파 누적값

    //선간 전압 누적 RMS
    double fundSumAVLL = 0.0;
    double fundSumBVLL = 0.0;
    double fundSumCVLL = 0.0;
    double sumAVLL = 0.0;
    double sumBVLL = 0.0;
    double sumCVLL = 0.0;
    double harmSumAVLL =0.0;
    double harmSumBVLL =0.0;
    double harmSumCVLL =0.0;




    double sumP_A = 0.0; //전력 누적값
    double sumP_B = 0.0; //전력 누적값
    double sumP_C = 0.0; //전력 누적값
    int sampleCnt = 0; //누적 샘플 수

    //residual
    double sumResidualV= 0.0;
    double sumResidualI= 0.0;


    size_t max_measures = 1024;


    //그래프 저장값
    std::chrono::nanoseconds simulationTime = std::chrono::nanoseconds(0);
    std::deque<data> data_queue; //전압 데이터

    const size_t max_count = 10000;

    bool paused = false;              // 일시정지, 시작 확인

};

#endif // DATAMANAGEMENT_H
