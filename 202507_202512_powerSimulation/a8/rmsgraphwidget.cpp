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

    plotVI = new QCustomPlot(this);
    plotP = new QCustomPlot(this);

    //VIPlot setup
    //좌: V, 우: I
    plotVI->yAxis->setLabel("Vrms (V)");
    plotVI->yAxis2->setVisible(true);
    plotVI->yAxis2->setLabel("Irms (A)");

    //공통 X축
    plotVI->xAxis->setLabel("Time (s)");
    plotVI->axisRect()->setupFullAxesBox(); //x축과 y축을 모두 감싸는 기본 좌표축 박스를 자동으로 만들어줌
    plotVI->yAxis2->setTickLabels(true); //오른쪽 틱 보이게 하기
    //plotVI->xAxis->grid()->setVisible(true); //X축 격자
    //plotVI->yAxis->grid()->setVisible(true); //Y축 격자
    //plotVI->legend->setVisible(true);  //범례 표시

    //데이터 라인
    gVrms = plotVI->addGraph(plotVI->xAxis, plotVI->yAxis); // 왼쪽 축
    gVrms->setPen(QPen(Qt::blue));
    gVrms->setScatterStyle(QCPScatterStyle::ssCircle);  // 점 표시
    gVrms->setLineStyle(QCPGraph::lsLine);               // 선 연결
    gVrms->setName("Voltage RMS");

    gIrms = plotVI->addGraph(plotVI->xAxis, plotVI->yAxis2); //오른쪽 축
    gIrms->setPen(QPen(Qt::red));
    gIrms->setScatterStyle(QCPScatterStyle::ssCircle);  // 점 표시
    gIrms->setLineStyle(QCPGraph::lsLine);               // 선 연결
    gIrms->setName("Current RMS");


    plotVI->yAxis->setRange(-500, 500);
    plotVI->replot(QCustomPlot::rpQueuedReplot);

    plotVI->yAxis2->setRange(-20, 20);
    plotVI->replot(QCustomPlot::rpQueuedReplot);

    // 마우스 인터랙션: 드래그/줌 on
    plotVI->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

    // X축만 드래그/줌 가능 (세로는 잠금)
    plotVI->axisRect()->setRangeDrag(Qt::Horizontal);
    plotVI->axisRect()->setRangeZoom(Qt::Horizontal);
    plotVI->axisRect()->setRangeDragAxes(plotVI->xAxis, /*vertical*/nullptr);
    plotVI->axisRect()->setRangeZoomAxes(plotVI->xAxis, /*vertical*/nullptr);


    //PPlotSetup
    plotP->yAxis->setLabel("Pavg (W) ");
    plotP->xAxis->setLabel("Time (s)");
    plotP->axisRect()->setupFullAxesBox();
    plotP->legend->setVisible(false);


    gPavg = plotP->addGraph(); //좌측
    gPavg->setScatterStyle(QCPScatterStyle::ssCircle);  // 점 표시
    gPavg->setLineStyle(QCPGraph::lsLine);
    gPavg->setName("Pavg");

    plotP->yAxis->setRange(-10000, 10000);
    plotP->replot(QCustomPlot::rpQueuedReplot);

    // 마우스 인터랙션: 드래그/줌 on
    plotP->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

    plotP->axisRect()->setRangeDrag(Qt::Horizontal);
    plotP->axisRect()->setRangeZoom(Qt::Horizontal);
    plotP->axisRect()->setRangeDragAxes(plotP->xAxis, nullptr);
    plotP->axisRect()->setRangeZoomAxes(plotP->xAxis, nullptr);

    // 좌/우/하단 마진을 동일 그룹으로 묶기 -> 마진을 동일하게 줌
    auto *mg = new QCPMarginGroup(plotVI);
    plotVI->axisRect()->setMarginGroup(QCP::msLeft | QCP::msRight | QCP::msBottom, mg);
    plotP->axisRect()->setMarginGroup(QCP::msLeft | QCP::msRight | QCP::msBottom, mg);


    // 배치 (위: VI, 아래: P)
    auto buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(autoModeBtn);
    vl->addLayout(buttonLayout);
    vl->addWidget(plotVI, /*stretch*/1);
    vl->addWidget(plotP,  /*stretch*/1);
    setLayout(vl);

    // 사용자 조작 시 자동모드 해제 + 버튼 상태 동기화
    auto disableAuto = [this](){
        m_followTail = false;
        if (autoModeBtn) autoModeBtn->setChecked(false);
    };

    // VI/P 어느 쪽이든 조작하면 오토팔로우 off
    connect(plotVI, &QCustomPlot::mousePress,  this, [=](QMouseEvent*){ disableAuto(); });
    connect(plotVI, &QCustomPlot::mouseWheel,  this, [=](QWheelEvent*){ disableAuto(); });
    connect(plotP,  &QCustomPlot::mousePress,  this, [=](QMouseEvent*){ disableAuto(); });
    connect(plotP,  &QCustomPlot::mouseWheel,  this, [=](QWheelEvent*){ disableAuto(); });

    // 버튼 토글로 자동모드 on/off
    connect(autoModeBtn, &QToolButton::toggled, this, [&](bool on){
        m_followTail = on;
        if (on) {
            const double w = std::max(0.01, dataMng.getGraphWidth());
            // m_lastT 가 초기값(-1)일 수 있으니 방어
            const double right = (m_lastT >= 0.0) ? m_lastT : w;
            const double left  = std::max(0.0, right - w);

            plotVI->xAxis->setRange(left, right);
            plotP->xAxis->setRange(left, right);
            plotVI->replot(QCustomPlot::rpQueuedReplot);
            plotP->replot(QCustomPlot::rpQueuedReplot);
        }
    });

    // (선택) 두 플롯 X축 동기화
    connect(plotVI->xAxis, SIGNAL(rangeChanged(QCPRange)), plotP->xAxis,  SLOT(setRange(QCPRange)));
    connect(plotP->xAxis,  SIGNAL(rangeChanged(QCPRange)), plotVI->xAxis, SLOT(setRange(QCPRange)));

    connect(&dataMng, &dataManagement::rmsDataChanged,
            this, &rmsGraphWidget::updateData);

}


