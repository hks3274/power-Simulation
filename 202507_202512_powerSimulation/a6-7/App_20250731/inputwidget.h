#ifndef INPUTWIDGET_H
#define INPUTWIDGET_H

#include "datamanagement.h"
#include "slideedit.h"
#include <QWidget>
#include <QLineEdit>
#include <QSlider>
#include <QDial>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QCheckBox>
#include <QTimer>

class inputWidget : public QWidget
{
    Q_OBJECT
public:
    explicit inputWidget(dataManagement& dataMng);

private slots:
    void inputChanged(float newVal);
    void intervalChanged();
    void frequencyChanged(float newVal);
    void timeScaleChanged(float newVal);
    void currentValueChanged(float newVal);
    void currentPhaseChanged(float newVal);


private:
    dataManagement& dataMng;
    slideEdit* vSlideEdit;
    slideEdit* fSlideEdit;
    slideEdit* tsSlideEdit;
    slideEdit* iSlideEdit;
    QSpinBox *samplingCycleSpinBox;
    QSpinBox *cycleSampleNumSpinBox;
    QLabel *timeInterVal;
    QLabel *currentPhaseDisplay;
    QDial *currentPhaseDial;
};

#endif // INPUTWIDGET_H


