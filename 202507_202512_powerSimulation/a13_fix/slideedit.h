#ifndef SLIDEEDIT_H
#define SLIDEEDIT_H
#include <QLabel>
#include <QLineEdit>
#include <QSlider>
#include <QPushButton>
#include <QDoubleSpinBox>

class slideEdit : public QWidget
{
    Q_OBJECT
public:
    explicit slideEdit(QString cText, double cValue, double Max, double Min);

    void setValue(double newVal);

signals:
    void valueChanged(double newValue); //바뀐 값을 전달

private slots:
    void sliderChanged(); //슬라이드 값이 바뀌었을때
    void lineEditChanged(); //lineEdit값이 바뀌었을때


private:

    double cValue; //초기 세팅 값
    double cMin; //최소값
    double cMax; //최대값

    QLabel *cLabel; //조절 값의 이름
    QLabel *minLabel;
    QLabel *maxLabel;
    QLineEdit *cLineEdit;
    QSlider *cSlider;

    //확대/축소 버튼
    QPushButton* plusBtn;
    QPushButton* minusBtn;

    QDoubleSpinBox* stepSpinBox;
};
#endif // SLIDEEDIT_H
