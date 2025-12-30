#ifndef INPUTSLIDEWIDGET_H
#define INPUTSLIDEWIDGET_H

#include <QSlider>

class inputSlideWidget
: public QWidget
{
    Q_OBJECT
public:
    explicit inputSlideWidget(QWidget *parent = nullptr);

signals:
    void vdataEntered(int voltage);
    void sdataEntered(double time);

private slots:
    void vInputPressed();
    void sInputPressed();

private:
    QSlider *vSlider;
    QSlider *sSlider;

};

#endif // INPUTWIDGET_H
