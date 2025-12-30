#ifndef SLIDER_WIDGET_H
#define SLIDER_WIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QSlider>
#include <qboxlayout.h>

class slider_widget: public QWidget
{
    Q_OBJECT
public:
    slider_widget(QString name, double initialVal, double min, double max, int decimalPoint);

signals:
    void valueChanged(double v);

private:
    QLineEdit* valueEdit;
    QSlider* slider;
};

#endif // SLIDER_WIDGET_H
