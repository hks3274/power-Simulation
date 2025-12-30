#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H
#include<QWidget>
#include "datamanagement.h"
#include "graphsetting.h"
#include "qcustomplot.h"

#include <QPushButton>


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

    graphSetting* settingDialog;
    QPushButton* graphSettingBtn;
    QPushButton* liveViewBtn;

    QCustomPlot* customPlot;
    QCPGraph* graph;

    QCPGraph* lineGraph;
    QCPGraph* pointGraph;

    bool userInteracted = false;
};

#endif // GRAPHWIDGET_H
