#include "graphwidget.h"
#include<QBoxLayout>
#include <QDebug>
using namespace std::chrono;

graphWidget::graphWidget(dataManagement& dataMng)
    : dataMng{dataMng}
{

    //그래프 자동모드 되돌리기
    autoModeBtn = new QToolButton(this);
    autoModeBtn->setIcon(QIcon(":/icon/auto.png"));   // qrc에 넣은 경로
    autoModeBtn->setIconSize(QSize(18,18));
    autoModeBtn->setToolTip("자동모드 (축 자동 따라가기)");
    autoModeBtn->setAutoRaise(true);                         // 플랫한 툴버튼
    autoModeBtn->setCheckable(true);
    autoModeBtn->setChecked(true);                           // 시작은 자동모드 on
    autoModeBtn->setToolButtonStyle(Qt::ToolButtonIconOnly);

    autoModeBtn->setStyleSheet(
        "QToolButton:checked { background-color: #d0d0d0; color: white; }"
        "QToolButton:hover { background-color: #d0d0d0; }"
        );


    //그래프 생성
    customPlot = new QCustomPlot(this);
    customPlot->legend->setVisible(true);
    customPlot->legend->setBrush(QBrush(QColor(255,255,255,220)));

    customPlot->plotLayout()->insertRow(0);
    customPlot->plotLayout()->addElement(
        0, 0, new QCPTextElement(customPlot, QStringLiteral("3상 그래프"),
                           QFont(QStringLiteral("맑은 고딕"), 12, QFont::Bold)));

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
    styleGraph(voltageGraphA, VOLT_R, 1.8, true);
    styleGraph(voltageGraphB, VOLT_B, 1.8, true);
    styleGraph(voltageGraphC, VOLT_G, 1.8, true);

    // 전류(파스텔, 선은 약간 얇게, 점 표시)
    styleGraph(currentGraphA, CURR_R, 1.2, true);
    styleGraph(currentGraphB, CURR_B, 1.2, true);
    styleGraph(currentGraphC, CURR_G, 1.2, true);


    // 축 라벨 설정
    customPlot->xAxis->setLabel("Time [s]");
    customPlot->yAxis->setLabel("Voltage");
    customPlot->yAxis2->setLabel("Current");
    customPlot->yAxis2->setVisible(true);
    customPlot->yAxis->setRange(-700, 700);
    customPlot->yAxis2->setRange(-40, 40);


    customPlot->yAxis2->setVisible(true);
    customPlot->yAxis2->setTickLabels(true);

    // y축 범위 설정 (고정)
    customPlot->yAxis->setRange(-700, 700);
    customPlot->replot(QCustomPlot::rpQueuedReplot);

    customPlot->yAxis2->setRange(-40,40);
    customPlot->replot(QCustomPlot::rpQueuedReplot);


    //그래프 내 확대/이동 전부 허용 -> QChart의 경우 직접 함수를 만들어야 한다...
    customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    customPlot->axisRect()->setRangeDrag(Qt::Horizontal);
    customPlot->axisRect()->setRangeZoom(Qt::Horizontal);

    //레이아웃 설정
    auto layout = new QVBoxLayout(this);
    auto buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(autoModeBtn);

    layout->addLayout(buttonLayout);
    layout->addWidget(customPlot);

    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(0);
    customPlot->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    layout->addWidget(customPlot, /*stretch*/ 1); // 그래프가 공간 대부분 차지

    // 사용자 조작 시 자동모드 해제 + 버튼 상태 동기화
    auto disableAuto = [this](){
        userInteracted = true;
        if (autoModeBtn) autoModeBtn->setChecked(false);
    };

    // graph update 시그널
    connect(&dataMng, &dataManagement::dataChanged, this, &graphWidget::updateGraph);


    //만약에 마우스 휠과 버튼을 클릭했을 때 -> 그래프 자동 모드 해제 ( 자동 축 해제 )
    connect(customPlot, &QCustomPlot::mouseWheel, this, [=]{ disableAuto(); });
    connect(customPlot, &QCustomPlot::mousePress, this, [=]{ disableAuto(); });


    // 버튼 토글로 자동모드 on/off
    connect(autoModeBtn, &QToolButton::toggled, this, [&](bool on){
        userInteracted = !on;
        if (on) {
            // 켜질 때 즉시 꼬리로 스냅
            const double w = std::max(0.01, dataMng.getGraphWidth());
            const double lastTime = voltageGraphA->dataCount()
                                        ? voltageGraphA->dataMainKey(voltageGraphA->dataCount()-1) : 0.0;
            customPlot->xAxis->setRange(lastTime - w, lastTime);
            updateFixedTicks(lastTime - w, lastTime);
            customPlot->replot(QCustomPlot::rpQueuedReplot);
        }
    });
}

