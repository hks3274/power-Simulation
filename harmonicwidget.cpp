 #include "harmonicwidget.h"


harmonicwidget::harmonicwidget(dataManagement& dataMng)
    : dataMng(dataMng)
{
    auto* tabWidget = new QTabWidget(this);

    // --- 전압 탭 ---
    QWidget* vTab = new QWidget();
    auto* vLayout = new QVBoxLayout(vTab);

    vOrderSpin = new QSpinBox();
    vOrderSpin->setRange(1, 50);
    vOrderSpin->setValue(dataMng.getVHarmonicOrder());

    vMagSpin = new QDoubleSpinBox();
    vMagSpin->setRange(0, 100);
    vMagSpin->setDecimals(1);      // 소숫점 1자리까지 허용
    vMagSpin->setSingleStep(0.1);  // 0.1 단위로 증감
    vMagSpin->setValue(dataMng.getVHarmonicMag());

    vPhaseDial = new QDial();
    vPhaseDial->setRange(-180, 180);
    vPhaseDial->setValue(dataMng.getVHarmonicPhase());
    vPhaseDial->setNotchesVisible(true);

    vPhaseSpin = new QSpinBox();
    vPhaseSpin->setRange(-180, 180);
    vPhaseSpin->setValue(dataMng.getVHarmonicPhase());

    // 다이얼 → 스핀박스
    QObject::connect(vPhaseDial, &QDial::valueChanged, vPhaseSpin, &QSpinBox::setValue);
    // 스핀박스 → 다이얼
    QObject::connect(vPhaseSpin, QOverload<int>::of(&QSpinBox::valueChanged), vPhaseDial, &QDial::setValue);

    vLayout->addWidget(new QLabel("고조파 차수"));
    vLayout->addWidget(vOrderSpin);
    vLayout->addWidget(new QLabel("고조파 크기"));
    vLayout->addWidget(vMagSpin);
    vLayout->addWidget(new QLabel("고조파 위상"));
    vLayout->addWidget(vPhaseDial);
    vLayout->addWidget(vPhaseSpin);

    tabWidget->addTab(vTab, "전압");


    connect(vOrderSpin, &QSpinBox::valueChanged, this, [&](int val) {
        dataMng.setVHarmonicOrder(val);
    } );
    connect(vMagSpin, &QDoubleSpinBox::valueChanged, this, [&](double val) {
        dataMng.setVHarmonicMag(val);
    } );
    connect(vPhaseSpin, &QSpinBox::valueChanged, this, [&](int val) {
        dataMng.setVHarmonicPhase(val);
    } );

    // --- 전류 탭 ---
    QWidget* cTab = new QWidget();
    auto* cLayout = new QVBoxLayout(cTab);

    cOrderSpin = new QSpinBox();
    cOrderSpin->setRange(1, 50);
    cOrderSpin->setValue(dataMng.getCHarmonicOrder());

    cMagSpin = new QDoubleSpinBox();
    cMagSpin->setRange(0, 100);
    cMagSpin->setDecimals(1);      // 소숫점 1자리까지 허용
    cMagSpin->setSingleStep(0.1);  // 0.1 단위로 증감
    cMagSpin->setValue(dataMng.getCHarmonicMag());

    cPhaseDial = new QDial();
    cPhaseDial->setRange(-180, 180);
    cPhaseDial->setValue(dataMng.getCHarmonicPhase());
    cPhaseDial->setNotchesVisible(true);

    cPhaseSpin = new QSpinBox();
    cPhaseSpin->setRange(-180, 180);
    cPhaseSpin->setValue(dataMng.getCHarmonicPhase());

    // 다이얼 → 스핀박스
    QObject::connect(cPhaseDial, &QDial::valueChanged, cPhaseSpin, &QSpinBox::setValue);
    // 스핀박스 → 다이얼
    QObject::connect(cPhaseSpin, QOverload<int>::of(&QSpinBox::valueChanged), cPhaseDial, &QDial::setValue);


    cLayout->addWidget(new QLabel("고조파 차수"));
    cLayout->addWidget(cOrderSpin);
    cLayout->addWidget(new QLabel("고조파 크기"));
    cLayout->addWidget(cMagSpin);
    cLayout->addWidget(new QLabel("고조파 위상"));
    cLayout->addWidget(cPhaseDial);
    cLayout->addWidget(cPhaseSpin);

    tabWidget->addTab(cTab, "전류");

    connect(cOrderSpin, &QSpinBox::valueChanged, this, [&](int val) {
        dataMng.setCHarmonicOrder(val);
    } );
    connect(cMagSpin, &QDoubleSpinBox::valueChanged, this, [&](double val) {
        dataMng.setCHarmonicMag(val);
    } );
    connect(cPhaseSpin, &QSpinBox::valueChanged, this, [&](int val) {
        dataMng.setCHarmonicPhase(val);
    } );

    // 메인 레이아웃
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(tabWidget);
    setLayout(mainLayout);
}

void harmonicwidget::refreshFromData()
{
    // === 전압 관련 위젯 업데이트 ===
    // 신호 블록하여 무한 루프 방지
    vOrderSpin->blockSignals(true);
    vMagSpin->blockSignals(true);
    vPhaseSpin->blockSignals(true);
    vPhaseDial->blockSignals(true);

    // 데이터에서 현재 값 가져와서 설정
    vOrderSpin->setValue(dataMng.getVHarmonicOrder());
    vMagSpin->setValue(dataMng.getVHarmonicMag());
    vPhaseSpin->setValue(dataMng.getVHarmonicPhase());
    vPhaseDial->setValue(dataMng.getVHarmonicPhase());

    // 신호 블록 해제
    vOrderSpin->blockSignals(false);
    vMagSpin->blockSignals(false);
    vPhaseSpin->blockSignals(false);
    vPhaseDial->blockSignals(false);

    // === 전류 관련 위젯 업데이트 ===
    // 신호 블록하여 무한 루프 방지
    cOrderSpin->blockSignals(true);
    cMagSpin->blockSignals(true);
    cPhaseSpin->blockSignals(true);
    cPhaseDial->blockSignals(true);

    // 데이터에서 현재 값 가져와서 설정
    cOrderSpin->setValue(dataMng.getCHarmonicOrder());
    cMagSpin->setValue(dataMng.getCHarmonicMag());
    cPhaseSpin->setValue(dataMng.getCHarmonicPhase());
    cPhaseDial->setValue(dataMng.getCHarmonicPhase());

    // 신호 블록 해제
    cOrderSpin->blockSignals(false);
    cMagSpin->blockSignals(false);
    cPhaseSpin->blockSignals(false);
    cPhaseDial->blockSignals(false);
}
