#ifndef INPUTSETTINGWIDGET_H
#define INPUTSETTINGWIDGET_H

#include "dataManagement.h"
#include "slideedit.h"
#include <QWidget>
#include <QObject>
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

class inputSettingWidget :  public QWidget
{
    Q_OBJECT
public:
    inputSettingWidget(dataManagement& dataMng);

    void refreshFromData();

private slots:
    void intervalChanged();
    void frequencyChanged(double newVal);
    void timeScaleChanged(double newVal);
    void widthChanged(double newVal);
    void graphSettingChange();

public slots:
    void setCpsFromTracker(double fs);

private:
    dataManagement& dataMng;

    slideEdit* fSlideEdit;
    slideEdit* tsSlideEdit;
    QDoubleSpinBox *samplingCycleSpinBox;
    QSpinBox *cycleSampleNumSpinBox;
    QLabel *timeInterVal;
    QDoubleSpinBox *widthSpinBox;
    QButtonGroup* updateModeGroup;

    QRadioButton* oneSampleBtn;
    QRadioButton* halfCycleBtn;
    QRadioButton* fullCycleBtn;


};

#endif // INPUTSETTINGWIDGET_H
