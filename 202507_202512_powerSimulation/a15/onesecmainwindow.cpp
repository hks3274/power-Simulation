#include "onesecmainwindow.h"
#include <QGridLayout>
#include <QGroupBox>
#include <QFont>
#include <QMenuBar>

oneSecMainWindow::oneSecMainWindow(dataManagement& dataMng, QWidget* parent)
    : QMainWindow(parent)
    , dataMng(dataMng)
{
    this->setWindowTitle("1초 데이터 창");
    this->resize(500, 400);

    QWidget* central = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(central);

    // 상태 및 제어 영역
    QHBoxLayout* controlLayout = new QHBoxLayout();
    statusLabel = new QLabel("상태: 대기중", this);
    toggleBtn   = new QToolButton(this);
    toggleBtn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    toggleBtn->setShortcut(QKeySequence(Qt::Key_Space));
    toggleBtn->setStyleSheet(
        "QToolButton { padding:4px 10px; border:1px solid #cfcfcf; border-radius:6px; }"
        "QToolButton:hover { background-color:#e9e9e9; }"
        "QToolButton:checked { background-color:#444; color:#fff; border-color:#444; }"
        "QToolButton:pressed { background-color:#d0d0d0; }"
        );

    // 초기 상태
    if (dataMng.isPaused()) {
        statusLabel->setText("상태: 정지됨");
        statusLabel->setStyleSheet("QLabel { color: red; }");
        toggleBtn->setText("시작");
    } else {
        statusLabel->setText("상태: 실행 중");
        statusLabel->setStyleSheet("QLabel { color: green; }");
        toggleBtn->setText("정지");
    }

    // 폰트 설정
    QFont boldFont;
    boldFont.setBold(true);
    statusLabel->setFont(boldFont);

    // controlLayout 구성
    controlLayout->addWidget(statusLabel);
    controlLayout->addStretch();
    controlLayout->addWidget(toggleBtn);

    mainLayout->addLayout(controlLayout);

    // RMS 값 표시 영역
    QGroupBox* rmsGroup = new QGroupBox("RMS 값", this);
    QGridLayout* rmsLayout = new QGridLayout(rmsGroup);

    // 폰트 설정
    QFont labelFont;
    labelFont.setFamily("Courier New");
    labelFont.setPointSize(10);

    // RMS 라벨 및 값 라벨 생성
    QLabel* rmsBasicVLabel = new QLabel("기본파 V :", this);
    rmsBasicVLabel->setFont(labelFont);
    rmsBasicVLabel->setAlignment(Qt::AlignCenter);
    rmsLayout->addWidget(rmsBasicVLabel, 0, 0);
    rmsBasicV = new QLabel("0.00 V", this);
    rmsLayout->addWidget(rmsBasicV, 0, 1);

    QLabel* rmsBasicILabel = new QLabel("기본파 I :", this);
    rmsBasicILabel->setFont(labelFont);
    rmsBasicILabel->setAlignment(Qt::AlignCenter);
    rmsLayout->addWidget(rmsBasicILabel, 0, 2);
    rmsBasicI = new QLabel("0.00 A", this);
    rmsLayout->addWidget(rmsBasicI, 0, 3);

    QLabel* rmsHarmonicVLabel = new QLabel("고조파 V :", this);
    rmsHarmonicVLabel->setFont(labelFont);
    rmsHarmonicVLabel->setAlignment(Qt::AlignCenter);
    rmsLayout->addWidget(rmsHarmonicVLabel, 1, 0);
    rmsHarmonicV = new QLabel("0.00 V", this);
    rmsLayout->addWidget(rmsHarmonicV, 1, 1);

    QLabel* rmsHarmonicILabel = new QLabel("고조파 I :", this);
    rmsHarmonicILabel->setFont(labelFont);
    rmsHarmonicILabel->setAlignment(Qt::AlignCenter);
    rmsLayout->addWidget(rmsHarmonicILabel, 1, 2);
    rmsHarmonicI = new QLabel("0.00 A", this);
    rmsLayout->addWidget(rmsHarmonicI, 1, 3);

    QLabel* rmsTotalVLabel = new QLabel("합성파 V :", this);
    rmsTotalVLabel->setFont(labelFont);
    rmsTotalVLabel->setAlignment(Qt::AlignCenter);
    rmsLayout->addWidget(rmsTotalVLabel, 2, 0);
    rmsTotalV = new QLabel("0.00 V", this);
    rmsLayout->addWidget(rmsTotalV, 2, 1);

    QLabel* rmsTotalILabel = new QLabel("합성파 I :", this);
    rmsTotalILabel->setFont(labelFont);
    rmsTotalILabel->setAlignment(Qt::AlignCenter);
    rmsLayout->addWidget(rmsTotalILabel, 2, 2);
    rmsTotalI = new QLabel("0.00 A", this);
    rmsLayout->addWidget(rmsTotalI, 2, 3);

    mainLayout->addWidget(rmsGroup);

    // Phasor 값 표시 영역
    QGroupBox* phasorGroup = new QGroupBox("위상 값", this);
    QGridLayout* phasorLayout = new QGridLayout(phasorGroup);

    QLabel* phasorBasicVLabel = new QLabel("기본파 V 위상:", this);
    phasorBasicVLabel->setFont(labelFont);
    phasorBasicVLabel->setAlignment(Qt::AlignCenter);
    phasorLayout->addWidget(phasorBasicVLabel, 0, 0);
    phasorBasicV = new QLabel("0.00°", this);
    phasorLayout->addWidget(phasorBasicV, 0, 1);

    QLabel* phasorBasicILabel = new QLabel("기본파 I 위상:", this);
    phasorBasicILabel->setFont(labelFont);
    phasorBasicILabel->setAlignment(Qt::AlignCenter);
    phasorLayout->addWidget(phasorBasicILabel, 0, 2);
    phasorBasicI = new QLabel("0.00°", this);
    phasorLayout->addWidget(phasorBasicI, 0, 3);

    QLabel* phasorHarmonicVLabel = new QLabel("고조파 V 위상:", this);
    phasorHarmonicVLabel->setFont(labelFont);
    phasorHarmonicVLabel->setAlignment(Qt::AlignCenter);
    phasorLayout->addWidget(phasorHarmonicVLabel, 1, 0);
    phasorHarmonicV = new QLabel("0.00°", this);
    phasorLayout->addWidget(phasorHarmonicV, 1, 1);

    QLabel* phasorHarmonicILabel = new QLabel("고조파 I 위상:", this);
    phasorHarmonicILabel->setFont(labelFont);
    phasorHarmonicILabel->setAlignment(Qt::AlignCenter);
    phasorLayout->addWidget(phasorHarmonicILabel, 1, 2);
    phasorHarmonicI = new QLabel("0.00°", this);
    phasorLayout->addWidget(phasorHarmonicI, 1, 3);

    mainLayout->addWidget(phasorGroup);

    // 전력 값 표시 영역
    QGroupBox* powerGroup = new QGroupBox("전력 값", this);
    QGridLayout* powerLayout = new QGridLayout(powerGroup);

    QLabel* avgPowerLabel = new QLabel("평균 전력 :", this);
    avgPowerLabel->setFont(labelFont);
    avgPowerLabel->setAlignment(Qt::AlignCenter);
    powerLayout->addWidget(avgPowerLabel, 0, 0);
    avgPower = new QLabel("0.00 W", this);
    powerLayout->addWidget(avgPower, 0, 1);

    QLabel* powerEnergyLabel = new QLabel("전력량 :", this);
    powerEnergyLabel->setFont(labelFont);
    powerEnergyLabel->setAlignment(Qt::AlignCenter);
    powerLayout->addWidget(powerEnergyLabel, 0, 2);
    powerEnergy = new QLabel("0.00 Wh", this);
    powerLayout->addWidget(powerEnergy, 0, 3);

    mainLayout->addWidget(powerGroup);

    // 데이터 라벨들 스타일 설정
    QFont dataFont;
    dataFont.setFamily("Courier New");
    dataFont.setPointSize(10);

    QList<QLabel*> dataLabels = {
        rmsBasicV, rmsBasicI, rmsHarmonicV, rmsHarmonicI, rmsTotalV, rmsTotalI,
        phasorBasicV, phasorBasicI, phasorHarmonicV, phasorHarmonicI,
        avgPower, powerEnergy
    };

    for (QLabel* label : dataLabels) {
        label->setFont(dataFont);
        label->setAlignment(Qt::AlignCenter); // 수평, 수직 모두 중심 정렬
        label->setStyleSheet("QLabel { background-color: #f0f0f0; padding: 5px; border: 1px solid #ccc; }");
        label->setMinimumWidth(80);
    }

    mainLayout->addStretch();
    setCentralWidget(central);

    // 버튼 이벤트 연결
    connect(toggleBtn, &QPushButton::clicked, this, [this, &dataMng]() {
        bool nowPaused = dataMng.isPaused();
        dataMng.setPaused(!nowPaused);  // 상태 반전
    });

    connect(&dataMng, &dataManagement::pausedChanged, this, [this](bool paused){
        if (paused) {
            statusLabel->setText("상태: 정지됨");
            statusLabel->setStyleSheet("QLabel { color: red; }");
            toggleBtn->setText("시작");
        } else {
            statusLabel->setText("상태: 실행 중");
            statusLabel->setStyleSheet("QLabel { color: green; }");
            toggleBtn->setText("정지");
        }
    });

    connect(&dataMng, &dataManagement::rmsDataChanged, this, &oneSecMainWindow::onRmsData);


}

