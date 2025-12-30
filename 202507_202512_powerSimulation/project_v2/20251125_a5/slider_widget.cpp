#include "slider_widget.h"

#include <QBoxLayout>
#include <QLabel>
#include <QDoubleValidator>

slider_widget::slider_widget(QString name, double initialVal, double min, double max, int decimalPoint)
{
    QVBoxLayout* mainlay = new QVBoxLayout(this);

    QHBoxLayout* hlay = new QHBoxLayout();

    QLabel* namelbl = new QLabel(name);
    hlay->addWidget(namelbl);
    valueEdit = new QLineEdit(this);

    valueEdit->setText(QString::number(initialVal));
    QDoubleValidator* dv = new QDoubleValidator(min, max, decimalPoint, this);
    dv->setNotation(QDoubleValidator::StandardNotation);
    valueEdit->setValidator(dv);
    valueEdit->setMinimumWidth(120);
    hlay->addStretch();
    hlay->addWidget(valueEdit);

    mainlay->addLayout(hlay);

    // 스케일 팩터 (소수점 2자리)
    int factor = 100;

    slider = new QSlider(Qt::Horizontal, this);
    slider->setRange(min * factor, max * factor);
    slider->setValue(initialVal * factor);
    mainlay->addWidget(slider);

    connect(slider, &QSlider::valueChanged, this, [=, this](int v){
        double realVal = (double)v / factor;
        valueEdit->setText(QString::number(realVal, 'f', decimalPoint));
        emit valueChanged(realVal);
    });

    // 2) LineEdit → Slider
    connect(valueEdit, &QLineEdit::returnPressed, this, [=, this](){
        double val = valueEdit->text().toDouble();
        slider->setValue(val * factor);
    });

}
