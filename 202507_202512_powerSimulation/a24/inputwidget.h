// inputwidget.h
#ifndef INPUTWIDGET_H
#define INPUTWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QDial>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QSpinBox>
#include "dataManagement.h"
#include "slideedit.h"

class inputWidget : public QWidget
{
    Q_OBJECT
public:
    explicit inputWidget(dataManagement& dataMng);

    // 저장값 불러올 때 UI만 갱신 (연동은 나중 단계에서)
    void refreshFromData();

private:
    dataManagement& dataMng;
    slideEdit* vASlideEdit;
    slideEdit* iASlideEdit;
    QSpinBox *iAPhaseSpin;
    QDial *iAPhaseDial;

    slideEdit* vBSlideEdit;
    QSpinBox *vBPhaseSpin;
    QDial *vBPhaseDial;
    slideEdit* iBSlideEdit;
    QSpinBox *iBPhaseSpin;
    QDial *iBPhaseDial;

    slideEdit* vCSlideEdit;
    QSpinBox *vCPhaseSpin;
    QDial *vCPhaseDial;
    slideEdit* iCSlideEdit;
    QSpinBox *iCPhaseSpin;
    QDial *iCPhaseDial;

};

#endif // INPUTWIDGET_H