// 데이터 업데이트 메소드들
void oneSecMainWindow::updateRMSValues(double basicV, double basicI, double harmonicV,
                                       double harmonicI, double totalV, double totalI)
{
    rmsBasicV->setText(QString::number(basicV, 'f', 2) + " V");
    rmsBasicI->setText(QString::number(basicI, 'f', 2) + " A");
    rmsHarmonicV->setText(QString::number(harmonicV, 'f', 2) + " V");
    rmsHarmonicI->setText(QString::number(harmonicI, 'f', 2) + " A");
    rmsTotalV->setText(QString::number(totalV, 'f', 2) + " V");
    rmsTotalI->setText(QString::number(totalI, 'f', 2) + " A");
}

void oneSecMainWindow::updatePhasorValues(double basicVPhase, double basicIPhase,
                                          double harmonicVPhase, double harmonicIPhase)
{
    phasorBasicV->setText(QString::number(basicVPhase, 'f', 2) + "°");
    phasorBasicI->setText(QString::number(basicIPhase, 'f', 2) + "°");
    phasorHarmonicV->setText(QString::number(harmonicVPhase, 'f', 2) + "°");
    phasorHarmonicI->setText(QString::number(harmonicIPhase, 'f', 2) + "°");
}

void oneSecMainWindow::updatePowerValues(double avgPowerVal, double energyVal)
{
    avgPower->setText(QString::number(avgPowerVal, 'f', 2) + " W");
    powerEnergy->setText(QString::number(energyVal, 'f', 2) + " Wh");
}


