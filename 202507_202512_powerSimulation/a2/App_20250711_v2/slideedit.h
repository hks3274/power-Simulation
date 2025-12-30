#ifndef SLIDEEDIT_H
#define SLIDEEDIT_H
#include <QLabel>
#include <QLineEdit>
#include <QSlider>

class slideEdit : public QWidget
{
    Q_OBJECT
public:
    explicit slideEdit(QString cText, float cValue, int Max, int Min);

signals:
    void valueChanged(float newValue);

private slots:
    void sliderChanged();
    void lineEditChanged();


private:
    float cValue;
    float cMin;
    float cMax;
    float cStep;
    int scaleFactor;

    QLabel *cLabel;
    QLineEdit *cLineEdit;
    QSlider *cSlider;
};
#endif // SLIDEEDIT_H
