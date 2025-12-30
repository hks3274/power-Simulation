#ifndef DIAL_WIDGET_H
#define DIAL_WIDGET_H

#include <QWidget>

class dial_widget : public QWidget
{
    Q_OBJECT
public:
    dial_widget(QString name, double value, int min, int max);

signals:
    void valueChanged(double v);
};

#endif // DIAL_WIDGET_H
