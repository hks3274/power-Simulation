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

    // 스타일 적용
    setupStyles();

    connect(cSlider, &QSlider::valueChanged, this, &slideEdit::sliderChanged);
    connect(cLineEdit, &QLineEdit::returnPressed, this, &slideEdit::lineEditChanged);
}

void slideEdit::setupStyles()
{
    // 전체 위젯 스타일
    this->setStyleSheet(
        "slideEdit {"
        "   background-color: #f8f9fa;"
        "   border: 2px solid #e9ecef;"
        "   border-radius: 12px;"
        "   padding: 5px;"
        "}"
        );

    // 라벨 스타일
    cLabel->setStyleSheet(
        "QLabel {"
        "   color: #495057;"
        "   font-weight: bold;"
        "   font-size: 14px;"
        "   background-color: transparent;"
        "   padding: 5px 0px;"
        "}"
        );

    // LineEdit 스타일
    cLineEdit->setStyleSheet(
        "QLineEdit {"
        "   background-color: #ffffff;"
        "   border: 2px solid #dee2e6;"
        "   border-radius: 8px;"
        "   padding: 8px 12px;"
        "   font-size: 13px;"
        "   color: #495057;"
        "   min-width: 80px;"
        "   max-width: 100px;"
        "}"
        "QLineEdit:focus {"
        "   border-color: #4dabf7;"
        "   background-color: #f8f9ff;"
        "   outline: none;"
        "}"
        "QLineEdit:hover {"
        "   border-color: #74c0fc;"
        "}"
        );

    // 슬라이더 스타일 (box-shadow 제거)
    cSlider->setStyleSheet(
        "QSlider::groove:horizontal {"
        "   background: #e9ecef;"
        "   height: 8px;"
        "   border-radius: 4px;"
        "   margin: 2px 0;"
        "}"
        "QSlider::handle:horizontal {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
        "       stop:0 #4dabf7, stop:1 #339af0);"
        "   border: 3px solid #ffffff;"
        "   width: 20px;"
        "   height: 20px;"
        "   margin: -8px 0;"
        "   border-radius: 12px;"
        "}"
        "QSlider::handle:horizontal:hover {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
        "       stop:0 #74c0fc, stop:1 #4dabf7);"
        "   border: 3px solid #ffffff;"
        "   transform: scale(1.1);"
        "}"
        "QSlider::handle:horizontal:pressed {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
        "       stop:0 #339af0, stop:1 #228be6);"
        "   border: 2px solid #ffffff;"
        "}"
        "QSlider::sub-page:horizontal {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
        "       stop:0 #4dabf7, stop:1 #74c0fc);"
        "   border-radius: 4px;"
        "}"
        "QSlider::add-page:horizontal {"
        "   background: #e9ecef;"
        "   border-radius: 4px;"
        "}"
        );

    // LineEdit에 툴팁 추가
    cLineEdit->setToolTip(QString("값 범위: %1 ~ %2").arg(cMin).arg(cMax));
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

