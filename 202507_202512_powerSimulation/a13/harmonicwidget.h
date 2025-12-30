#ifndef HARMONICWIDGET_H
#define HARMONICWIDGET_H

#include <QWidget>
#include <QTabWidget>
#include <QSpinBox>
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


private:
    dataManagement& dataMng;

    // 전압 탭 위젯
    QSpinBox* vOrderSpin;
    QSpinBox* vMagSpin;
    QDial*    vPhaseDial;
    QSpinBox* vPhaseSpin;

    // 전류 탭 위젯
    QSpinBox* cOrderSpin;
    QSpinBox* cMagSpin;
    QDial*    cPhaseDial;
    QSpinBox* cPhaseSpin;

};

#endif // HARMONICWIDGET_H
