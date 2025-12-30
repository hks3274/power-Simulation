#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H
#include<QWidget>
#include "datamanagement.h"
#include "graphsetting.h"
#include "qcustomplot.h"

#include <QPushButton>
#include <memory>

/*
public = 어디서든 접근이 가능.
protected = 상속관계일 때 접근이 가능
private = 해당 클래스에서만 접근이 가능
*/

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
