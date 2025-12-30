#include "inputslidewidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFont>
#include <QMessageBox>
#include <QLabel>
#include <QWidget>
#include <QIntValidator>
#include <QDoubleValidator>
#include <QSlider>
#include <iostream>

inputSlideWidget::inputSlideWidget(QWidget *parent)
    : QWidget(parent)
{
    QFont font("맑은고딕", 15, QFont::Bold);

    QLabel *vLabel = new QLabel("전압");
    vLabel->setFont(font);

    vSlider = new QSlider(Qt::Horizontal);
    vSlider->setMaximum(500);
    vSlider->setMinimum(-500);
    vSlider->setFont(font);

    QLabel *vUnitLabel = new QLabel("V");
    vUnitLabel->setFont(font);

    QLabel *sLabel = new QLabel("시간");
    sLabel->setFont(font);

    sSlider = new QSlider(Qt::Horizontal);
    sSlider->setMaximum(10);
    sSlider->setMinimum(1);
    sSlider->setFont(font);

    QLabel *sUnitLabel = new QLabel("s");
    sUnitLabel->setFont(font);

    // 레이아웃 설정 (간단 예시)
    auto layout = new QVBoxLayout(this);

    auto hLayout1 = new QHBoxLayout();
    hLayout1->addWidget(vLabel);
    hLayout1->addWidget(vSlider);
    hLayout1->addWidget(vUnitLabel);

    auto hLayout2 = new QHBoxLayout();
    hLayout2->addWidget(sLabel);
    hLayout2->addWidget(sSlider);
    hLayout2->addWidget(sUnitLabel);

    layout->addLayout(hLayout1);
    layout->addLayout(hLayout2);

    // 엔터 입력 시 슬롯 호출
    connect(vSlider, &QSlider::valueChanged, this, &inputSlideWidget::vInputPressed);
    connect(sSlider, &QSlider::valueChanged, this, &inputSlideWidget::sInputPressed);
}

void inputSlideWidget::vInputPressed()
{
    bool ok;
    int voltage = vSlider->value();
    //std::cout << voltage << std::endl;


    //유효성 검사
    if ( voltage < -500 || voltage > 500) {
        QMessageBox::warning(this, "입력 오류", " -500V ~ 500V 전압을 올바르게 입력하세요.");
        return;
    }

    emit vdataEntered(voltage); //emit: 등록된 이벤트를 실행시키는 함수 -> 뷰에서 처리 X, 컨트롤러에서 처리 O
}

void inputSlideWidget::sInputPressed()
{
    bool ok;
    int time = sSlider->value();
    float slideTime = time / 10.0;
    //std::cout << time << std::endl;

    //유효성 검사
    if (slideTime < 0.1 || slideTime > 60.0 ) {
        QMessageBox::warning(this, "입력 오류", "0.1초 ~ 60초 사이의 값을 입력하세요.");
        return;
    }

    emit sdataEntered(slideTime);
}
