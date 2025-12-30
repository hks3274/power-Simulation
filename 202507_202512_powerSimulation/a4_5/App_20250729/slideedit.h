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
    void valueChanged(float newValue); //바뀐 값을 전달

private slots:
    void sliderChanged(); //슬라이드 값이 바뀌었을때
    void lineEditChanged(); //lineEdit값이 바뀌었을때


private:

    void setupStyles();


    float cValue; //초기 세팅 값
    float cMin; //최소값
    float cMax; //최대값

    QLabel *cLabel; //조절 값의 이름
    QLineEdit *cLineEdit;
    QSlider *cSlider;
};
#endif // SLIDEEDIT_H
