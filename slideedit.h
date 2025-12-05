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
    explicit slideEdit(QString titleName, double value, double max, double min);

    void setValue(double value);

signals:
    void valueChanged(int value);

private slots:
    void lineEditChanged();
    void sliderChanged();

private:
    QLabel*   title = nullptr;
    QLineEdit* lineValue = nullptr;
    QSlider*   lineSlider = nullptr;

    QLabel* minLabel = nullptr;
    QLabel* maxLabel = nullptr;

    double value = 0.0;
};

#endif // SLIDEEDIT_H
