#include "graphwidget.h"
#include<QBoxLayout>

using namespace std::chrono_literals;

graphWidget::graphWidget(dataManagement& dataMng)
    : dataMng{dataMng}
{
    graphSettingBtn = new QPushButton();
    graphSettingBtn->setText("그래프 설정");
    graphSettingBtn->setFixedSize(100, 40);

    series = new QLineSeries();
    series->setPointsVisible(true);
    chart = new QChart();
    chart->addSeries(series);
    chart->createDefaultAxes();
    chart->setTitle("입력값 그래프");

    chartView = new QChartView(chart, this);
    chartView->setRenderHint(QPainter::Antialiasing);

    this->setMinimumSize(530,600);
    auto layout = new QVBoxLayout(this);


    auto buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();  // 왼쪽 공간 채우기
    buttonLayout->addWidget(graphSettingBtn); // 버튼을 오른쪽에 위치시킴

    layout->addLayout(buttonLayout);
    layout->addWidget(chartView);

    settingDialog = new graphSetting(dataMng);

    // graph update 시그널
    connect(&dataMng, &dataManagement::dataChanged, this, &graphWidget::updateGraph);

    //graphSettingModal열기
    connect(graphSettingBtn, &QPushButton::clicked, this, [this]() {
        settingDialog->exec();  // 모달 다이얼로그
    });

}

void graphWidget::updateGraph()
{
    auto data = dataMng.getData();
    if (data.empty()) return;

    // 1. 기준 시간 = 마지막 시간 - 폭
    auto latest_time = data.back().t;

    // 2. 사용자가 설정한 시간 폭 (ms)
    int width_ms = dataMng.getGraphWidth();  // 예: 5000 (5초)

    // 3. 보여줄 시간 범위 시작점
    auto start_time = latest_time - std::chrono::milliseconds(width_ms+1000);

    series->clear();

    for (const auto& point : data) {
        // 해당 구간에 들어오는 점만 추가
        if (point.t >= start_time) {
            auto d = point.t - start_time;
            double x = std::chrono::duration<double>(d).count();  // 상대 시간 (초)
            double y = static_cast<double>(point.value);
            series->append(x, y);
        }
    }

    // x축
    auto xAxes = chart->axes(Qt::Horizontal, series);
    if (!xAxes.isEmpty()) {
        xAxes[0]->setRange(0, width_ms / 1000);
    }

    // y축
    auto yAxes = chart->axes(Qt::Vertical, series);
    if (!yAxes.isEmpty()) {
        yAxes[0]->setRange(-500, 500);
    }

    chartView->update();
}
