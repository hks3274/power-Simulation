#include "graphwidget.h"
#include<QBoxLayout>
#include <QDebug>
#include <chrono>
using namespace std::chrono;

graphWidget::graphWidget(dataManagement& dataMng)
    : dataMng{dataMng}
{
    //그래프설정버튼 생성
    graphSettingBtn = new QPushButton("그래프 설정");
    graphSettingBtn->setFixedSize(100, 40);

    //그래프 라이브모드 되돌리기
    liveViewBtn = new QPushButton("라이브 보기");
    liveViewBtn->setFixedSize(100, 40);


    //그래프 생성
    customPlot = new QCustomPlot(this);

    // 그래프 추가
    graph = customPlot->addGraph();
    graph->setScatterStyle(QCPScatterStyle::ssCircle);  // 점 표시
    graph->setLineStyle(QCPGraph::lsLine);               // 선 연결

    // lineGraph = customPlot->addGraph();
    // lineGraph->setLineStyle(QCPGraph::lsLine);
    // lineGraph->setScatterStyle(QCPScatterStyle::ssNone); // 선만

    // pointGraph = customPlot->addGraph();
    // pointGraph->setLineStyle(QCPGraph::lsNone);
    // pointGraph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, Qt::red, 6)); // 점만

    // 축 라벨 설정
    customPlot->xAxis->setLabel("Time");
    customPlot->yAxis->setLabel("Value");

    // y축 범위 설정 (고정)
    customPlot->yAxis->setRange(-500, 500);

    //그래프 내 확대/이동 전부 허용
    customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    customPlot->axisRect()->setRangeDrag(Qt::Horizontal);
    customPlot->axisRect()->setRangeZoom(Qt::Horizontal);

    //안티앨리어싱 끄기(빠름)
    customPlot->setNotAntialiasedElements(QCP::aeAll);


    //레이아웃 설정
    auto layout = new QVBoxLayout(this);
    auto buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(graphSettingBtn);
    buttonLayout->addWidget(liveViewBtn);

    layout->addLayout(buttonLayout);
    layout->addWidget(customPlot);

    //그래프 설정 버튼을 누르면 모달이 생성
    settingDialog = new graphSetting(dataMng);

    // graph update 시그널
    connect(&dataMng, &dataManagement::dataChanged, this, &graphWidget::updateGraph);

    //graphSettingModal열기
    connect(graphSettingBtn, &QPushButton::clicked, this, [this]() {
        settingDialog->exec();  // 모달 다이얼로그
    });


    //만약에 마우스 휠과 버튼을 클릭했을 때 -> 그래프 라이브 모드 해제 ( 자동 축 해제 )
    connect(customPlot, &QCustomPlot::mouseWheel, this, [&] { userInteracted = true; });
    connect(customPlot, &QCustomPlot::mousePress, this, [&] { userInteracted = true; });

    //liveView버튼 클릭시 다시 되돌아옴.
    connect(liveViewBtn, &QPushButton::clicked, this, [this]() {
        userInteracted = false;
    });

}


void graphWidget::updateGraph()
{
    const auto& data = dataMng.getData();
    if (data.empty()) return;

    const auto& pt = data.back(); // 가장 최근 값만 추가

    double timeInSec = duration_cast<duration<double>>(pt.t).count(); //nanoseconds를 그냥 초단위로 변환

    //그래프에 새로운 데이터 포인트 추가
    graph->addData(timeInSec, pt.value);

    //오래된 데이터 제거 (시간 기준)
    double maxRange = 5.0; // 5초
    graph->data()->removeBefore(timeInSec - maxRange);

    // 선용 그래프에 전체 흐름 추가
    // lineGraph->addData(pt.t, pt.value);
    // lineGraph->data()->removeBefore(pt.t - 10.0); // 10초 범위 유지

    // // 점용 그래프: 최근 N개 점만 표시
    // const int maxPoints = 30;
    // pointGraph->data()->clear();
    // int count = std::min((int)data.size(), maxPoints);
    // for (int i = data.size() - count; i < data.size(); ++i) {
    //     pointGraph->addData(data[i].t, data[i].value);
    // }


    // 자동 축 이동 (단, 사용자가 조작하지 않았을 때만)
    if (!userInteracted) {
        double width_ms = dataMng.getGraphWidth();
        customPlot->xAxis->setRange(timeInSec - width_ms, timeInSec);
    }

    customPlot->replot(); // 그래프 갱신
}
