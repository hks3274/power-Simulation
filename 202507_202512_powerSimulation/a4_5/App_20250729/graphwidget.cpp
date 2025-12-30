#include "graphwidget.h"
#include<QBoxLayout>
#include <QDebug>
using namespace std::chrono;

graphWidget::graphWidget(dataManagement& dataMng)
    : dataMng{dataMng}
{
    //그래프설정버튼 생성
    graphSettingBtn = new QPushButton("그래프 설정");
    graphSettingBtn->setFixedSize(100, 40);

    //그래프 자동모드 되돌리기
    autoModeBtn = new QPushButton("자동모드");
    autoModeBtn->setFixedSize(100, 40);


    //그래프 생성
    customPlot = new QCustomPlot(this);

    // 그래프 추가
    graph = customPlot->addGraph();
    graph->setScatterStyle(QCPScatterStyle::ssCircle);  // 점 표시
    graph->setLineStyle(QCPGraph::lsLine);               // 선 연결

    // 축 라벨 설정
    customPlot->xAxis->setLabel("Time");
    customPlot->yAxis->setLabel("Value");

    // y축 범위 설정 (고정)
    customPlot->yAxis->setRange(-500, 500);

    //그래프 내 확대/이동 전부 허용 -> QChart의 경우 직접 함수를 만들어야 한다...
    customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    customPlot->axisRect()->setRangeDrag(Qt::Horizontal);
    customPlot->axisRect()->setRangeZoom(Qt::Horizontal);

    //레이아웃 설정
    auto layout = new QVBoxLayout(this);
    auto buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(graphSettingBtn);
    buttonLayout->addWidget(autoModeBtn);

    layout->addLayout(buttonLayout);
    layout->addWidget(customPlot);

    //그래프 설정 버튼을 누르면 모달이 생성
    settingDialog = std::make_unique<graphSetting>(dataMng, this); //자동으로 메모리를 delete해줌 -> 마스타 포인터 사용

    // graph update 시그널
    connect(&dataMng, &dataManagement::dataChanged, this, &graphWidget::updateGraph);

    //graphSettingModal열기
    connect(graphSettingBtn, &QPushButton::clicked, this, [this]() {
        settingDialog->exec();  // 모달 다이얼로그
    });


    //만약에 마우스 휠과 버튼을 클릭했을 때 -> 그래프 자동 모드 해제 ( 자동 축 해제 )
    connect(customPlot, &QCustomPlot::mouseWheel, this, [&] { userInteracted = true; });
    connect(customPlot, &QCustomPlot::mousePress, this, [&] { userInteracted = true; });

    //auto버튼 클릭시 다시 되돌아옴.
    connect(autoModeBtn, &QPushButton::clicked, this, [this]() {
        userInteracted = false;
    });

    //customplot 툴팁 활성화
    customPlot->setAttribute(Qt::WA_AlwaysShowToolTips, true);
}


void graphWidget::updateGraph()
{
    //간단하게 가공된 값을 불러와서 삽입하기만 하면 됨 -> data가공과 그래프 출력 부분 분리
    auto [xData, yData] = dataMng.getPlotDataInRangeSec(10.0);
    if (xData.isEmpty()) return;

    graph->setData(xData, yData);

    if (!userInteracted) {
        double width = dataMng.getGraphWidth();
        double maxTime = xData.last();
        customPlot->xAxis->setRange(maxTime - width, maxTime);
    }

    customPlot->replot();
}
