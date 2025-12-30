#ifndef HARMONICWIDGET_H
#define HARMONICWIDGET_H

#include <QWidget>
#include <QTabWidget>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QDial>
#include <QVBoxLayout>
#include <QLabel>
#include <QTextEdit>
#include "dataManagement.h"

class harmonicwidget: public QWidget
{
    Q_OBJECT
public:
    harmonicwidget(dataManagement& dataMng);

    void refreshFromData();

private:
    dataManagement& dataMng;

    // 전압 탭 위젯
    QSpinBox* vOrderSpin;
    QDoubleSpinBox* vMagSpin;
    QDial*    vPhaseDial;
    QSpinBox* vPhaseSpin;

    // 전류 탭 위젯
    QSpinBox* cOrderSpin;
    QDoubleSpinBox* cMagSpin;
    QDial*    cPhaseDial;
    QSpinBox* cPhaseSpin;

};

#endif // HARMONICWIDGET_H
