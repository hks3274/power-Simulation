#include "input_widget.h"
#include "dial_widget.h"
#include "slider_widget.h"

#include <QBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QValidator>
#include <QDial>
#include <QIntValidator>

input_widget::input_widget(datamanagement& dataMng)
    : dataMng(dataMng)
{
    resize(400,500);

    QVBoxLayout* mainLay = new QVBoxLayout(this);

    slider_widget* vlotSlide = new slider_widget("Voltage ", dataMng.getVoltVal(), -500, 500,2);

    dial_widget* voltageDial = new dial_widget("Volt.Deg ", dataMng.getVoltDialVal(), 0, 360);

    slider_widget* currSlide = new slider_widget("Current ", dataMng.getCurrVal(), -500, 500,2);

    dial_widget* currentDial = new dial_widget("Curr.Deg ", dataMng.getCurrDialVal(), 0, 360);

    slider_widget* frequncySlide = new slider_widget("주파수 ",dataMng.getFrequncy(), 1, 60,2);

    slider_widget* cpsSlide = new slider_widget("초당 cycle 수 ",dataMng.getFrequncy(), 1, 60,2);

    slider_widget* spcSlide = new slider_widget("cycle당 sample 수 ",dataMng.getFrequncy(), 1, 100, 0);

    QLabel* intervallbl = new QLabel("시간간격 : " + QString::number(dataMng.getInterval().count()) + " ns");

    slider_widget* timeScaleSlide =  new slider_widget("시간 비율", dataMng.getTimeScale(), 1, 10,0);

    mainLay->addWidget(vlotSlide);
    mainLay->addWidget(voltageDial);
    mainLay->addWidget(currSlide);
    mainLay->addWidget(currentDial);
    mainLay->addWidget(frequncySlide);
    mainLay->addWidget(cpsSlide);
    mainLay->addWidget(spcSlide);
    mainLay->addWidget(intervallbl);
    mainLay->addWidget(timeScaleSlide);



    connect(vlotSlide, &slider_widget::valueChanged, this, [&dataMng](double value){
        dataMng.setVoltVal(value);
    });

    connect(voltageDial, &dial_widget::valueChanged, this, [&dataMng](int value){
        dataMng.setVoltDialVal(value);
    });

    connect(currSlide, &slider_widget::valueChanged, this, [&dataMng](double value){
        dataMng.setCurrVal(value);
    });

    connect(currentDial, &dial_widget::valueChanged, this, [&dataMng](int value){
        dataMng.setCurrDialVal(value);
    });

    connect(frequncySlide, &slider_widget::valueChanged, this, [frequncySlide, this, &dataMng](double value){
        dataMng.setFrequncy(value);
    });



    connect(cpsSlide, &slider_widget::valueChanged, this, [&dataMng, intervallbl](double value){
        dataMng.setCyclePerSecond(value);
        intervallbl->setText("시간간격 : " + QString::number(dataMng.getInterval().count()) + " ns");
    });

    connect(spcSlide, &slider_widget::valueChanged, this, [&dataMng, intervallbl](int value){
        dataMng.setSamplePerCycle(value);
        intervallbl->setText("시간간격 : " + QString::number(dataMng.getInterval().count()) + " ns");
    });

    connect(timeScaleSlide, &slider_widget::valueChanged, this, [&dataMng](int value){
        dataMng.setTimeScale(value);
    });

}


