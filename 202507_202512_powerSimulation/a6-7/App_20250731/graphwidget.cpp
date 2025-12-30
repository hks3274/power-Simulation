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

    // 전압그래프 추가
    voltageGraph = customPlot->addGraph();
    voltageGraph->setPen(QPen(Qt::blue));
    voltageGraph->setName("Voltage");
    voltageGraph->setScatterStyle(QCPScatterStyle::ssCircle);  // 점 표시
    voltageGraph->setLineStyle(QCPGraph::lsLine);               // 선 연결

    //전류그래프 추가
    currentGraph = customPlot->addGraph();
    currentGraph->setPen(QPen(Qt::red));
    currentGraph->setName("Voltage");
    currentGraph->setScatterStyle(QCPScatterStyle::ssCircle);  // 점 표시
    currentGraph->setLineStyle(QCPGraph::lsLine);               // 선 연결

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
    settingDialog = std::make_unique<graphSetting>(dataMng, this); //자동으로 메모리를 delete해줌 -> 스마트 포인터 사용

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

}


void graphWidget::updateGraph()
{
    auto [timeValuePair, currentVal] = dataMng.getLatestVoltageAndCurrent();
    double x = timeValuePair.first;
    double voltageVal = timeValuePair.second;

    // 현재 샘플을 버퍼에 저장 (항상)
    voltageBuffer.append({x, voltageVal});
    currentBuffer.append({x, currentVal});

    // 누적 시간 가져오기
    double& sumTime = dataMng.sumTime;
    int frequency = dataMng.getInputFreq();
    bool shouldEmit = false;

    dataMng.sumTime += dataMng.getTimeInterval();


    //화면 갱신 주기 설정
    switch (dataMng.getUpdateMode()) {
    case updateMode::OneSample:
        shouldEmit = true;
        break;
    case updateMode::HalfCycle:
        if (sumTime >= (500.0 / frequency)) {
            shouldEmit = true;
            sumTime = 0.0;
        }
        break;
    case updateMode::FullCycle:
        if (sumTime >= (1000.0 / frequency)) {
            shouldEmit = true;
            sumTime = 0.0;
        }
        break;
    }

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
            customPlot->xAxis->setRange(lastTime - dataMng.getGraphWidth(), lastTime);
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
