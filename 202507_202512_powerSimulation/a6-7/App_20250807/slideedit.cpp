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
    cSlider->setRange(Min * 10, Max * 10);
    cSlider->setValue(cValue * 10.0);

    plusBtn = new QPushButton("+", this);
    minusBtn = new QPushButton("-", this);

    minLabel = new QLabel(QString::number(Min), this);
    maxLabel = new QLabel(QString::number(Max), this);

    stepSpinBox = new QDoubleSpinBox(this);
    stepSpinBox->setDecimals(1);        // 소수점 한 자리까지
    stepSpinBox->setRange(0.1, 100.0);  // 최소/최대 설정
    stepSpinBox->setSingleStep(0.1);    // 한 번에 움직이는 단위
    stepSpinBox->setValue(1.0);         // 기본값 1.0

    // 버튼 행
    auto buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(minusBtn);
    buttonLayout->addWidget(stepSpinBox);
    buttonLayout->addWidget(plusBtn);

    // 슬라이더 + min/max
    auto sliderLayout = new QHBoxLayout();
    sliderLayout->addWidget(minLabel);
    sliderLayout->addWidget(cSlider);
    sliderLayout->addWidget(maxLabel);

    // 텍스트 입력
    auto labelLayout = new QHBoxLayout();
    labelLayout->addWidget(cLineEdit);

    QFrame *outerFrame = new QFrame(this);
    outerFrame->setFrameStyle(QFrame::Box | QFrame::Plain);
    outerFrame->setLineWidth(1);  // 두께 조절
    outerFrame->setObjectName("myOuterFrame");
    outerFrame->setStyleSheet("#myOuterFrame { border: 1px solid #ccc; border-radius: 5px; background-color: #eee; }");

    // 전체 조립
    auto frameLayout_inner = new QVBoxLayout();
    frameLayout_inner->addLayout(labelLayout);
    frameLayout_inner->addLayout(buttonLayout);
    frameLayout_inner->addLayout(sliderLayout);


    auto frameLayout = new QHBoxLayout();
    frameLayout->addWidget(cLabel);
    frameLayout->addLayout(frameLayout_inner);

    outerFrame->setLayout(frameLayout);

    auto mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(outerFrame);

    connect(cSlider, &QSlider::valueChanged, this, &slideEdit::sliderChanged);
    connect(cLineEdit, &QLineEdit::returnPressed, this, &slideEdit::lineEditChanged);

    connect(plusBtn, &QPushButton::clicked, this, [=, this]() {
        int step = static_cast<int>(stepSpinBox->value() * 10.0);  // 내부는 10배 단위
        int newVal = cSlider->value() + step;
        cSlider->setValue(newVal);
    });

    connect(minusBtn, &QPushButton::clicked, this, [=, this]() {
        int step = static_cast<int>(stepSpinBox->value() * 10.0);
        int newVal = cSlider->value() - step;
        cSlider->setValue(newVal);
    });

    //폰트 설정
    QFont boldFont("나눔고딕", 10, QFont::Bold);
    cLabel->setFont(boldFont);
}

void slideEdit::lineEditChanged()
{
    bool ok;
    float newVal = cLineEdit->text().toFloat(&ok);

    if (ok) {
        cValue = newVal * 10.0;
        cSlider->setValue(static_cast<int>(cValue));
    } else {
        // 잘못된 입력 시 되돌림
        cLineEdit->setText(QString::number(cValue / 10.0));
    }
}

void slideEdit::sliderChanged()
{
    cValue = cSlider->value();
    cLineEdit->setText(QString::number(cValue / 10.0));

    emit valueChanged(cValue / 10.0);
}

// slideedit.cpp
void slideEdit::setValue(float newVal)
{
    cValue = newVal * 10.0; // 내부는 10배 스케일
    cSlider->setValue(static_cast<int>(cValue));
    cLineEdit->setText(QString::number(newVal));
}

