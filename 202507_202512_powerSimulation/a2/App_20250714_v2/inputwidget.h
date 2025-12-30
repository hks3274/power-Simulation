#ifndef INPUTWIDGET_H
#define INPUTWIDGET_H

#include "datamanagement.h"
#include <QWidget>
#include <QLineEdit>
#include <QSlider>

class inputWidget : public QWidget
{
    Q_OBJECT
public:
    explicit inputWidget(dataManagement& dataMng);

private slots:
    void inputChanged(float newVal);
    void intervalChanged(float newVal);

private:
    dataManagement& dataMng;
    QLineEdit *dataLineVal;
    QLineEdit *timeInterLineval;
    QSlider *dataSlideVal;
    QSlider *timeInterSlideval;
};

#endif // INPUTWIDGET_H
