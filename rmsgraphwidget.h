#ifndef RMSGRAPHWIDGET_H
#define RMSGRAPHWIDGET_H

#include "dataManagement.h"
#include "qcustomplot.h"
#include <QWidget>
#include <QVBoxLayout>
#include <QToolButton>

class rmsGraphWidget : public QWidget
{
    Q_OBJECT
public:
    explicit rmsGraphWidget(dataManagement& dataMng);

    void refreshFromData();
    void setRmsVisible(const QString& key, bool visible);

private slots:
    void updateData(dataManagement::measure_data m);

private:
    dataManagement& dataMng;

    QToolButton* autoModeBtn;

    QCustomPlot* plotVIP;
    QCustomPlot* plotFundVI;
    QCustomPlot* plotHarmonicVI;

    QCPGraph *gVrmsA, *gVrmsB, *gVrmsC;
    QCPGraph *gIrmsA, *gIrmsB, *gIrmsC;
    QCPGraph *gPavg;

    QCPGraph *fundVrmsA,*fundVrmsB,*fundVrmsC;
    QCPGraph *fundCrmsA,*fundCrmsB,*fundCrmsC;

    QCPGraph *harmVrmsA,*harmVrmsB;
    QCPGraph *harmCrmsA,*harmCrmsB;

    double m_lastT = -1.0;
    double visibleSec = 10.0;
    bool m_followTail = true;   // true: 새 데이터 올 때 오른쪽으로 자동 스크롤
    void updateFixedTicks(double rangeStart, double rangeEnd);
};

#endif // RMSGRAPHWIDGET_H
