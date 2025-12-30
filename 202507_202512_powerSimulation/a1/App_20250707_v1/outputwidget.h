#ifndef OUTPUTWIDGET_H
#define OUTPUTWIDGET_H

#include <QWidget>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QChart>
#include <deque>
#include <tuple>

class outputWidget: public QWidget
{
    Q_OBJECT
public:
    explicit outputWidget(QWidget *parent = nullptr);

signals:

public slots:
    void updateGraph(double time, std::deque<std::tuple<int, long long>> &dataDeque);

private:
    QLineSeries *series; //그래프 선
    QChart *chart;  //Qchart
    QChartView *chartView; //ChartView
};

#endif // OUTPUTWIDGET_H
