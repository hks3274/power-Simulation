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
    void inputAngleChange(float newVal);
    void autoTimerUpdate();
    void rpsChanged(float newVal);
    void timeScaleChanged(int newVal);

private:
    dataManagement& dataMng;
    QTimer autoTimer;

    slideEdit* vSlideEdit;
    slideEdit* timeScaleSlideEdit;
    QDial *vDial;
    QLabel *aLabel;
    QCheckBox* automode;
    QDoubleSpinBox* rpsSpinBox;
    QTimer* autoRotateTimer;
};

#endif // INPUTWIDGET_H
