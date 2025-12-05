#ifndef DIALEDIT_H
#define DIALEDIT_H

#include <QWidget>
#include <QDial>
#include <QSpinBox>

class dialEdit : public QWidget
{
    Q_OBJECT
public:
    dialEdit(QString lblName, int value);

    void setDialValue(int newVal);

private slots:
    void onDialChanged(int deg);
    void onSpinChanged(int deg);

signals:
    void valueChanged(int deg);

private:
    QDial* phaseDial;
    QSpinBox* phaseSpin;
};

#endif // DIALEDIT_H
