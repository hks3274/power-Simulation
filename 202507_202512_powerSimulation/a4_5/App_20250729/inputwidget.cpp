#include "inputwidget.h"

#include <QBoxLayout>
#include <Qt>


inputWidget::inputWidget(dataManagement& dataMng) //생성자를 호출할 때 넘겨는주는 인자(argument), 생성자의 매개변수
    : dataMng(dataMng) // 멤버 초기화 리스트,  클래스 내부에 있는 멤버 변수 dataMng를 매개변수 dataMng로 초기화
{
    auto vl = new QVBoxLayout(this);

    //connect(&autoTimer, &QTimer::timeout, this, &inputWidget::autoTimerUpdate);

    //전압값과 주파수값 생성
    {
        //SlideEdit 사용
        vSlideEdit = new slideEdit("전압값(V)", dataMng.getInputValue(), 500, -500);
        vl->addWidget(vSlideEdit);

        fSlideEdit = new slideEdit("주파수(Hz)", dataMng.getInputFreq(), 100, 1);
        vl->addWidget(fSlideEdit);

        tsSlideEdit = new slideEdit("시간 비율", dataMng.getTimeScale(), 100, 1);
        vl->addWidget(tsSlideEdit);

        connect(vSlideEdit, &slideEdit::valueChanged, this, &inputWidget::inputChanged);
        connect(fSlideEdit, &slideEdit::valueChanged, this, &inputWidget::frequencyChanged);
        connect(tsSlideEdit, &slideEdit::valueChanged, this, &inputWidget::timeScaleChanged);

    }


    //sampling 시간 간격 입력
    {

        auto hl = new QHBoxLayout();

        QLabel *samplingCycleLabel = new QLabel("A. Sampling 초당 cycle 수 : ");
        samplingCycleSpinBox = new  QSpinBox();
        samplingCycleSpinBox->setMaximum(50);
        samplingCycleSpinBox->setMinimum(1);
        samplingCycleSpinBox->setValue(1);


        hl->addWidget(samplingCycleLabel);
        hl->addWidget(samplingCycleSpinBox);

        connect(samplingCycleSpinBox, &QSpinBox::valueChanged, this, &inputWidget::intervalChanged);

        vl->addLayout(hl);
    }

    {
        auto hl = new QHBoxLayout();

        QLabel *cycleSampleNumLabel = new QLabel("B. sampling Cycle당 sample 수 : ");
        cycleSampleNumSpinBox = new QSpinBox();
        cycleSampleNumSpinBox->setMaximum(50);
        cycleSampleNumSpinBox->setMinimum(1);
        cycleSampleNumSpinBox->setValue(10);

        hl->addWidget(cycleSampleNumLabel);
        hl->addWidget(cycleSampleNumSpinBox);

        connect(cycleSampleNumSpinBox, &QSpinBox::valueChanged, this, &inputWidget::intervalChanged);

        vl->addLayout(hl);
    }

    {
        auto hl = new QHBoxLayout();

        QLabel *timeIntervalLabel = new QLabel( " 시간간격 : " );

        timeInterVal = new QLabel(QString::number((1.0f / (samplingCycleSpinBox->value() * cycleSampleNumSpinBox->value())) * 1000.0f)+"ms");
        dataMng.setTimeInterval((1.0f / (samplingCycleSpinBox->value() * cycleSampleNumSpinBox->value())) * 1000.0f);

        hl->addWidget(timeIntervalLabel);
        hl->addWidget(timeInterVal);

        vl->addLayout(hl);
    }
}


//전압값 변경시 전압값 데이터 변경
void inputWidget::inputChanged(float newVal)
{
    dataMng.setInputValue(newVal);
}

//시간간격 계산 -> data 변경
void inputWidget::intervalChanged()
{
    float interval_ms = (1000.0f / (samplingCycleSpinBox->value() * cycleSampleNumSpinBox->value()));
    timeInterVal->setText(QString::number(interval_ms) +"ms");

    dataMng.setTimeInterval(interval_ms);
}

//주파수 값 변경시 주파수 data 변경
void inputWidget::frequencyChanged(float newVal)
{
    dataMng.setInputFreq(newVal);
}

// 시간 비율 변경 시 시간비율 data 변경
void inputWidget::timeScaleChanged(float newVal)
{
    dataMng.setTimeScale(newVal);
}

