#include "dial_widget.h"

#include <QBoxLayout>
#include <QDial>
#include <QLabel>
#include <QLineEdit>
#include<QIntValidator>

dial_widget::dial_widget(QString name, double value, int min, int max)
{
    QHBoxLayout* mainLay = new QHBoxLayout(this);
    QDial* dial = new QDial();
    dial->setRange(min, max);
    dial->setMinimumSize(120, 120);
    dial->setMaximumSize(200, 200);
    int dialVal = dial->value();
    int degree = (dialVal + 180) % 360;
    dial->setValue(degree);

    QLabel* dialName = new QLabel(name);

    QLineEdit* dialEdit = new QLineEdit(QString(QString::number(value)));
    QIntValidator* iv = new QIntValidator(0, 360, 0);
    dialEdit->setValidator(iv);
    dialEdit->setMinimumWidth(80);

    mainLay->addWidget(dialName);
    mainLay->addWidget(dial);
    mainLay->addWidget(dialEdit);


    connect(dial, &QDial::valueChanged, this, [dial, dialEdit, this](int value){
        int degree = (value + 180) % 360;
        dialEdit->setText(QString(QString::number(degree)));
        emit valueChanged(degree);
    });

    connect(dialEdit, &QLineEdit::returnPressed, this, [dial, dialEdit](){
        int value = dialEdit->text().toInt();
        dial->setValue((value + 180) % 360);
    });
}


