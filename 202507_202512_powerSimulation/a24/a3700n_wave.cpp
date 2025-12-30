#include "a3700n_wave.h"
#include "qcustomplot.h"

#include <QBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include <QFile>

a3700n_wave::a3700n_wave(dataManagement& dataMng)
    : dataMng(dataMng)
{
    loadQSS(":/styles/a3700n_wave_style.qss");
    setFixedSize(600, 310);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);


    //--------------시작, 일시정지, 각 그래프 CHECK -----------
    QHBoxLayout* topbarLay = new QHBoxLayout();

    QPushButton* playBtn = new QPushButton();
    playBtn->setCheckable(true);  // 토글 가능하게 설정
    playBtn->setFixedWidth(50);
    playBtn->setObjectName("playBtn");


    topbarLay->addWidget(playBtn);
    topbarLay->addStretch(1);

    QLabel* voltCheckLabel = new QLabel("Volt");
    voltCheckLabel->setObjectName("checkLabel");
    topbarLay->addWidget(voltCheckLabel);

    VACheck = new QCheckBox("A");
    VACheck->setObjectName("voltA");
    VACheck->setChecked(true);
    topbarLay->addWidget(VACheck);

    VBCheck = new QCheckBox("B");
    VBCheck->setObjectName("voltB");
    VBCheck->setChecked(true);
    topbarLay->addWidget(VBCheck);

    VCCheck = new QCheckBox("C");
    VCCheck->setObjectName("voltC");
    VCCheck->setChecked(true);
    topbarLay->addWidget(VCCheck);

    topbarLay->addStretch(1);

    QLabel* currCheckLabel = new QLabel("Curr");
    currCheckLabel->setObjectName("checkLabel");
    topbarLay->addWidget(currCheckLabel);

    CACheck = new QCheckBox("A");
    CACheck->setObjectName("currA");
    CACheck->setChecked(true);
    topbarLay->addWidget(CACheck);

    CBCheck = new QCheckBox("B");
    CBCheck->setObjectName("currB");
    CBCheck->setChecked(true);
    topbarLay->addWidget(CBCheck);

    CCCheck = new QCheckBox("C");
    CCCheck->setObjectName("currC");
    CCCheck->setChecked(true);
    topbarLay->addWidget(CCCheck);



    connect(VACheck, &QCheckBox::toggled, this, [this](bool checked) {
        voltageGraphA->setVisible(checked);
        customPlot->replot(QCustomPlot::rpQueuedReplot);
    });
    connect(VBCheck, &QCheckBox::toggled, this, [this](bool checked) {
        voltageGraphB->setVisible(checked);
        customPlot->replot(QCustomPlot::rpQueuedReplot);
    });
    connect(VCCheck, &QCheckBox::toggled, this, [this](bool checked) {
        voltageGraphC->setVisible(checked);
        customPlot->replot(QCustomPlot::rpQueuedReplot);
    });
    connect(CACheck, &QCheckBox::toggled, this, [this](bool checked) {
        currentGraphA->setVisible(checked);
        customPlot->replot(QCustomPlot::rpQueuedReplot);
    });
    connect(CBCheck, &QCheckBox::toggled, this, [this](bool checked) {
        currentGraphB->setVisible(checked);
        customPlot->replot(QCustomPlot::rpQueuedReplot);
    });
    connect(CCCheck, &QCheckBox::toggled, this, [this](bool checked) {
        currentGraphC->setVisible(checked);
        customPlot->replot(QCustomPlot::rpQueuedReplot);
    });


    connect (playBtn, &QPushButton::toggled, this, [this](bool checked) {
        stopWave = checked;

        if(checked) {
            stop_vA = vABuffer;
            stop_vB = vBBuffer;
            stop_vC = vCBuffer;
            stop_cA = cABuffer;
            stop_cB = cBBuffer;
            stop_cC = cCBuffer;
        }
    });

    //-------------- 스케일 조정, 그래프 출력 -------------

    QHBoxLayout* buttomLay = new QHBoxLayout();

    QWidget* outerBox = new QWidget();
    outerBox->setObjectName("outerBox");
    outerBox->setContentsMargins(2, 2, 2, 2);
    outerBox->setFixedWidth(65);
    outerBox->setFixedHeight(215);

    QVBoxLayout* outerLayout = new QVBoxLayout(outerBox);
    outerLayout->setContentsMargins(0, 0, 0, 0);
    outerLayout->setSpacing(4);

    QLabel* titleLabel = new QLabel("Scale");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setObjectName("titleLab");
    outerLayout->addWidget(titleLabel);

    QWidget* innerBox = new QWidget(outerBox);
    innerBox->setObjectName("innerBox");

    QVBoxLayout* innerLayout = new QVBoxLayout(innerBox);
    innerLayout->setSpacing(2);
    innerLayout->setContentsMargins(2, 2, 2, 2);
    innerLayout->setAlignment(Qt::AlignTop);


    autoBtn = new QPushButton("Auto");
    autoBtn->setFixedHeight(43);
    autoBtn->setCheckable(true);
    autoBtn->setChecked(true);
    autoBtn->setObjectName("autoBtn");
    innerLayout->addWidget(autoBtn);

    VABtn = new RichTextButton(
        "<span style='font-size: 15pt; font-weight: bold;'>V</span>"
        "<span style='font-size: 10pt; font-weight: bold;'>/A</span>"
        );
    VABtn->setFixedHeight(43);
    VABtn->setObjectName("VABtn");
    VABtn->setCheckable(true);
    VABtn->setChecked(false);
    innerLayout->addWidget(VABtn);


    plusBtn = new QPushButton("+");
    plusBtn->setFixedHeight(43);
    plusBtn->setObjectName("pmBtn");
    innerLayout->addWidget(plusBtn);

    minusBtn = new QPushButton("-");
    minusBtn->setFixedHeight(43);
    minusBtn->setObjectName("pmBtn");
    innerLayout->addWidget(minusBtn);

    outerLayout->addWidget(innerBox);
    buttomLay->addWidget(outerBox);
    buttomLay->addSpacing(7);


    QObject::connect(VABtn, &QPushButton::toggled, [&](bool checked){
        if (checked) {
            VABtn->setHtml("<span style='font-size: 10pt; font-weight: bold;'>V/</span>"
                           "<span style='font-size: 15pt; font-weight: bold;'>A</span>");
        } else {
            VABtn->setHtml("<span style='font-size: 15pt; font-weight: bold;'>V</span>"
                           "<span style='font-size: 10pt; font-weight: bold;'>/A</span>");
        }
    });

    connect(plusBtn, &QPushButton::clicked, this, [&]() {
        autoBtn->setChecked(false);
        adjustScale(+1); // 1단계 확대
    });

    connect(minusBtn, &QPushButton::clicked, this, [&]() {
        autoBtn->setChecked(false);
        adjustScale(-1); // 1단계 축소
    });

    connect(autoBtn, &QPushButton::clicked, this, [&]() {
        autoScale();
    });