void graphWidget::updateFixedTicks(double rangeStart, double rangeEnd)
{
    // tick 위치는 항상 고정 (5등분)
    QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);

    const int numTicks = 6;  // 0, 1, 2, 3, 4, 5 (6개)
    const double width = rangeEnd - rangeStart;

    for (int i = 0; i < numTicks; i++) {
        double tickPos = rangeStart + (width * i / (numTicks - 1));
        double labelValue = rangeStart + (width * i / (numTicks - 1));

        textTicker->addTick(tickPos, QString::number(labelValue, 'f', 2));
    }

    customPlot->xAxis->setTicker(textTicker);
}

void graphWidget::updateGraph()
{
    auto s = dataMng.getLatestVoltageAndCurrent();
    double x = std::chrono::duration<double>(s.t).count();
    const double Fs = dataMng.getCyclePerSecond() * dataMng.getSamplePerCycle();
    const double xq = std::round(x * Fs) / Fs;  // 격자(Δt=1/Fs)로 반올림

    // --- 값 꺼내기 ---
    const double vA = s.vAVal, iA = s.cAVal;
    const double vB = s.vBVal, iB = s.cBVal;
    const double vC = s.vCVal, iC = s.cCVal;

    //qDebug() << vA << "," << vB << "," << vC << "," << iA << "," << iB << "," << iC;


    // --- 버퍼 적재 ---
    vABuffer.append({xq, vA});  cABuffer.append({xq, iA});
    vBBuffer.append({xq, vB});  cBBuffer.append({xq, iB});
    vCBuffer.append({xq, vC});  cCBuffer.append({xq, iC});

    // ===== 갱신 판정은 샘플 수로 =====
    sampleIndex_++;     // 전체 샘플 인덱스 증가 -> 나중에 쓰일 것?
    emitCounter_++;     // 이번 프레임 이후 누적

    const int spc = std::max(1, dataMng.getSamplePerCycle());
    bool shouldEmit = false;
    switch (dataMng.getUpdateMode()) {
    case updateMode::OneSample: shouldEmit = true; break;
    case updateMode::HalfCycle: { const int half = std::max(1, spc/2);
        if (emitCounter_ >= half) { shouldEmit = true; emitCounter_ = 0; } break; }
    case updateMode::FullCycle:
        if (emitCounter_ >= spc) { shouldEmit = true; emitCounter_ = 0; } break;
    }

    if (shouldEmit) {
        // --- 오래된 데이터 제거(최근 10초만) ---
        if (!vABuffer.isEmpty()) {
            const double rangeStart = vABuffer.last().first - 10.0;
            for (QCPGraph* g : {voltageGraphA, voltageGraphB, voltageGraphC,
                                currentGraphA, currentGraphB, currentGraphC}) {
                g->data()->removeBefore(rangeStart);
            }
        }

        // --- 버퍼 → 그래프 ---
        auto addAll = [](QCPGraph* g, const QVector<QPair<double,double>>& buf){
            for (const auto& p : buf) g->addData(p.first, p.second);
        };
        addAll(voltageGraphA, vABuffer); addAll(currentGraphA, cABuffer);
        addAll(voltageGraphB, vBBuffer); addAll(currentGraphB, cBBuffer);
        addAll(voltageGraphC, vCBuffer); addAll(currentGraphC, cCBuffer);

        // --- 축 범위 자동 스크롤 ---
        if (!userInteracted && !vABuffer.isEmpty()) {
            const double lastTime   = vABuffer.last().first;
            const double rangeStart = lastTime - dataMng.getGraphWidth();
            customPlot->xAxis->setRange(rangeStart, lastTime);
            updateFixedTicks(rangeStart, lastTime);
        }

        // --- 버퍼 비우고 리플롯 ---
        vABuffer.clear(); cABuffer.clear();
        vBBuffer.clear(); cBBuffer.clear();
        vCBuffer.clear(); cCBuffer.clear();
        customPlot->replot();
    }

    if (dataMng.getUpdateMode() == updateMode::OneSample) {
        vABuffer.clear(); cABuffer.clear();
        vBBuffer.clear(); cBBuffer.clear();
        vCBuffer.clear(); cCBuffer.clear();
    }
}

//graphWidget 저장된 값을 불러올 때 새로고침
void graphWidget::refreshFromData()
{
    customPlot->xAxis->setRange(0, dataMng.getGraphWidth());

    if (autoModeBtn) {
        autoModeBtn->setChecked(true);
        userInteracted = false;
    }

    customPlot->replot();
}

void graphWidget::setWaveVisible(const QString& key, bool visible)
{
    // 전압
    if (key.contains("전압_A")) voltageGraphA->setVisible(visible);
    else if (key.contains("전압_B")) voltageGraphB->setVisible(visible);
    else if (key.contains("전압_C")) voltageGraphC->setVisible(visible);

    // 전류
    else if (key.contains("전류_A")) currentGraphA->setVisible(visible);
    else if (key.contains("전류_B")) currentGraphB->setVisible(visible);
    else if (key.contains("전류_C")) currentGraphC->setVisible(visible);

    customPlot->replot(QCustomPlot::rpQueuedReplot);
}
