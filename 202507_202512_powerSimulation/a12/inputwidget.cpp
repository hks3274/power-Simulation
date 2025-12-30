#include "inputwidget.h"
#include <QBoxLayout>
#include <Qt>
#include <QGroupBox>
#include <QRandomGenerator>

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
        vSlideEdit = new slideEdit("전압값(V)", dataMng.getInputValue(), 500.0, -500.0);
        vl->addWidget(vSlideEdit);

        QFont boldFont("나눔고딕", 10, QFont::Bold);

        //전류 위상 변경용 다이얼 위젯 사용
        {
            auto dialLayout = new QHBoxLayout(outerFrame);

            iSlideEdit = new slideEdit("전류값(A)", dataMng.getCurrentValue(), 20.0, -20.0);
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

        fSlideEdit = new slideEdit("주파수(Hz)", dataMng.getInputFreq(), 60.0, 1.0);
        vl->addWidget(fSlideEdit);

        // fSlideEdit->setVisible(false);
        // auto fButton = new QPushButton("주파수 값 랜덤", this);
        // vl->addWidget(fButton);

        // connect(fButton, &QPushButton::clicked, this, [this]() {
        //     static std::mt19937 rng(std::random_device{}());
        //     std::uniform_real_distribution<double> dist(0.1, 60.0);

        //     double randomFreq = dist(rng);
        //     this->dataMng.setInputFreq(randomFreq);
        //     this->fSlideEdit->setValue(randomFreq);
        // });


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
        samplingCycleSpinBox = new QDoubleSpinBox();
        samplingCycleSpinBox->setRange(1, 60);
        samplingCycleSpinBox->setValue(dataMng.getCyclePerSecond());

        hl->addWidget(label);
        hl->addStretch();
        hl->addWidget(samplingCycleSpinBox);

        connect(samplingCycleSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &inputWidget::intervalChanged);

        groupLayout->addLayout(hl);
    }

    // B. 사이클당 샘플 수
    {
        auto hl = new QHBoxLayout();
        hl->setContentsMargins(0, 0, 0, 0);
        hl->setSpacing(10);

        QLabel *label = new QLabel("B. Cycle당 sample 수");
        cycleSampleNumSpinBox = new QSpinBox();
        cycleSampleNumSpinBox->setRange(1, 60);
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
    double interval_ms = (1000.0f / (samplingCycleSpinBox->value() * cycleSampleNumSpinBox->value()));
    timeInterVal->setText(QString::number(interval_ms) + "ms");
    dataMng.setTimeInterval(interval_ms);

    // 마지막으로 메인 레이아웃에 추가
    vl->addWidget(samplingGroupBox);


    {
        QHBoxLayout *hlayout = new QHBoxLayout();

        QLabel *graphWidthLabel = new QLabel();
        graphWidthLabel->setText("그래프 폭");  // setText()로 텍스트 지정

        // 스핀박스 생성
        widthSpinBox = new QDoubleSpinBox();
        widthSpinBox->setMinimum(0.01);
        widthSpinBox->setMaximum(10.0);
        widthSpinBox->setValue(dataMng.getGraphWidth());

        hlayout->addWidget(graphWidthLabel);
        hlayout->addWidget(widthSpinBox);

        vl-> addLayout(hlayout);

        connect(widthSpinBox, &QDoubleSpinBox::valueChanged, this, &inputWidget::widthChanged);
    }

    {
        QGroupBox* modeGroupBox = new QGroupBox("화면 갱신 모드");
        modeGroupBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        QHBoxLayout* groupBoxLayout = new QHBoxLayout(modeGroupBox);

        updateModeGroup = new QButtonGroup(this);
        oneSampleBtn = new QRadioButton("1 Sample");
        halfCycleBtn = new QRadioButton("Half Cycle");
        fullCycleBtn = new QRadioButton("1 Cycle");

        updateModeGroup->addButton(oneSampleBtn, 0);
        updateModeGroup->addButton(halfCycleBtn, 1);
        updateModeGroup->addButton(fullCycleBtn, 2);

        groupBoxLayout->addWidget(oneSampleBtn);
        groupBoxLayout->addWidget(halfCycleBtn);
        groupBoxLayout->addWidget(fullCycleBtn);

        // 현재 설정 반영
        switch (dataMng.getUpdateMode()) {
        case updateMode::OneSample:
            oneSampleBtn->setChecked(true);
            break;
        case updateMode::HalfCycle:
            halfCycleBtn->setChecked(true);
            break;
        case updateMode::FullCycle:
            fullCycleBtn->setChecked(true);
            break;
        }

        // 기존 layout에 추가
        vl->addWidget(modeGroupBox);

        connect(updateModeGroup, &QButtonGroup::buttonClicked,this, &inputWidget::graphSettingChange);
    }


}


//전압값 변경시 전압값 데이터 변경
void inputWidget::inputChanged(double newVal)
{
    dataMng.setInputValue(newVal);
}