//---------------------------- 그래프 ----------------------------------
    customPlot = new QCustomPlot(this);

    voltageGraphA = customPlot->addGraph(customPlot->xAxis, customPlot->yAxis);
    voltageGraphA->setName("Va");

    voltageGraphB = customPlot->addGraph(customPlot->xAxis, customPlot->yAxis);
    voltageGraphB->setName("Vb");

    voltageGraphC = customPlot->addGraph(customPlot->xAxis, customPlot->yAxis);
    voltageGraphC->setName("Vc");

    currentGraphA = customPlot->addGraph(customPlot->xAxis, customPlot->yAxis2);
    currentGraphA->setName("Ia");

    currentGraphB = customPlot->addGraph(customPlot->xAxis, customPlot->yAxis2);
    currentGraphB->setName("Ib");

    currentGraphC = customPlot->addGraph(customPlot->xAxis, customPlot->yAxis2);
    currentGraphC->setName("Ic");

    // Voltage(진한 원색)
    const QColor VOLT_R(255,   0,   0);   // A상 전압: 빨강
    const QColor VOLT_B(  0,   0, 255);   // B상 전압: 파랑
    const QColor VOLT_G(0  , 160,   0);   // C상 전압: 초록

    // Current(파스텔)
    const QColor CURR_R(235, 133, 133);   // A상 전류: 연빨강
    const QColor CURR_B(133, 184, 235);   // B상 전류: 연파랑
    const QColor CURR_G(133, 235, 133);   // C상 전류: 연초록

    auto styleGraph = [](QCPGraph* g, const QColor& color, qreal widthPx, bool showScatter=true){
        QPen p = g->pen();
        p.setColor(color);
        p.setWidthF(widthPx);
        g->setPen(p);
        g->setLineStyle(QCPGraph::lsLine);
        g->setAntialiased(true);
        g->setAntialiasedScatters(true);
        g->setAdaptiveSampling(true);
        if (showScatter) {
            g->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 2)); // 점 표시
        } else {
            g->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssNone));
        }
    };

    // 전압(진한 원색, 선 두껍게, 점 표시)
    styleGraph(voltageGraphA, VOLT_R, 1.8, false);
    styleGraph(voltageGraphB, VOLT_B, 1.8, false);
    styleGraph(voltageGraphC, VOLT_G, 1.8, false);

    // 전류(파스텔, 선은 약간 얇게, 점 표시)
    styleGraph(currentGraphA, CURR_R, 1.2, false);
    styleGraph(currentGraphB, CURR_B, 1.2, false);
    styleGraph(currentGraphC, CURR_G, 1.2, false);


    customPlot->yAxis2->setVisible(true);
    customPlot->yAxis->setRange(-600, 600);
    customPlot->yAxis2->setRange(-40, 40);

    // y축 범위 설정 (고정)
    customPlot->yAxis->setRange(-600, 600);
    customPlot->replot(QCustomPlot::rpQueuedReplot);

    customPlot->yAxis2->setRange(-40,40);
    customPlot->replot(QCustomPlot::rpQueuedReplot);

    customPlot->xAxis->setTickLabels(false); //x축이 보이지 않게

    //격자 넣기
    QSharedPointer<QCPAxisTicker> xTicker(new QCPAxisTicker);
    xTicker->setTickCount(10);   // 세로줄 (X축 격자) 약 10개
    customPlot->xAxis->setTicker(xTicker);

    QSharedPointer<QCPAxisTicker> yTicker(new QCPAxisTicker);
    yTicker->setTickCount(9);    // 가로줄 (Y축 격자) 약 9개
    customPlot->yAxis->setTicker(yTicker);
    customPlot->yAxis2->setTicker(yTicker);
    customPlot->yAxis->ticker()->setTickCount(8);
    customPlot->yAxis2->ticker()->setTickCount(8);

    customPlot->xAxis->grid()->setVisible(true);
    customPlot->yAxis->grid()->setVisible(true);
    customPlot->yAxis2->grid()->setVisible(true);

    // 부드러운 얇은 선
    customPlot->xAxis->grid()->setPen(QPen(QColor(220, 220, 220), 0.5));
    customPlot->yAxis->grid()->setZeroLinePen(QPen(Qt::black, 2)); // y축 가운데 선 진하게
    customPlot->yAxis->grid()->setPen(QPen(QColor(220, 220, 220), 0.5));
    customPlot->yAxis2->grid()->setPen(QPen(QColor(220, 220, 220), 0.5));


    // 전압 측 (왼쪽)
    customPlot->yAxis->setLabelFont(QFont("맑은 고딕", 10));
    customPlot->yAxis->setLabelColor(Qt::black);
    customPlot->yAxis->setTickLabelSide(QCPAxis::lsOutside);
    customPlot->yAxis->setLabel("[V]");
    customPlot->yAxis->setLabelPadding(12);

    // 전류 축 (오른쪽)
    customPlot->yAxis2->setLabelFont(QFont("맑은 고딕", 10));
    customPlot->yAxis2->setLabelColor(Qt::black);
    customPlot->yAxis2->setTickLabelSide(QCPAxis::lsOutside);
    customPlot->yAxis2->setLabel("[A]");
    customPlot->yAxis2->setLabelPadding(8);


    buttomLay->addWidget(customPlot);
