#include "slideedit.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QString>

slideEdit::slideEdit(QString cText, float cValue, int Max, int Min)
:
    cValue(cValue), cMin(Min), cMax(Max)
{
    cLabel = new QLabel(cText, this);
    cLineEdit = new QLineEdit(QString::number(cValue), this);
    cSlider = new QSlider(Qt::Horizontal, this);

    cSlider->setRange(Min, Max);
    cSlider->setValue(static_cast<int>(cValue));

    auto hLayout = new QHBoxLayout();
    hLayout->addWidget(cLabel);
    hLayout->addWidget(cLineEdit);

    auto vLayout = new QVBoxLayout(this);
    vLayout->addLayout(hLayout);
    vLayout->addWidget(cSlider);

    connect(cSlider, &QSlider::valueChanged, this, &slideEdit::sliderChanged);
    connect(cLineEdit, &QLineEdit::returnPressed, this, &slideEdit::lineEditChanged);
}

void slideEdit::lineEditChanged()
{
    bool ok;
    float newVal = cLineEdit->text().toFloat(&ok);

    if (ok) {
        cValue = newVal;
        cSlider->setValue(static_cast<int>(cValue));
    } else {
        // 잘못된 입력 시 되돌림
        cLineEdit->setText(QString::number(cValue));
    }
}

void slideEdit::sliderChanged()
{
    cValue = cSlider->value();
    cLineEdit->setText(QString::number(cValue));

    emit valueChanged(cValue);
}