//시간간격 계산 -> data 변경
void inputWidget::intervalChanged()
{
    double cps = samplingCycleSpinBox->value();
    int spc = cycleSampleNumSpinBox->value();

    const double interval_ms = 1000.0 / (cps * static_cast<double>(spc));
    timeInterVal->setText(QString::number(interval_ms, 'f', 6) + " ms");

    // dataMng에 반영
    dataMng.setCyclePerSecond(cps);
    dataMng.setSamplePerCycle(spc);
    dataMng.setTimeInterval(interval_ms);
}

//주파수 값 변경시 주파수 data 변경
void inputWidget::frequencyChanged(double newVal)
{
    dataMng.setInputFreq(newVal);
}

// 시간 비율 변경 시 시간비율 data 변경
void inputWidget::timeScaleChanged(double newVal)
{
    dataMng.setTimeScale(newVal);
}

// 전류 값 변경 전류값 데이터 변경
void inputWidget::currentValueChanged(double newVal)
{
    dataMng.setCurrentValue(newVal);
}

// 전류 위상값 변경 전류 위상값 데이터 변경
void inputWidget::currentPhaseChanged(double newVal)
{
    currentPhaseDisplay->setText(QString::number(newVal) + "°");
    dataMng.setCurrentPhaseDeg(newVal);
}

// 그래프 폭 변경 시 데이터 변경
void inputWidget::widthChanged(double newVal) {
    dataMng.setGraphWidth(newVal);

}

void inputWidget::graphSettingChange()
{
    int index = updateModeGroup->checkedId();

    switch (index) {
    case 0:
        dataMng.setUpdateMode(updateMode::OneSample);
        break;
    case 1:
        dataMng.setUpdateMode(updateMode::HalfCycle);
        break;
    case 2:
        dataMng.setUpdateMode(updateMode::FullCycle);
        break;
    }
}

//inputWidget 저장된 값을 불러올 때 새로고침?
void inputWidget::refreshFromData()
{
    vSlideEdit->setValue(dataMng.getInputValue());
    fSlideEdit->setValue(dataMng.getInputFreq());
    tsSlideEdit->setValue(dataMng.getTimeScale());
    iSlideEdit->setValue(dataMng.getCurrentValue());
    currentPhaseDial->setValue(dataMng.getCurrentPhaseDeg());

    //sampleCycleSpnBox를 조정하면 intervalChanged 함수가 실행된다. -> 그러면서 동시 cycleSampleNumSpinBox의 값이 setValue와 동시에 다른곳에서도 사용되고 잇는 것이다
    //이러한 결과는 그래프가 변경되지 않게 되는 결과를 만들었고 이를 위해 block시그널을 사용해 신호를 차단하고 값을 변경하고 다시 신호를 보낼 수 잇도록 하였다/
    // 신호를 다시 보낸 이유는 값을 불러오고 난 뒤에 사용자가 수동으로 값을 변경하면 적용이 되지 않기 때문이다.

    // 신호 막기
    samplingCycleSpinBox->blockSignals(true);
    cycleSampleNumSpinBox->blockSignals(true);

    samplingCycleSpinBox->setValue(dataMng.getCyclePerSecond());
    cycleSampleNumSpinBox->setValue(dataMng.getSamplePerCycle());

    // 신호 다시 허용
    samplingCycleSpinBox->blockSignals(false);
    cycleSampleNumSpinBox->blockSignals(false);

    double interval = 1000.0 / (dataMng.getCyclePerSecond() * dataMng.getSamplePerCycle());
    timeInterVal->setText(QString::number(interval) + " ms");
    dataMng.setTimeInterval(interval);

    // ---- 그래프 폭 반영 ----
    if (widthSpinBox) {
        // 값만 반영하고 valueChanged 신호는 막기
        const QSignalBlocker blocker(widthSpinBox); // 또는 widthSpinBox->blockSignals(true/false);
        widthSpinBox->setValue(dataMng.getGraphWidth());
    }

    // ---- 화면 갱신 모드(신호 갱신 주기) 반영 ----
    if (updateModeGroup) {
        updateModeGroup->blockSignals(true); // 버튼 클릭 시그널( buttonClicked ) 방지

        switch (dataMng.getUpdateMode()) {
        case updateMode::OneSample:
            if (oneSampleBtn) oneSampleBtn->setChecked(true);
            break;
        case updateMode::HalfCycle:
            if (halfCycleBtn) halfCycleBtn->setChecked(true);
            break;
        case updateMode::FullCycle:
            if (fullCycleBtn) fullCycleBtn->setChecked(true);
            break;
        }

        updateModeGroup->blockSignals(false);
    }

}

void inputWidget::setCpsFromTracker(double fs)
{
    // 스핀박스 값만 반영 (valueChanged 막음)
    QSignalBlocker b1(samplingCycleSpinBox);
    samplingCycleSpinBox->setValue(fs);

    // 라벨/타이머 동기화 (intervalChanged 호출 대신 직접 반영)
    const int spc = cycleSampleNumSpinBox->value();
    const double interval_ms = 1000.0 / (fs * std::max(1, spc));
    timeInterVal->setText(QString::number(interval_ms, 'f', 6) + " ms");

    // 모델 쪽 타이머 간격만 맞춰줌(주기/샘플수는 이미 최신일 것)
    dataMng.setTimeInterval(interval_ms);
}