//--------------------------------------------------------------

    // 메인 레이아웃에 추가
    mainLayout->addLayout(topbarLay);
    mainLayout->setSpacing(7);
    mainLayout->addLayout(buttomLay);
    setLayout(mainLayout);

    connect(&dataMng, &dataManagement::dataChanged, this, &a3700n_wave::updateGraph);
}

void a3700n_wave::updateGraph()
{
    if (stopWave) {
        //일시정지 상태에서는 새 데이터 무시, 기존 스냅샷만 표시
        voltageGraphA->data()->clear();
        voltageGraphB->data()->clear();
        voltageGraphC->data()->clear();
        currentGraphA->data()->clear();
        currentGraphB->data()->clear();
        currentGraphC->data()->clear();

        for (int i = 0; i < stop_vA.size(); ++i) {
            voltageGraphA->addData(i, stop_vA[i]);
            voltageGraphB->addData(i, stop_vB[i]);
            voltageGraphC->addData(i, stop_vC[i]);
            currentGraphA->addData(i, stop_cA[i]);
            currentGraphB->addData(i, stop_cB[i]);
            currentGraphC->addData(i, stop_cC[i]);
        }

        customPlot->replot(QCustomPlot::rpQueuedReplot);
        return;
    }

    auto s = dataMng.getLatestVoltageAndCurrent();
    const double vA = s.vAVal, iA = s.cAVal;
    const double vB = s.vBVal, iB = s.cBVal;
    const double vC = s.vCVal, iC = s.cCVal;

    // --- 버퍼 적재 ---
    vABuffer.append(vA);  cABuffer.append(iA);
    vBBuffer.append(vB);  cBBuffer.append(iB);
    vCBuffer.append(vC);  cCBuffer.append(iC);

    sampleIndex_++;
    emitCounter_++;

    const int spc = dataMng.getSamplePerCycle();
    bool shouldEmit = false;

    if (emitCounter_ >= spc * dataMng.getCyclePerSecond()) {  // 1동안 수집된 데이터
        shouldEmit = true;
        emitCounter_ = 0;
    }
    const int keepCount = 2 * spc;

    if (shouldEmit) {
        // --- 최근 2주기만 남기기 ---
        auto trim = [keepCount](QVector<double>& buf) {
            if (buf.size() > keepCount)
                buf.erase(buf.begin(), buf.end() - keepCount);
        };
        trim(vABuffer); trim(vBBuffer); trim(vCBuffer);
        trim(cABuffer); trim(cBBuffer); trim(cCBuffer);

        // --- 그래프 데이터 갱신 ---
        voltageGraphA->data()->clear();
        voltageGraphB->data()->clear();
        voltageGraphC->data()->clear();
        currentGraphA->data()->clear();
        currentGraphB->data()->clear();
        currentGraphC->data()->clear();

        for (int i = 0; i < vABuffer.size(); ++i) {
            voltageGraphA->addData(i, vABuffer[i]);
            voltageGraphB->addData(i, vBBuffer[i]);
            voltageGraphC->addData(i, vCBuffer[i]);
            currentGraphA->addData(i, cABuffer[i]);
            currentGraphB->addData(i, cBBuffer[i]);
            currentGraphC->addData(i, cCBuffer[i]);


            VMax = std::max({VMax, vABuffer[i], vBBuffer[i], vCBuffer[i]});
            VMin = std::min({VMin, vABuffer[i], vBBuffer[i], vCBuffer[i]});
            CMax = std::max({CMax, cABuffer[i], cBBuffer[i], cCBuffer[i]});
            CMin = std::min({CMin, cABuffer[i], cBBuffer[i], cCBuffer[i]});


            customPlot->xAxis->setRange(0, keepCount);

            double absV = std::max(std::abs(VMax), std::abs(VMin)); // 절댓값 기준
            double absC = std::max(std::abs(CMax), std::abs(CMin));

            if (absV == 0 && absC == 0) {
                return; // 현재 newRange, newRangeC 그대로 유지
            }

            if(autoBtn->isChecked()){
                if      (absV < 4)   newRange = 4;
                else if (absV < 8)   newRange = 8;
                else if (absV < 20)  newRange = 20;
                else if (absV < 40)  newRange = 40;
                else if (absV < 80)  newRange = 80;
                else if (absV < 200) newRange = 200;
                else if (absV < 400) newRange = 400;
                else                  newRange = 800;


                if      (absC < 2)   newRangeC = 2;
                else if (absC < 4)   newRangeC = 4;
                else if (absC < 8)   newRangeC = 8;
                else if (absC < 20)  newRangeC = 20;
                else if (absC < 40)  newRangeC = 40;

            }


            customPlot->yAxis->setRange(-newRange, newRange);
            customPlot->yAxis2->setRange(-newRangeC, newRangeC);



            customPlot->replot(QCustomPlot::rpQueuedReplot);

        }

        //다시 최소화하기
        VMax = std::numeric_limits<double>::lowest(); // 가장 작은 값으로 초기화
        VMin = std::numeric_limits<double>::max(); // 가장 큰 값으로 초기화
        CMax = std::numeric_limits<double>::lowest();
        CMin = std::numeric_limits<double>::max();

    }

}