void rmsGraphWidget::updateData(dataManagement::measure_data m)
{
    const double tSec = duration<double>(m.t).count();

    // 0) 현재 설정
    const double width   = std::max(0.01, dataMng.getGraphWidth()); // 표시 폭
    const double keepSec = std::max(visibleSec, width);             // 보관 길이(=최소 보관시간)

    // 되감김
    if (m_lastT >= 0.0 && tSec < m_lastT) {
        gVrms->data()->clear();
        gIrms->data()->clear();
        gPavg->data()->clear();
        plotVI->xAxis->setRange(0.0, width);  // ← 초기 표시도 그래프 폭에 맞춤
        plotP->xAxis->setRange(0.0, width);
        m_lastT = -1.0;
    }

    if (m_lastT >= 0.0 && tSec == m_lastT) return;

    // 추가
    gVrms->addData(tSec, m.voltageRMSVal);
    gIrms->addData(tSec, m.currentRMSVal);
    gPavg->addData(tSec, m.powerVal);
    m_lastT = tSec;

    // 보관 길이만큼만 유지
    const double leftKeep = std::max(0.0, m_lastT - keepSec);
    gVrms->data()->removeBefore(leftKeep);
    gIrms->data()->removeBefore(leftKeep);
    gPavg->data()->removeBefore(leftKeep);

    // 화면에는 '그래프 폭'만큼만 보여줌 (우측 정렬)
    const double xLeft = std::max(0.0, m_lastT - width);

    // 오토팔로우일 때만 setRange로 덮어쓴다
    if (m_followTail) {
        plotVI->xAxis->setRange(xLeft, m_lastT);
        plotP->xAxis->setRange(xLeft, m_lastT);
    }

    plotVI->replot(QCustomPlot::rpQueuedReplot);
    plotP->replot(QCustomPlot::rpQueuedReplot);
}

void rmsGraphWidget::refreshFromData()
{
    // 그래프 폭 다시 적용
    const double w = std::max(0.01, dataMng.getGraphWidth());
    double right = (m_lastT >= 0.0) ? m_lastT : w;
    double left  = std::max(0.0, right - w);

    plotVI->xAxis->setRange(left, right);
    plotP->xAxis->setRange(left, right);

    // 자동 모드 버튼 초기화
    if (autoModeBtn) {
        autoModeBtn->setChecked(true);
        m_followTail = true;
    }

    plotVI->replot(QCustomPlot::rpQueuedReplot);
    plotP->replot(QCustomPlot::rpQueuedReplot);
}
