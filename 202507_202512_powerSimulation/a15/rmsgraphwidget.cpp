#include "rmsgraphwidget.h"

using namespace std::chrono;

rmsGraphWidget::rmsGraphWidget(dataManagement& dataMng)
    : dataMng{dataMng}
{

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

    //우선 따로 따로 그래프 V,I => 1개 플롯 P=> 1개 플롯
    auto * vl = new QVBoxLayout(this);

    plotVIP = new QCustomPlot(this);
    plotFundVI = new QCustomPlot(this);
    plotHarmonicVI = new QCustomPlot(this);

    // VIP Plot 제목
    plotVIP->plotLayout()->insertRow(0);  // 레이아웃 맨 위에 행 추가
    QCPTextElement *titleVIP = new QCPTextElement(plotVIP, "전체 RMS (V, A, W)", QFont("맑은 고딕", 10, QFont::Bold));
    plotVIP->plotLayout()->addElement(0, 0, titleVIP);

    // Fund Plot 제목
    plotFundVI->plotLayout()->insertRow(0);
    QCPTextElement *titleFund = new QCPTextElement(plotFundVI, "기본파 RMS (V, A)", QFont("맑은 고딕", 10, QFont::Bold));
    plotFundVI->plotLayout()->addElement(0, 0, titleFund);

    // Harmonic Plot 제목
    plotHarmonicVI->plotLayout()->insertRow(0);
    QCPTextElement *titleHarm = new QCPTextElement(plotHarmonicVI, "고조파 RMS (V, A)", QFont("맑은 고딕", 10, QFont::Bold));
    plotHarmonicVI->plotLayout()->addElement(0, 0, titleHarm);


    //VIP Plot setup
    //좌: V, 우: I
    plotVIP->yAxis->setLabel("V total rms (V)");
    plotVIP->yAxis2->setVisible(true);
    plotVIP->yAxis2->setLabel("I total rms (A)");


    QCPAxis* yAxisP = plotVIP->axisRect()->addAxis(QCPAxis::atRight);
    yAxisP->setLabel("total Pavg (W)");
    yAxisP->setOffset(50);              // 기존 yAxis2 바깥으로 100px 정도 띄우기
    yAxisP->setRange(-5000, 5000);    // 전력 범위(예시)

    // 범례 지정
    plotVIP->legend->setVisible(true);
    plotVIP->legend->setFont(QFont("맑은 고딕", 7));     // 폰트 크기 ↓
    plotVIP->legend->setIconSize(QSize(6, 6));          // 심볼 크기 ↓

    //공통 X축
    plotVIP->xAxis->setLabel("Time (s)");
    plotVIP->axisRect()->setupFullAxesBox(); //x축과 y축을 모두 감싸는 기본 좌표축 박스를 자동으로 만들어줌
    plotVIP->yAxis2->setTickLabels(true);

    //데이터 라인
    gVrms = plotVIP->addGraph(plotVIP->xAxis, plotVIP->yAxis); // 왼쪽 축
    gVrms->setPen(QPen(Qt::blue));
    gVrms->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 4));  // 점 표시
    gVrms->setLineStyle(QCPGraph::lsLine);               // 선 연결
    gVrms->setName("V Total RMS");

    gIrms = plotVIP->addGraph(plotVIP->xAxis, plotVIP->yAxis2); //오른쪽 축
    gIrms->setPen(QPen(Qt::red));
    gIrms->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 4));  // 점 표시
    gIrms->setLineStyle(QCPGraph::lsLine);               // 선 연결
    gIrms->setName("I Total RMS");

    gPavg = plotVIP->addGraph(plotVIP->xAxis, yAxisP);
    gPavg->setPen(QPen(Qt::darkGreen));
    gPavg->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 4));
    gPavg->setLineStyle(QCPGraph::lsLine);
    gPavg->setName("Pavg");


    plotVIP->yAxis->setRange(0, 500);
    plotVIP->replot(QCustomPlot::rpQueuedReplot);

    plotVIP->yAxis2->setRange(0, 20);
    plotVIP->replot(QCustomPlot::rpQueuedReplot);

    // 마우스 인터랙션: 드래그/줌 on
    plotVIP->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

    // X축만 드래그/줌 가능 (세로는 잠금)
    plotVIP->axisRect()->setRangeZoom(Qt::Horizontal);
    plotVIP->axisRect()->setRangeDragAxes(plotVIP->xAxis, /*vertical*/nullptr);
    plotVIP->axisRect()->setRangeZoomAxes(plotVIP->xAxis, /*vertical*/nullptr);


    //PlotSetup
    plotFundVI->yAxis->setLabel(" fund V rms (V) ");
    plotFundVI->yAxis2->setVisible(true);
    plotFundVI->yAxis2->setLabel(" fund I rms (A) ");
    plotFundVI->xAxis->setLabel("Time (s)");
    plotFundVI->axisRect()->setupFullAxesBox();
    plotFundVI->yAxis2->setTickLabels(true);


    // 범례지정
    plotFundVI->legend->setVisible(true);
    plotFundVI->legend->setFont(QFont("맑은 고딕", 7));
    plotFundVI->legend->setIconSize(QSize(6, 6));


    fundVrms = plotFundVI->addGraph(plotFundVI->xAxis, plotFundVI->yAxis); // 왼쪽 축
    fundVrms->setPen(QPen(Qt::blue));
    fundVrms->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 4));  // 점 표시
    fundVrms->setLineStyle(QCPGraph::lsLine);               // 선 연결
    fundVrms->setName("V Fund RMS");

    fundCrms = plotFundVI->addGraph(plotFundVI->xAxis, plotFundVI->yAxis2); //오른쪽 축
    fundCrms->setPen(QPen(Qt::red));
    fundCrms->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 4));  // 점 표시
    fundCrms->setLineStyle(QCPGraph::lsLine);               // 선 연결
    fundCrms->setName("I Fund RMS");

    plotFundVI->yAxis->setRange(0, 500);
    plotFundVI->replot(QCustomPlot::rpQueuedReplot);

    plotFundVI->yAxis2->setRange(0, 20);
    plotFundVI->replot(QCustomPlot::rpQueuedReplot);


    // 마우스 인터랙션: 드래그/줌 on
    plotFundVI->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

    plotFundVI->axisRect()->setRangeDrag(Qt::Horizontal);
    plotFundVI->axisRect()->setRangeZoom(Qt::Horizontal);
    plotFundVI->axisRect()->setRangeDragAxes(plotFundVI->xAxis, nullptr);
    plotFundVI->axisRect()->setRangeZoomAxes(plotFundVI->xAxis, nullptr);


    //PlotSetup
    plotHarmonicVI->yAxis->setLabel(" Harm V rms (V) ");
    plotHarmonicVI->yAxis2->setVisible(true);
    plotHarmonicVI->yAxis2->setLabel(" Harm I rms (A) ");
    plotHarmonicVI->xAxis->setLabel("Time (s)");
    plotHarmonicVI->axisRect()->setupFullAxesBox();
    plotHarmonicVI->yAxis2->setTickLabels(true);


    // 범례지정
    plotHarmonicVI->legend->setVisible(true);
    plotHarmonicVI->legend->setFont(QFont("맑은 고딕", 7));
    plotHarmonicVI->legend->setIconSize(QSize(6, 6));


    harmonicVrms = plotHarmonicVI->addGraph(plotHarmonicVI->xAxis, plotHarmonicVI->yAxis); // 왼쪽 축
    harmonicVrms->setPen(QPen(Qt::blue));
    harmonicVrms->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 4));  // 점 표시
    harmonicVrms->setLineStyle(QCPGraph::lsLine);               // 선 연결
    harmonicVrms->setName("V Harm RMS");

    harmonicCrms = plotHarmonicVI->addGraph(plotHarmonicVI->xAxis, plotHarmonicVI->yAxis2); //오른쪽 축
    harmonicCrms->setPen(QPen(Qt::red));
    harmonicCrms->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 4));  // 점 표시
    harmonicCrms->setLineStyle(QCPGraph::lsLine);               // 선 연결
    harmonicCrms->setName("I Harm RMS");

    plotHarmonicVI->yAxis->setRange(0, 100);
    plotHarmonicVI->replot(QCustomPlot::rpQueuedReplot);

    plotHarmonicVI->yAxis2->setRange(0, 20);
    plotHarmonicVI->replot(QCustomPlot::rpQueuedReplot);


    // 마우스 인터랙션: 드래그/줌 on
    plotHarmonicVI->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

    plotHarmonicVI->axisRect()->setRangeDrag(Qt::Horizontal);
    plotHarmonicVI->axisRect()->setRangeZoom(Qt::Horizontal);
    plotHarmonicVI->axisRect()->setRangeDragAxes(plotHarmonicVI->xAxis, nullptr);
    plotHarmonicVI->axisRect()->setRangeZoomAxes(plotHarmonicVI->xAxis, nullptr);


    // 좌/우/하단 마진을 동일 그룹으로 묶기 -> 마진을 동일하게 줌
    auto *mg = new QCPMarginGroup(plotVIP);
    plotVIP->axisRect()->setMarginGroup(QCP::msLeft | QCP::msRight | QCP::msBottom, mg);
    plotFundVI->axisRect()->setMarginGroup(QCP::msLeft | QCP::msRight | QCP::msBottom, mg);
    plotHarmonicVI->axisRect()->setMarginGroup(QCP::msLeft | QCP::msRight | QCP::msBottom, mg);


    // 배치 (위: VI, 아래: P)
    auto buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(autoModeBtn);
    vl->addLayout(buttonLayout);
    vl->addWidget(plotVIP, /*stretch*/1);
    vl->addWidget(plotFundVI,  /*stretch*/1);
    vl->addWidget(plotHarmonicVI,  /*stretch*/1);
    setLayout(vl);

    // 사용자 조작 시 자동모드 해제 + 버튼 상태 동기화
    auto disableAuto = [this](){
        m_followTail = false;
        if (autoModeBtn) autoModeBtn->setChecked(false);
    };

    // VI/P 어느 쪽이든 조작하면 오토팔로우 off
    connect(plotVIP, &QCustomPlot::mousePress,  this, [=](QMouseEvent*){ disableAuto(); });
    connect(plotVIP, &QCustomPlot::mouseWheel,  this, [=](QWheelEvent*){ disableAuto(); });
    connect(plotFundVI,  &QCustomPlot::mousePress,  this, [=](QMouseEvent*){ disableAuto(); });
    connect(plotFundVI,  &QCustomPlot::mouseWheel,  this, [=](QWheelEvent*){ disableAuto(); });
    connect(plotHarmonicVI,  &QCustomPlot::mousePress,  this, [=](QMouseEvent*){ disableAuto(); });
    connect(plotHarmonicVI,  &QCustomPlot::mouseWheel,  this, [=](QWheelEvent*){ disableAuto(); });

    // 버튼 토글로 자동모드 on/off
    connect(autoModeBtn, &QToolButton::toggled, this, [&](bool on){
        m_followTail = on;
        if (on) {
            const double w = std::max(0.01, dataMng.getGraphWidth());
            // m_lastT 가 초기값(-1)일 수 있으니 방어
            const double right = (m_lastT >= 0.0) ? m_lastT : w;
            const double left  = std::max(0.0, right - w);

            plotVIP->xAxis->setRange(left, right);
            plotFundVI->xAxis->setRange(left, right);
            plotHarmonicVI->xAxis->setRange(left, right);

            updateFixedTicks(left, right);

            plotVIP->replot(QCustomPlot::rpQueuedReplot);
            plotFundVI->replot(QCustomPlot::rpQueuedReplot);
            plotHarmonicVI->replot(QCustomPlot::rpQueuedReplot);
        }
    });

    // (선택) 세 플롯 X축 동기화
    connect(plotVIP->xAxis, SIGNAL(rangeChanged(QCPRange)), plotFundVI->xAxis,  SLOT(setRange(QCPRange)));
    connect(plotVIP->xAxis,  SIGNAL(rangeChanged(QCPRange)), plotHarmonicVI->xAxis, SLOT(setRange(QCPRange)));
    connect(plotFundVI->xAxis,  SIGNAL(rangeChanged(QCPRange)), plotVIP->xAxis, SLOT(setRange(QCPRange)));
    connect(plotFundVI->xAxis,  SIGNAL(rangeChanged(QCPRange)), plotHarmonicVI->xAxis, SLOT(setRange(QCPRange)));
    connect(plotHarmonicVI->xAxis,  SIGNAL(rangeChanged(QCPRange)), plotVIP->xAxis, SLOT(setRange(QCPRange)));
    connect(plotHarmonicVI->xAxis,  SIGNAL(rangeChanged(QCPRange)), plotFundVI->xAxis, SLOT(setRange(QCPRange)));

    connect(&dataMng, &dataManagement::rmsDataChanged,
            this, &rmsGraphWidget::updateData);

}

