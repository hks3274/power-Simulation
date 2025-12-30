#include "outputwidget.h"
#include <QVBoxLayout>
#include <QTimer>
#include <iostream>
#include <QValueAxis>
#include <QCategoryAxis>
#include <QDateTime>

outputWidget::outputWidget(QWidget *parent)
    : QWidget(parent)
{
    series = new QLineSeries();
    series->setPointsVisible(true); //데이터마다 점을 찍는다.
    chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("전압");
    chart->createDefaultAxes();

    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    auto layout = new QVBoxLayout(this);
    layout->addWidget(chartView);
}

void outputWidget::updateGraph(double time, std::deque<std::tuple<int, long long>> &dataDeque)
{

    // for (const auto& item : dataDeque) {
    //     int voltage;
    //     long long time;
    //     std::tie(voltage, time) = item;  // tuple에서 값 꺼내기

    //     std::cout << "Voltage: " << voltage << ", Time: " << time << std::endl;
    // }


    //만약에 데이터가 비었다면 다시 돌아가라
    if (dataDeque.empty()) return;

    //최근 시간
    double latestTime = std::get<1>(dataDeque.back());
    double startTime = latestTime - 10000; //최근의 시간에서 10초 전까지 보여줌 (5153952 - 10000)

    series->clear(); // 우선 다 지워봐


    //depue에 있는 값을 불러와
    for (const auto& [voltage, timestamp] : dataDeque) { //구조적 바인딩 가능

        // X축은 시작 시간부터의 상대 시간
        long long x = static_cast<long long>(timestamp);
        int y = static_cast<int>(voltage);

        // time 안에 있는 데이터만 추가
        if (timestamp >= startTime)
            series->append(x, y);
    }

    // 자동 축 갱신 (매번 새로 생성하면 이전 축이 누적되지 않도록 해야 함)
    chart->removeAxis(chart->axisX());
    chart->removeAxis(chart->axisY());

    // QValueAxis *axisX = new QValueAxis();
    // axisX->setRange(startTime, latestTime);  // 최신에서 50초 전까지 보여줌
    // axisX->setTitleText("Time (s)");


    // QCategoryAxis: 숫자형축에 문자열(라벨)을 붙일 수 있는 축
    QCategoryAxis *axisX = new QCategoryAxis();
    axisX->setTitleText("Time (hh:mm:ss)"); // x축의 제목을 Time(hh:mm:ss)로 설정합니다.
    axisX->setRange(startTime, latestTime); // 축의 값 범위를 설정합니다.

    int labelCount = 5; // 라벨 개수 (예: 5개), X축 아해의 시간표시가 0부터 5까지 6개 붙음
    double interval = (latestTime - startTime) / labelCount; // start와 latestTime 사이의 전체 길이를 labelcount(5)로 나누어 시간간격을 계산

    for (int i = 0; i <= labelCount; ++i) { //라벨 간격만큼 반복
        qint64 ts = static_cast<qint64>(startTime + i * interval); // 각 라벨의 위치를 밀리초 단위의 타임스탬프로 계산 라벨의 위치 계산
        QDateTime dt = QDateTime::fromMSecsSinceEpoch(ts); //ts 밀리초 타임 스탬프를 QDateTime 객체로 변환 => 유닉스 타임스탬프를 현재 날짜로 계산
        QString label = dt.toString("hh:mm:ss"); // QDateTime객체를 hh:mm:ss로 변환
        axisX->append(label, ts); // 라벨과 위치 추가
    }

    QValueAxis *axisY = new QValueAxis();
    axisY->setRange(-500, 500); // 전압 범위 적절히 설정
    axisY->setTitleText("Voltage");

    chart->setAxisX(axisX, series);
    chart->setAxisY(axisY, series);

    chartView->update();

}
