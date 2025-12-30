#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H
#include<QWidget>
#include "datamanagement.h"
#include "graphsetting.h"
#include "qcustomplot.h"

#include <QPushButton>
#include <memory>


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
    QPushButton* autoModeBtn;

    QCustomPlot* customPlot;
    QCPGraph* voltageGraph;
    QCPGraph* currentGraph;

    bool userInteracted = false;
    std::unique_ptr<graphSetting> settingDialog;

    QVector<QPair<double, double>> voltageBuffer;  // (time, voltage)
    QVector<QPair<double, double>> currentBuffer;  // (time, current)
};

#endif // GRAPHWIDGET_H
