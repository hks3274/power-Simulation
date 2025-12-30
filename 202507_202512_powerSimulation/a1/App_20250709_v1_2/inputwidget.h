#ifndef INPUTWIDGET_H
#define INPUTWIDGET_H

#include "datamanagement.h"
#include <QWidget>
#include <QLineEdit>
#include <QSlider>
#include <QDial>

class inputWidget : public QWidget
{
    Q_OBJECT
public:
    explicit inputWidget(dataManagement& dataMng);

private slots:
    void inputValueChanged();
    void intervalChanged();
    void phaseValueChanged();

private:
    dataManagement& dataMng;
    //QLineEdit *dataVal;
    //QLineEdit *timeInterval;
    QSlider *dataVal;
    QSlider *timeInterval;
    QDial *phaseVal;
};

#endif // INPUTWIDGET_H
