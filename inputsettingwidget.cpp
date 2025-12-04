#include "inputsettingwidget.h"
#include <QGroupBox>
#include <QBoxLayout>

inputSettingWidget::inputSettingWidget(dataManagement& dataMng)
    : dataMng(dataMng)
{
    auto vl = new QVBoxLayout(this);

    //전압값과 주파수값 생성
    {
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


        connect(fSlideEdit, &slideEdit::valueChanged, this, &inputSettingWidget::frequencyChanged);
        connect(tsSlideEdit, &slideEdit::valueChanged, this, &inputSettingWidget::timeScaleChanged);

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

        QLabel *label = new QLabel("초당 cycle 수");
        samplingCycleSpinBox = new QDoubleSpinBox();
        samplingCycleSpinBox->setRange(1, 100);
        samplingCycleSpinBox->setValue(dataMng.getCyclePerSecond());

        hl->addWidget(label);
        hl->addStretch();
        hl->addWidget(samplingCycleSpinBox);

        connect(samplingCycleSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &inputSettingWidget::intervalChanged);

        groupLayout->addLayout(hl);
    }

    // B. 사이클당 샘플 수
    {
        auto hl = new QHBoxLayout();
        hl->setContentsMargins(0, 0, 0, 0);
        hl->setSpacing(10);

        QLabel *label = new QLabel("Cycle당 sample 수");
        cycleSampleNumSpinBox = new QSpinBox();
        cycleSampleNumSpinBox->setRange(1, 200);
        cycleSampleNumSpinBox->setValue(dataMng.getSamplePerCycle());

        hl->addWidget(label);
        hl->addStretch();
        hl->addWidget(cycleSampleNumSpinBox);

        connect(cycleSampleNumSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &inputSettingWidget::intervalChanged);

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
    double interval_sec = 1.0 / (dataMng.getCyclePerSecond() * static_cast<double>(dataMng.getSamplePerCycle())); // 초 단위
    auto interval_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::duration<double>(interval_sec));
    timeInterVal->setText( QString::number(interval_sec * 1000.0, 'f', 6) + " ms");
    dataMng.setTimeInterval(interval_ns);

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

        connect(widthSpinBox, &QDoubleSpinBox::valueChanged, this, &inputSettingWidget::widthChanged);
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

        connect(updateModeGroup, &QButtonGroup::buttonClicked,this, &inputSettingWidget::graphSettingChange);
    }


}


//시간간격 계산 -> data 변경
void inputSettingWidget::intervalChanged()
{
    double cps = samplingCycleSpinBox->value();
    int spc = cycleSampleNumSpinBox->value();

    // --- ms 단위 대신, 바로 나노초 단위 계산 ---
    double interval_sec = 1.0 / (cps * static_cast<double>(spc)); // 초 단위
    auto interval_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::duration<double>(interval_sec));

    timeInterVal->setText(QString::number(interval_sec * 1000.0, 'f', 6) + " ms");

    // dataMng에 반영
    dataMng.setCyclePerSecond(cps);
    dataMng.setSamplePerCycle(spc);
    dataMng.setTimeInterval(interval_ns);
}

//주파수 값 변경시 주파수 data 변경
void inputSettingWidget::frequencyChanged(double newVal)
{
    dataMng.setInputFreq(newVal);
}

// 시간 비율 변경 시 시간비율 data 변경
void inputSettingWidget::timeScaleChanged(double newVal)
{
    dataMng.setTimeScale(newVal);
}

// 그래프 폭 변경 시 데이터 변경
void inputSettingWidget::widthChanged(double newVal) {
    dataMng.setGraphWidth(newVal);

}

void inputSettingWidget::graphSettingChange()
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
void inputSettingWidget::refreshFromData()
{

    fSlideEdit->setValue(dataMng.getInputFreq());
    tsSlideEdit->setValue(dataMng.getTimeScale());

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

    // --- ms 단위 대신, 바로 나노초 단위 계산 ---
    double interval_sec = 1.0 / (dataMng.getCyclePerSecond() * static_cast<double>(dataMng.getSamplePerCycle())); // 초 단위
    auto interval_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::duration<double>(interval_sec));

    timeInterVal->setText(QString::number(interval_sec * 1000.0, 'f', 6) + " ms");
    dataMng.setTimeInterval(interval_ns);

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

void inputSettingWidget::setCpsFromTracker(double fs)
{
    // 스핀박스 값만 반영 (valueChanged 막음)
    QSignalBlocker b1(samplingCycleSpinBox);
    samplingCycleSpinBox->setValue(fs);

    // 라벨/타이머 동기화 (intervalChanged 호출 대신 직접 반영)
    const int spc = cycleSampleNumSpinBox->value();
    // --- ms 단위 대신, 바로 나노초 단위 계산 ---
    double interval_sec = 1.0 / (fs * static_cast<double>(spc)); // 초 단위
    auto interval_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::duration<double>(interval_sec));


    timeInterVal->setText(QString::number(interval_sec * 1000.0, 'f', 6) + " ms");

    // dataMng에 반영
    dataMng.setTimeInterval(interval_ns);
}

