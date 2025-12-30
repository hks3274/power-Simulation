#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H
#include<QWidget>
#include "datamanagement.h"

#include <QtCharts/QChartView>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QValueAxis>

class graphWidget: public QWidget
{
    Q_OBJECT
public:
    explicit graphWidget(dataManagement& dataMng);


private slots:
    void updateGraph();

private:
    dataManagement& dataMng;

    QChart* chart;
    QLineSeries* series;
    QChartView* chartView;
    int xCounter = 0;
    QValueAxis* xAxis;
};

#endif // GRAPHWIDGET_H
