#include "graphwidget.h"
#include<QBoxLayout>
#include <QDateTime>


using namespace std::chrono_literals;

graphWidget::graphWidget(dataManagement& dataMng)
    : dataMng{dataMng}
{
    graphSettingBtn = new QPushButton("그래프 설정");
    graphSettingBtn->setFixedSize(150, 40);

    series = new QLineSeries();
    series->setPointsVisible(true);

    //chart 내부 그래프 선 색상 & 두께 지정
    QPen pen(QColor(0, 248, 204));  // 선 색상 설정
    pen.setWidth(3);              // 선 두께 설정 (픽셀 단위)
    pen.setStyle(Qt::SolidLine);            // 선 굵기
    series->setPen(pen);


    chart = new QChart();
    chart->setTitle("입력값 그래프");

    //chart배경, 제목 색상 지정
    chart->setTitleBrush(QBrush(QColor(255,255,255)));
    chart->setBackgroundBrush(QBrush(QColor(46,50,56)));


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
    axisX->setRange(0, 10); // 초기 범위: 0~10초
    axisX->setLabelFormat("%.2f");
    axisX->setTitleText("Time(s)");

    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    //y축 설정
    axisY = new QValueAxis();
    axisY->setRange(-500, 500);
    axisY->setTitleText("Value");
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    //X축 라벨, 제목 색상 지정
    axisX->setTitleBrush(QBrush(QColor(255,255,255)));
    axisX->setLabelsBrush(QBrush(QColor(255,255,255)));

    // Y축 라벨, 제목 색상지정
    axisY->setTitleBrush(QBrush(QColor(255,255,255)));
    axisY->setLabelsBrush(QBrush(QColor(255,255,255)));
}

void graphWidget::updateGraph()
{
    auto data = dataMng.getData();
    if (data.empty()) return;

    // 1. 최신 데이터 (가장 마지막 값)를 가져온다
    const auto& point = data.back();

    // 2. 시간(ms 단위)과 값 추출
    double latest_ms = point.t / 1000.0;
    double value = point.value;

    // 3. 시리즈에 새 값 추가
    series->append(latest_ms, value);

    // 4. 최대 포인트 개수 유지 (낡은 데이터 삭제)
    constexpr int maxCount = 1000;
    if (series->count() > maxCount) {
        series->removePoints(0, series->count() - maxCount);
    }

    // 5. 사용자 설정 시간 폭 (ms 단위) 가져오기
    float width_ms = dataMng.getGraphWidth();

    // 6. X축 범위 설정: [latest - 폭] ~ [latest]
    axisX->setRange(latest_ms - width_ms, latest_ms);
}