void a3700n_wave::autoScale() {
    double absV = std::max(std::abs(VMax), std::abs(VMin)); // 절댓값 기준
    double absC = std::max(std::abs(CMax), std::abs(CMin));

    if (absV == 0 && absC == 0) {
        return; // 현재 newRange, newRangeC 그대로 유지
    }

    if(autoBtn->isChecked()){
        if      (absV < 4)   newRange = 4;
        else if (absV < 8)   newRange = 8;
        else if (absV < 20)  newRange = 20;
        else if (absV < 40)  newRange = 40;
        else if (absV < 80)  newRange = 80;
        else if (absV < 200) newRange = 200;
        else if (absV < 400) newRange = 400;
        else                  newRange = 800;


        if      (absC < 2)   newRangeC = 2;
        else if (absC < 4)   newRangeC = 4;
        else if (absC < 8)   newRangeC = 8;
        else if (absC < 20)  newRangeC = 20;
        else if (absC < 40)  newRangeC = 40;

    }

    customPlot->yAxis->setRange(-newRange, newRange);
    customPlot->yAxis2->setRange(-newRangeC, newRangeC);

    //다시 최소화하기
    VMax = std::numeric_limits<double>::lowest(); // 가장 작은 값으로 초기화
    VMin = std::numeric_limits<double>::max(); // 가장 큰 값으로 초기화
    CMax = std::numeric_limits<double>::lowest();
    CMin = std::numeric_limits<double>::max();


    customPlot->replot(QCustomPlot::rpImmediateRefresh);
}

