#include "inputwidget.h"

#include <QBoxLayout>
#include <Qt>
#include <QGroupBox>


inputWidget::inputWidget(dataManagement& dataMng) //생성자를 호출할 때 넘겨는주는 인자(argument), 생성자의 매개변수
    : dataMng(dataMng) // 멤버 초기화 리스트,  클래스 내부에 있는 멤버 변수 dataMng를 매개변수 dataMng로 초기화
{
    auto vl = new QVBoxLayout(this);

    QFrame *outerFrame = new QFrame(this);
    outerFrame->setFrameStyle(QFrame::Box | QFrame::Plain);
    outerFrame->setLineWidth(1);  // 두께 조절
    outerFrame->setObjectName("myOuterFrame");
    outerFrame->setStyleSheet("#myOuterFrame { border: 1px solid #ccc; border-radius: 5px; background-color: #eee; }");

    //전압값과 주파수값 생성
    {
        //SlideEdit 사용
        vSlideEdit = new slideEdit("전압값(V)", dataMng.getInputValue(), 500, 0);
        vl->addWidget(vSlideEdit);

        QFont boldFont("나눔고딕", 10, QFont::Bold);

        //전류 위상 변경용 다이얼 위젯 사용
        {
            auto dialLayout = new QHBoxLayout(outerFrame);

            iSlideEdit = new slideEdit("전류값(A)", dataMng.getCurrentValue(), 500, 0);
            vl->addWidget(iSlideEdit);

            QLabel* phaseLabel = new QLabel("전류 위상(°)");
            phaseLabel->setFont(boldFont);
            currentPhaseDial = new QDial();
            currentPhaseDial->setRange(-180, 180);
            currentPhaseDial->setValue(dataMng.getCurrentPhaseDeg());  // 기본값
            currentPhaseDial->setNotchesVisible(true);

            currentPhaseDisplay = new QLabel(QString::number(dataMng.getCurrentPhaseDeg())+"°");

            dialLayout->addWidget(phaseLabel);
            dialLayout->addWidget(currentPhaseDial);
            dialLayout->addWidget(currentPhaseDisplay);

            outerFrame->setLayout(dialLayout);
            vl->addWidget(outerFrame);

            connect(currentPhaseDial, &QDial::valueChanged, this, &inputWidget::currentPhaseChanged);
        }

        fSlideEdit = new slideEdit("주파수(Hz)", dataMng.getInputFreq(), 100, 1);
        vl->addWidget(fSlideEdit);

        tsSlideEdit = new slideEdit("시간 비율", dataMng.getTimeScale(), 100, 1);
        vl->addWidget(tsSlideEdit);


        connect(vSlideEdit, &slideEdit::valueChanged, this, &inputWidget::inputChanged);
        connect(fSlideEdit, &slideEdit::valueChanged, this, &inputWidget::frequencyChanged);
        connect(tsSlideEdit, &slideEdit::valueChanged, this, &inputWidget::timeScaleChanged);
        connect(iSlideEdit, &slideEdit::valueChanged, this, &inputWidget::currentValueChanged);

    }


    // 샘플링 관련 설정 박스
    QGroupBox* samplingGroupBox = new QGroupBox("샘플링 설정");
    samplingGroupBox->setFont(QFont("맑은 고딕", 10));
    auto groupLayout = new QVBoxLayout(samplingGroupBox);

    // A. 초당 사이클 수
    {
        auto hl = new QHBoxLayout();
        hl->setContentsMargins(0, 0, 0, 0);
        hl->setSpacing(10);

        QLabel *label = new QLabel("A. 초당 cycle 수");
        samplingCycleSpinBox = new QSpinBox();
        samplingCycleSpinBox->setRange(1, 50);
        samplingCycleSpinBox->setValue(dataMng.getCyclePerSecond());

        hl->addWidget(label);
        hl->addStretch();
        hl->addWidget(samplingCycleSpinBox);

        connect(samplingCycleSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &inputWidget::intervalChanged);

        groupLayout->addLayout(hl);
    }

    // B. 사이클당 샘플 수
    {
        auto hl = new QHBoxLayout();
        hl->setContentsMargins(0, 0, 0, 0);
        hl->setSpacing(10);

        QLabel *label = new QLabel("B. Cycle당 sample 수");
        cycleSampleNumSpinBox = new QSpinBox();
        cycleSampleNumSpinBox->setRange(1, 50);
        cycleSampleNumSpinBox->setValue(dataMng.getSamplePerCycle());

        hl->addWidget(label);
        hl->addStretch();
        hl->addWidget(cycleSampleNumSpinBox);

        connect(cycleSampleNumSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &inputWidget::intervalChanged);

        groupLayout->addLayout(hl);
    }

    // 시간 간격 표시
    {
        auto hl = new QHBoxLayout();
        hl->setContentsMargins(0, 0, 0, 0);
        hl->setSpacing(10);

        QLabel *label = new QLabel("시간 간격:");
        timeInterVal = new QLabel("0.0ms");

        hl->addWidget(label);
        hl->addStretch();
        hl->addWidget(timeInterVal);

        groupLayout->addLayout(hl);
    }

    // 초기 값 설정
    float interval_ms = (1000.0f / (samplingCycleSpinBox->value() * cycleSampleNumSpinBox->value()));
    timeInterVal->setText(QString::number(interval_ms) + "ms");
    dataMng.setTimeInterval(interval_ms);

    // 마지막으로 메인 레이아웃에 추가
    vl->addWidget(samplingGroupBox);


}


//전압값 변경시 전압값 데이터 변경
void inputWidget::inputChanged(float newVal)
{
    dataMng.setInputValue(newVal);
}

//시간간격 계산 -> data 변경
void inputWidget::intervalChanged()
{
    int cps = samplingCycleSpinBox->value();
    int spc = cycleSampleNumSpinBox->value();

    float interval_ms = (1000.0f / (cps * spc));
    timeInterVal->setText(QString::number(interval_ms) + "ms");

    // dataMng에 반영
    dataMng.setCyclePerSecond(cps);
    dataMng.setSamplePerCycle(spc);
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

// 전류 값 변경 전류값 데이터 변경
void inputWidget::currentValueChanged(float newVal)
{
    dataMng.setCurrentValue(newVal);
}

// 전류 위상값 변경 전류 위상값 데이터 변경
void inputWidget::currentPhaseChanged(float newVal)
{
    currentPhaseDisplay->setText(QString::number(newVal) + "°");
    dataMng.setCurrentPhaseDeg(newVal);
}

//inputWidget 저장된 값을 불러올 때 새로고침?
void inputWidget::refreshFromData()
{
    vSlideEdit->setValue(dataMng.getInputValue());
    fSlideEdit->setValue(dataMng.getInputFreq());
    tsSlideEdit->setValue(dataMng.getTimeScale());
    iSlideEdit->setValue(dataMng.getCurrentValue());
    currentPhaseDial->setValue(dataMng.getCurrentPhaseDeg());

    // 신호 막기
    samplingCycleSpinBox->blockSignals(true);
    cycleSampleNumSpinBox->blockSignals(true);

    samplingCycleSpinBox->setValue(dataMng.getCyclePerSecond());
    cycleSampleNumSpinBox->setValue(dataMng.getSamplePerCycle());

    // 신호 다시 허용
    samplingCycleSpinBox->blockSignals(false);
    cycleSampleNumSpinBox->blockSignals(false);

    float interval = 1000.0 / (dataMng.getCyclePerSecond() * dataMng.getSamplePerCycle());
    timeInterVal->setText(QString::number(interval) + " ms");

}


