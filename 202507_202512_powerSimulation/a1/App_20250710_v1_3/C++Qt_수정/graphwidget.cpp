#include "graphwidget.h"
#include<QBoxLayout>
#include <QValueAxis>
#include <QPen>
using namespace std::chrono_literals;

graphWidget::graphWidget(dataManagement& dataMng)
    : dataMng{dataMng}
    , xAxis(new QValueAxis())
{
    series = new QLineSeries();
    series->setPointsVisible(true);

    //chart 내부 그래프 선 색상 & 두께 지정
    QPen pen(QColor(0, 248, 204));  // 선 색상 설정
    pen.setWidth(3);              // 선 두께 설정 (픽셀 단위)
    pen.setStyle(Qt::SolidLine);            // 선 굵기
    series->setPen(pen);

    chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("입력값 그래프");

    //chart배경, 제목 색상 지정
    chart->setTitleBrush(QBrush(QColor(255,255,255)));
    chart->setBackgroundBrush(QBrush(QColor(46,50,56)));

    // 축 범위를 수동 조정 (시간은 0 ~ duration 초, y축은 0~최대값 혹은 고정)
    // X축 생성 및 설정
    //X축 라벨, 제목 색상 지정
    xAxis->setTitleBrush(QBrush(QColor(255,255,255)));
    xAxis->setLabelsBrush(QBrush(QColor(255,255,255)));

    xAxis->setTitleText("Time (s)");
    chart->addAxis(xAxis, Qt::AlignBottom);
    series->attachAxis(xAxis);

    // Y축 생성 및 설정
    QValueAxis* yAxis = new QValueAxis(chart);
    yAxis->setRange(-500, 500);

    // Y축 라벨, 제목 색상지정
    yAxis->setTitleBrush(QBrush(QColor(255,255,255)));
    yAxis->setLabelsBrush(QBrush(QColor(255,255,255)));

    yAxis->setTitleText("Value");
    chart->addAxis(yAxis, Qt::AlignLeft);
    series->attachAxis(yAxis);

    chartView = new QChartView(chart, this);
    chartView->setRenderHint(QPainter::Antialiasing);

    auto layout = new QVBoxLayout(this);
    layout->addWidget(chartView);

    connect(&dataMng, &dataManagement::dataChanged, this, &graphWidget::updateGraph);
}

void graphWidget::updateGraph()
{
    const auto& data = dataMng.getData();

    if (data.size() <= 0) return;


    auto oldest_time = data.front().t;
    auto latest_time = data.back().t;
    auto duration = latest_time - oldest_time;

    if (duration <= 10s) {
        duration = 10s;
    }

    // 시리즈 초기화 (이전 데이터 삭제)
    series->clear();

    // x축 좌표 기준용 카운터, 시간 비율 계산용
    for (const auto& point : data) {
        // point.t - oldest_time 은 chrono::duration 타입임 => 최근의 시간에서 가장 오래된 시간을 빼면 dration이 나옴
        auto d = point.t - oldest_time;

        // duration 대비 상대시간을 double로 변환 (초 단위)
        double x = std::chrono::duration<double>(d).count(); //시간끼리 빼면  2025-07-08 11:00:10.000  - 2025-07-08 11:00:07.000  = 3초 이렇게 나오는데 이걸 3.0으로 바꿔줌

        // y 값은 point.value 직접 사용, 필요시 스케일 조정 가능
        double y = static_cast<double>(point.value);

        // 시리즈에 점 추가
        series->append(x, y);
    }

    xAxis->setRange(0, std::chrono::duration<double>(duration).count());

    // 차트 뷰에 새로 그리도록 요청
    chartView->update();
}
