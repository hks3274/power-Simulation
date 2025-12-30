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
#include <QPushButton>

class inputWidget : public QWidget
{
    Q_OBJECT
public:
    explicit inputWidget(dataManagement& dataMng);

    void refreshFromData();

private slots:
    void inputChanged(double newVal);
    void intervalChanged();
    void frequencyChanged(double newVal);
    void timeScaleChanged(double newVal);
    void currentValueChanged(double newVal);
    void currentPhaseChanged(double newVal);
    void widthChanged(double newVal);
    void graphSettingChange();

public slots:
    void setCpsFromTracker(double fs);


private:
    dataManagement& dataMng;
    slideEdit* vSlideEdit;
    slideEdit* fSlideEdit;
    slideEdit* tsSlideEdit;
    slideEdit* iSlideEdit;
    QDoubleSpinBox *samplingCycleSpinBox;
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