void rmsGraphWidget::updateFixedTicks(double rangeStart, double rangeEnd)
{
    // tick 위치는 항상 고정 (6개: 5등분)
    QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);

    const int numTicks = 6;
    const double width = rangeEnd - rangeStart;

    for (int i = 0; i < numTicks; i++) {
        double tickPos = rangeStart + (width * i / (numTicks - 1));
        double labelValue = rangeStart + (width * i / (numTicks - 1));

        textTicker->addTick(tickPos, QString::number(labelValue, 'f', 2));
    }

    // 세 플롯 모두에 적용
    plotVIP->xAxis->setTicker(textTicker);

    QSharedPointer<QCPAxisTickerText> textTicker2(new QCPAxisTickerText);
    for (int i = 0; i < numTicks; i++) {
        double tickPos = rangeStart + (width * i / (numTicks - 1));
        double labelValue = rangeStart + (width * i / (numTicks - 1));
        textTicker2->addTick(tickPos, QString::number(labelValue, 'f', 2));
    }
    plotFundVI->xAxis->setTicker(textTicker2);

    QSharedPointer<QCPAxisTickerText> textTicker3(new QCPAxisTickerText);
    for (int i = 0; i < numTicks; i++) {
        double tickPos = rangeStart + (width * i / (numTicks - 1));
        double labelValue = rangeStart + (width * i / (numTicks - 1));
        textTicker3->addTick(tickPos, QString::number(labelValue, 'f', 2));
    }
    plotHarmonicVI->xAxis->setTicker(textTicker3);
}

