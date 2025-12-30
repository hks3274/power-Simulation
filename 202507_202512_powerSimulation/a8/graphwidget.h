#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H
#include<QWidget>
#include "dataManagement.h"
#include "qcustomplot.h"

#include <QPushButton>
#include <QToolButton>

class graphWidget: public QWidget
{
    Q_OBJECT
public:
    explicit graphWidget(dataManagement& dataMng);

    void refreshFromData();

private slots:
    void updateGraph();


private:
    void setupChart();
    dataManagement& dataMng;

    QPushButton* graphSettingBtn;
    QToolButton* autoModeBtn;

    QCustomPlot* customPlot;
    QCPGraph* voltageGraph;
    QCPGraph* currentGraph;

    bool userInteracted = false;

    QVector<QPair<double, double>> voltageBuffer;  // (time, voltage)
    QVector<QPair<double, double>> currentBuffer;  // (time, current)
};

#endif // GRAPHWIDGET_H
