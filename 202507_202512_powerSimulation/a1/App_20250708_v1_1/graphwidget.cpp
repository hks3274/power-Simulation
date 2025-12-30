#include "graphwidget.h"
#include<QBoxLayout>

using namespace std::chrono_literals;

graphWidget::graphWidget(dataManagement& dataMng)
    : dataMng{dataMng}
{
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
    layout->addWidget(chartView);

    connect(&dataMng, &dataManagement::dataChanged, this, &graphWidget::updateGraph);
}

void graphWidget::updateGraph()
{
    auto data = dataMng.getData();

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

    // 축 범위를 수동 조정 (시간은 0 ~ duration 초, y축은 0~최대값 혹은 고정)
    chart->axisX()->setRange(0, std::chrono::duration<double>(duration).count());

    // y축 범위 (-500 ~ 500)
    chart->axisY()->setRange(-500, 500);

    // 차트 뷰에 새로 그리도록 요청
    chartView->update();
}
