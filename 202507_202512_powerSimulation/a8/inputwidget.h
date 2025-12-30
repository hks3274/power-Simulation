#ifndef INPUTWIDGET_H
#define INPUTWIDGET_H

#include "dataManagement.h"
#include "slideedit.h"
#include <QWidget>
#include <QLineEdit>
#include <QSlider>
#include <QDial>
#include <QDoubleSpinBox>
#include <QButtonGroup>
#include <QLabel>
#include <QCheckBox>
#include <QTimer>
#include <QRadioButton>

class inputWidget : public QWidget
{
    Q_OBJECT
public:
    explicit inputWidget(dataManagement& dataMng);

    void refreshFromData();

private slots:
    void inputChanged(float newVal);
    void intervalChanged();
    void frequencyChanged(float newVal);
    void timeScaleChanged(float newVal);
    void currentValueChanged(float newVal);
    void currentPhaseChanged(float newVal);
    void widthChanged(double newVal);
    void graphSettingChange();


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
    QDoubleSpinBox *widthSpinBox;
    QButtonGroup* updateModeGroup;

    QRadioButton* oneSampleBtn;
    QRadioButton* halfCycleBtn;
    QRadioButton* fullCycleBtn;
};

#endif // INPUTWIDGET_H


