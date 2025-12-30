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

    // 전압그래프 추가
    voltageGraph = customPlot->addGraph(customPlot->xAxis, customPlot->yAxis);
    voltageGraph->setPen(QPen(Qt::blue));
    voltageGraph->setName("Voltage");
    voltageGraph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 4));  // 점 표시
    voltageGraph->setLineStyle(QCPGraph::lsLine);               // 선 연결

    //전류그래프 추가
    currentGraph = customPlot->addGraph(customPlot->xAxis, customPlot->yAxis2);
    currentGraph->setPen(QPen(Qt::red));
    currentGraph->setName("Current");
    currentGraph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 4));  // 점 표시
    currentGraph->setLineStyle(QCPGraph::lsLine);               // 선 연결

    // 축 라벨 설정
    customPlot->xAxis->setLabel("Time");
    customPlot->yAxis->setLabel("Voltage");
    customPlot->yAxis2->setLabel("Current");
    customPlot->axisRect()->setupFullAxesBox();


    customPlot->yAxis2->setVisible(true);
    customPlot->yAxis2->setTickLabels(true);

    // y축 범위 설정 (고정)
    customPlot->yAxis->setRange(-1000, 1000);
    customPlot->replot(QCustomPlot::rpQueuedReplot);

    customPlot->yAxis2->setRange(-50,50);
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
            const double lastTime = voltageGraph->dataCount()
                                        ? voltageGraph->dataMainKey(voltageGraph->dataCount()-1) : 0.0;
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
    auto [timeValuePair, currentVal] = dataMng.getLatestVoltageAndCurrent();
    double x = timeValuePair.first;
    const double Fs = dataMng.getCyclePerSecond() * dataMng.getSamplePerCycle();
    const double xq = std::round(x * Fs) / Fs;  // 격자(Δt=1/Fs)로 반올림

    double voltageVal = timeValuePair.second;

    // 현재 샘플을 버퍼에 저장 (항상)
    voltageBuffer.append({xq, voltageVal});
    currentBuffer.append({xq, currentVal});

    // ===== 갱신 판정은 샘플 수로 =====
    sampleIndex_++;     // 전체 샘플 인덱스 증가 -> 나중에 쓰일 것?
    emitCounter_++;     // 이번 프레임 이후 누적

    const int spc = std::max(1, dataMng.getSamplePerCycle());
    bool shouldEmit = false;

    switch (dataMng.getUpdateMode()) {
    case updateMode::OneSample:
        shouldEmit = true;
        break;

    case updateMode::HalfCycle: {
        const int half = std::max(1, spc / 2);
        if (emitCounter_ >= half) {
            shouldEmit = true;
            emitCounter_ = 0;
        }
        break;
    }

    case updateMode::FullCycle:
        if (emitCounter_ >= spc) {
            shouldEmit = true;
            emitCounter_ = 0;
        }
        break;
    }
    // ==========

    if (shouldEmit) {
        // 오래된 데이터 제거 (현재 시간 기준 10초 전)
        if (!voltageBuffer.isEmpty()) {
            double rangeStart = voltageBuffer.last().first - 10.0;
            voltageGraph->data()->removeBefore(rangeStart);
            currentGraph->data()->removeBefore(rangeStart);
        }

        // 버퍼에 있는 모든 샘플을 그래프에 추가
        for (const auto& data : std::as_const(voltageBuffer)) {
            voltageGraph->addData(data.first, data.second);
        }
        for (const auto& data : std::as_const(currentBuffer)) {
            currentGraph->addData(data.first, data.second);
        }

        // 축 범위 업데이트 (자동 모드일 때만)
        if (!userInteracted && !voltageBuffer.isEmpty()) {
            double lastTime = voltageBuffer.last().first;
            double rangeStart = lastTime - dataMng.getGraphWidth();
            customPlot->xAxis->setRange(rangeStart, lastTime);

            // tick 라벨만 업데이트 (위치는 고정)
            updateFixedTicks(rangeStart, lastTime);
        }

        // 버퍼 클리어
        voltageBuffer.clear();
        currentBuffer.clear();

        // 화면 업데이트
        customPlot->replot();
    }

    // OneSample 모드에서는 즉시 버퍼 클리어 (중복 방지)
    if (dataMng.getUpdateMode() == updateMode::OneSample) {
        voltageBuffer.clear();
        currentBuffer.clear();
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