//1초 데이터 가공 프로세스
//RMS값 -> 1cycle 데이터들의 RMS 값으로 계산
//phasor 값 --> 1초 데이터 중 마지막 값
// 전력량 (1초 전력 X 구간간격으로) 값을 적산함

void oneSecMainWindow::onRmsData(dataManagement::measure_data m)
{
    // 이번 샘플 시각과 직전 기준 시각 차이
    auto elapsed = m.t - prev_sumTime;

    oneSecSampleCnt++;

    // qDebug() << "샘플개수:" << oneSecSampleCnt
    //          << "경과(ns):" << std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();

    // --- RMS 누적 ---
    sumFundV2 += m.vFundRMSVal * m.vFundRMSVal;
    sumFundI2 += m.cFundRMSVal * m.cFundRMSVal;
    sumHarmV2 += m.vHarmonicRMSVal * m.vHarmonicRMSVal;
    sumHarmI2 += m.cHarmonicRMSVal * m.cHarmonicRMSVal;
    sumTotalV2 += m.voltageRMSVal * m.voltageRMSVal;
    sumTotalI2 += m.currentRMSVal * m.currentRMSVal;

    sumPower += m.powerVal;

    // --- 마지막 위상값 ---
    lastPhasor = dataMng.calcPhasor();

    // === 1초 경과 체크 ===
    if (elapsed >= std::chrono::milliseconds(995)) {
        double osFundV   = std::sqrt(sumFundV2 / oneSecSampleCnt);
        double osFundI   = std::sqrt(sumFundI2 / oneSecSampleCnt);
        double osHarmV   = std::sqrt(sumHarmV2 / oneSecSampleCnt);
        double osHarmI   = std::sqrt(sumHarmI2 / oneSecSampleCnt);
        double osTotalV  = std::sqrt(sumTotalV2 / oneSecSampleCnt);
        double osTotalI  = std::sqrt(sumTotalI2 / oneSecSampleCnt);

        double osFundVPhase = lastPhasor.vPhaseDeg;
        double osFundIPhase = lastPhasor.cPhaseDeg;
        double osHarmVPhase = lastPhasor.hvPhaseDeg;
        double osHarmIPhase = lastPhasor.hcPhaseDeg;

        // --- 전력량 적산 ---
        double dt_sec = std::chrono::duration_cast<std::chrono::duration<double>>(elapsed).count();
        qDebug() <<  "dt_sec:" << dt_sec;

        qDebug() << "1초 지남";

        double avgPowerVal = sumPower/oneSecSampleCnt;         // 1초 평균 전력
        sumEnergy += avgPowerVal * dt_sec;
        double energyVal   = sumEnergy / 3600.0;  // Wh 변환

        updateRMSValues(osFundV, osFundI, osHarmV, osHarmI, osTotalV, osTotalI);
        updatePhasorValues(osFundVPhase, osFundIPhase, osHarmVPhase, osHarmIPhase);
        updatePowerValues(avgPowerVal, energyVal);

        // --- 리셋 ---
        prev_sumTime = m.t; // 이번 시각을 새 기준으로
        oneSecSampleCnt = 0;
        sumFundV2 = sumFundI2 = sumHarmV2 = sumHarmI2 = 0.0;
        sumTotalV2 = sumTotalI2 = sumPower = 0.0;
    }
}


oneSecMainWindow::~oneSecMainWindow() = default;
