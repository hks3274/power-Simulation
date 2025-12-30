#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H
#include<QWidget>
#include "datamanagement.h"
#include "graphsetting.h"

#include <QtCharts/QChartView>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QPushButton>
#include <QtCharts/QDateTimeAxis>
#include <QtCharts/QValueAxis>

class graphWidget: public QWidget
{
    Q_OBJECT
public:
    explicit graphWidget(dataManagement& dataMng);


private slots:
    void updateGraph();

private:
    void setupChart();
    dataManagement& dataMng;

    QChart* chart;
    QChartView* chartView;
    QLineSeries* series;
    QValueAxis* axisX;
    QValueAxis* axisY;
    graphSetting* settingDialog;
    QPushButton* graphSettingBtn;
};

#endif // GRAPHWIDGET_H