void a3700n_wave::adjustScale(int direction){
    if (autoBtn->isChecked()) return; // 자동모드면 무시

    if (VABtn->isChecked()) {
        // 전류 축 조정
        if (direction > 0)
            newRangeC *= 2;
        else
            newRangeC /= 2;
    } else {
        // 전압 축 조정
        if (direction > 0)
            newRange *= 2;
        else
            newRange /= 2;
    }

    customPlot->yAxis->setRange(-newRange, newRange);
    customPlot->yAxis2->setRange(-newRangeC, newRangeC);

    customPlot->replot(QCustomPlot::rpQueuedReplot);

    //다시 최소화하기
    VMax = std::numeric_limits<double>::lowest(); // 가장 작은 값으로 초기화
    VMin = std::numeric_limits<double>::max(); // 가장 큰 값으로 초기화
    CMax = std::numeric_limits<double>::lowest();
    CMin = std::numeric_limits<double>::max();
}


void a3700n_wave::loadQSS(const QString& path)
{
    QFile file(path);
    if (file.open(QFile::ReadOnly)) {
        QString style = QString::fromUtf8(file.readAll());
        this->setStyleSheet(style); // 현재 파일에만 적용한다.
        file.close();
    } else {
        qWarning() << "QSS 파일을 찾을 수 없습니다:" << path;
    }
}


void a3700n_wave::setVolCheck(bool checked) {
    if(checked) {
        VACheck->setEnabled(true);
        VBCheck->setEnabled(true);
        VCCheck->setEnabled(true);

        VACheck->setChecked(true);
        VBCheck->setChecked(true);
        VCCheck->setChecked(true);
    }
    else {
        VACheck->setChecked(false);
        VBCheck->setChecked(false);
        VCCheck->setChecked(false);

        VACheck->setEnabled(false);
        VBCheck->setEnabled(false);
        VCCheck->setEnabled(false);
    }

    customPlot->replot(QCustomPlot::rpImmediateRefresh);
}

void a3700n_wave::setCorCheck(bool checked) {
    if(checked) {
        CACheck->setEnabled(true);
        CBCheck->setEnabled(true);
        CCCheck->setEnabled(true);

        CACheck->setChecked(true);
        CBCheck->setChecked(true);
        CCCheck->setChecked(true);
    }
    else {
        CACheck->setChecked(false);
        CBCheck->setChecked(false);
        CCCheck->setChecked(false);

        CACheck->setEnabled(false);
        CBCheck->setEnabled(false);
        CCCheck->setEnabled(false);
    }

    customPlot->replot(QCustomPlot::rpImmediateRefresh);
}