void rmsGraphWidget::updateData(dataManagement::measure_data m)
{
    const double tSec = duration<double>(m.t).count();

    // 현재 설정
    const double width   = std::max(0.01, dataMng.getGraphWidth()); // 표시 폭
    const double keepSec = std::max(visibleSec, width);             // 보관 길이(=최소 보관시간)

    // 되감김
    if (m_lastT >= 0.0 && tSec < m_lastT) {
        gVrms->data()->clear();
        gIrms->data()->clear();
        fundVrms->data()->clear();
        fundCrms->data()->clear();
        harmonicVrms->data()->clear();
        harmonicCrms->data()->clear();
        gPavg->data()->clear();

        plotVIP->xAxis->setRange(0.0, width);
        plotFundVI->xAxis->setRange(0.0, width);
        plotHarmonicVI->xAxis->setRange(0.0, width);
        updateFixedTicks(0.0, width);
        m_lastT = -1.0;
    }

    if (m_lastT >= 0.0 && tSec == m_lastT) return;

    // 추가
    gVrms->addData(tSec, m.voltageRMSVal);
    gIrms->addData(tSec, m.currentRMSVal);
    gPavg->addData(tSec, m.powerVal);
    fundVrms->addData(tSec, m.vFundRMSVal);
    fundCrms->addData(tSec, m.cFundRMSVal);
    harmonicVrms->addData(tSec, m.vHarmonicRMSVal);
    harmonicCrms->addData(tSec, m.cHarmonicRMSVal);
    m_lastT = tSec;

    // 보관 길이만큼만 유지
    const double leftKeep = std::max(0.0, m_lastT - keepSec);
    gVrms->data()->removeBefore(leftKeep);
    gIrms->data()->removeBefore(leftKeep);
    fundVrms->data()->removeBefore(leftKeep);
    fundCrms->data()->removeBefore(leftKeep);
    harmonicVrms->data()->removeBefore(leftKeep);
    harmonicCrms->data()->removeBefore(leftKeep);
    gPavg->data()->removeBefore(leftKeep);

    // 화면에는 '그래프 폭'만큼만 보여줌 (우측 정렬)
    const double xLeft = std::max(0.0, m_lastT - width);

    // 오토팔로우일 때만 setRange로 덮어쓴다
    if (m_followTail) {
        plotVIP->xAxis->setRange(xLeft, m_lastT);
        plotFundVI->xAxis->setRange(xLeft, m_lastT);
        plotHarmonicVI->xAxis->setRange(xLeft, m_lastT);

        // tick 라벨만 업데이트
        updateFixedTicks(xLeft, m_lastT);
    }

    plotVIP->replot(QCustomPlot::rpQueuedReplot);
    plotFundVI->replot(QCustomPlot::rpQueuedReplot);
    plotHarmonicVI->replot(QCustomPlot::rpQueuedReplot);
}

void rmsGraphWidget::refreshFromData()
{
    // 그래프 폭 다시 적용
    const double w = std::max(0.01, dataMng.getGraphWidth());
    double right = (m_lastT >= 0.0) ? m_lastT : w;
    double left  = std::max(0.0, right - w);

    plotVIP->xAxis->setRange(left, right);
    plotFundVI->xAxis->setRange(left, right);
    plotHarmonicVI->xAxis->setRange(left, right);

    updateFixedTicks(left, right);

    // 자동 모드 버튼 초기화
    if (autoModeBtn) {
        autoModeBtn->setChecked(true);
        m_followTail = true;
    }

    plotVIP->replot(QCustomPlot::rpQueuedReplot);
    plotFundVI->replot(QCustomPlot::rpQueuedReplot);
    plotHarmonicVI->replot(QCustomPlot::rpQueuedReplot);
}
