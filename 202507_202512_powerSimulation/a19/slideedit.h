#ifndef SLIDEEDIT_H
#define SLIDEEDIT_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QSlider>

class slideEdit : public QWidget
{
    Q_OBJECT
public:
    explicit slideEdit(QString cText, double cValue, double Max, double Min);

    void setValue(double newVal);

signals:
    void valueChanged(double v);

private slots:
    void lineEditChanged();
    void sliderChanged();

private:
    QLabel*   cLabel = nullptr;
    QLineEdit* cLineEdit = nullptr;
    QSlider*   cSlider = nullptr;

    QLabel* minLabel = nullptr;
    QLabel* maxLabel = nullptr;

    double cValue = 0.0;
    double cMin = 0.0;
    double cMax = 0.0;
};

#endif // SLIDEEDIT_H
