#include "graphwidget.h"
#include<QBoxLayout>
#include <QDateTime>


using namespace std::chrono_literals;

graphWidget::graphWidget(dataManagement& dataMng)
    : dataMng{dataMng}
{
    graphSettingBtn = new QPushButton("그래프 설정");
    graphSettingBtn->setFixedSize(100, 40);

    series = new QLineSeries();
    series->setPointsVisible(true);

    chart = new QChart();
    chart->setTitle("입력값 그래프");

    chartView = new QChartView(chart, this);
    chartView->setRenderHint(QPainter::Antialiasing);

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

    setupChart();

}


void graphWidget::setupChart()
{
    chart->addSeries(series);

    //x축 설정
    axisX = new QValueAxis();
    axisX->setLabelFormat("%.2f");
    axisX->setTitleText("Time");
    axisX->setTickCount(6);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    //y축 설정
    axisY = new QValueAxis();
    axisY->setRange(-500, 500);
    axisY->setTitleText("Value");
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);
}

void graphWidget::updateGraph()
{
    auto data = dataMng.getData();

    //2차원좌표를 저장할 수 있는 리스트(벡터), pointF=> 부동소수점(double) 좌표를 가지는 점(x,y)를 표현
    QVector<QPointF> points;
    if (data.empty()) return;

    //최신 데이터 (가장 마지막 값)를 가져온다
    const auto& point = data.back();

    //시리즈에 새 값 추가
    //한번에 데이터를 points 벡터에 넣어버림
    for(const auto& pt : data) {
        points.append(QPointF(pt.t, pt.value));
    }
    //그리고 한번에 교체한다.
    series->replace(points);

    //최대 포인트 개수 유지 (낡은 데이터 삭제)
    constexpr int maxCount = 1000;
    if (series->count() > maxCount) {
        series->removePoints(0, series->count() - maxCount);
    }

    //X축 범위 설정
    double latest_ms = data.back().t;
    double width_ms = dataMng.getGraphWidth();
    axisX->setRange(latest_ms - width_ms, latest_ms);
}
