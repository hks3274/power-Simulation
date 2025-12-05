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

    void setWaveVisible(const QString& key, bool visible);

private slots:
    void updateGraph();

private:
    void setupChart();
    dataManagement& dataMng;

    QPushButton* graphSettingBtn;
    QToolButton* autoModeBtn;

    QCustomPlot* customPlot;
    QCPGraph *voltageGraphA{}, *voltageGraphB{}, *voltageGraphC{};
    QCPGraph *currentGraphA{}, *currentGraphB{}, *currentGraphC{};

    bool userInteracted = false;

    QVector<QPair<double, double>> vABuffer;  // (time, voltage)
    QVector<QPair<double, double>> cABuffer;  // (time, current)
    QVector<QPair<double, double>> vBBuffer;  // (time, current)
    QVector<QPair<double, double>> cBBuffer;  // (time, current)
    QVector<QPair<double, double>> vCBuffer;  // (time, current)
    QVector<QPair<double, double>> cCBuffer;  // (time, current)

    int emitCounter_ = 0;            // 마지막 그리기 이후 샘플 누계

    void updateFixedTicks(double rangeStart, double rangeEnd);
};

#endif // GRAPHWIDGET_H
