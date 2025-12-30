#include "a3700n_gui.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QToolBar>
#include <QActionGroup>
#include <QStackedWidget>
#include <QListWidget>
#include <QLabel>
#include <QFrame>
#include <QFile>
#include <QStyleFactory>
#include <QDebug>
#include <Qmenubar>

#include "a3700n_harm.h"
#include "a3700n_phasor.h"
#include "a3700n_wave.h"
#include "mainwindow.h"
#include "inputmainwindow.h"
#include "onesecmainwindow.h"

A3700N_GUI::A3700N_GUI(dataManagement& dataMng)
    : dataMng(dataMng)
{
    setWindowTitle("A3700N TEST");
    QScreen *screen = QGuiApplication::primaryScreen();
    qreal scale = screen->devicePixelRatio();

    const int baseWidth  = 800;
    const int baseHeight = 430;

    setFixedSize(baseWidth/ scale, baseHeight/ scale);

    setStyle(QStyleFactory::create("Fusion")); // 전반 적인 시각 스타일을 fusion 테마로 변경한다 -> 일정한 스타일로 통일한다.
    loadQSS(":/styles/a3700n_style.qss");

    QMenuBar* menubar = menuBar();
    QMenu* windowMenu = menubar->addMenu("창열기");

    QAction* openMainWin   = new QAction("그래프창 열기", this);
    QAction* openOneSecWin = new QAction("1초 데이터창 열기", this);
    QAction* openInputWin = new QAction("입력창 열기", this);
    windowMenu->addAction(openInputWin);
    windowMenu->addAction(openMainWin);
    windowMenu->addAction(openOneSecWin);

    connect(openMainWin, &QAction::triggered, [this]() {
        if (mainWindow) {
            mainWindow->show();
            mainWindow->raise();
            mainWindow->activateWindow();
        }
    });
    connect(openOneSecWin, &QAction::triggered, [this]() {
        if (oneSecWindow) {
            oneSecWindow->show();
            oneSecWindow->raise();
            oneSecWindow->activateWindow();
        }
    });

    connect(openInputWin, &QAction::triggered, [this]() {
        if (inputWindow) {
            inputWindow->show();
            inputWindow->raise();
            inputWindow->activateWindow();
        }
    });


    // === 상단 툴바 ===
    QToolBar* toolbar = new QToolBar(this);
    toolbar->setMovable(false);
    toolbar->setFloatable(false);
    toolbar->setFixedHeight(40);
    toolbar->setContextMenuPolicy(Qt::PreventContextMenu);
    addToolBar(Qt::TopToolBarArea, toolbar);

    QAction* actVoltage = new QAction("VOLTAGE", this);
    QAction* actCurrent = new QAction("CURRENT", this);
    QAction* actPower = new QAction("POWER", this);
    QAction* actAnalysis = new QAction("ANALYSIS", this);
    actVoltage->setCheckable(true);
    actCurrent->setCheckable(true);
    actPower->setCheckable(true);
    actAnalysis->setCheckable(true);
    actVoltage->setChecked(true);

    QActionGroup* group = new QActionGroup(this);
    group->addAction(actVoltage);
    group->addAction(actCurrent);
    group->addAction(actPower);
    group->addAction(actAnalysis);
    group->setExclusive(true);

    toolbar->addAction(actVoltage);
    toolbar->addAction(actCurrent);
    toolbar->addAction(actPower);
    toolbar->addAction(actAnalysis);

    // === 메인 스택 (전체 전환) ===
    mainStack = new QStackedWidget(this);
    setCentralWidget(mainStack);

    // === VOLTAGE 페이지 생성 ===
    QWidget* voltagePage = createPage(
        {"voltage", "RMS", "Fundamental", "THD %", "Frequency", "Residual"}
        );

    // === CURRENT 페이지 생성 ===
    QWidget* currentPage = createPage(
        {"current", "RMS", "Fundamental", "THD %", "Residual"}
        );

    // === POWER 페이지 생성 ===
    QWidget* powerPage = createPage(
        {"power", "Active(P)", "Reactive(Q)", "Apparent(S)", "PF", "Energy"}
        );

    // === ANALYSIS ===
    QWidget* analysisPage = createPage(
        {"analysis", "Phasor", "Harmonics", "Waveform", "Volt. Symm.", "Volt. Unbal. %", "Curr. Symm.", "Curr. Unbal. %"}
        );

    mainStack->addWidget(voltagePage); // index 0
    mainStack->addWidget(currentPage); // index 1
    mainStack->addWidget(powerPage); // index 2
    mainStack->addWidget(analysisPage); // index 3

    // 각 페이지 마다 리스트 생성
    connect(actVoltage, &QAction::triggered, this, [=, this]() {
        mainStack->setCurrentWidget(voltagePage);
    });
    connect(actCurrent, &QAction::triggered, this, [=, this]() {
        mainStack->setCurrentWidget(currentPage);
    });
    connect(actPower, &QAction::triggered, this, [=, this]() {
        mainStack->setCurrentWidget(powerPage);
    });
    connect(actAnalysis, &QAction::triggered, this, [=, this]() {
        mainStack->setCurrentWidget(analysisPage);
    });

}

QWidget* A3700N_GUI::createPage(const QStringList& listNames)
{
    QWidget* page = new QWidget(this);
    page->setStyleSheet("border: none;");
    QHBoxLayout* mainLay = new QHBoxLayout(page);
    mainLay->setContentsMargins(0, 0, 0, 0);
    mainLay->setSpacing(0);

    // === 왼쪽 리스트 ===
    QListWidget* listWidget = new QListWidget(page);

    QFont listFont("맑은 고딕", 11, 600);
    listWidget->setFont(listFont);

    // listNames[0]는 리스트에 추가하지 않음
    for (int i = 1; i < listNames.size(); ++i) {
        listWidget->addItem(listNames[i]);
    }

    listWidget->setCurrentRow(0);
    mainLay->addWidget(listWidget);

    // === 오른쪽 스택 ===
    QStackedWidget* rightStack = new QStackedWidget(page);

    // 각 항목마다 페이지 생성
    QString baseKey = listNames[0];  // "voltage" or "current"

    for (int i = 1; i < listNames.size(); ++i) {
        QWidget* rightArea = new QWidget(page);
        rightArea->setObjectName("content");
        rightArea->setProperty("itemKey", listNames[i]);  // "RMS", "THD %", ...

        QVBoxLayout* vLay = new QVBoxLayout(rightArea);
        vLay->setContentsMargins(20, 30, 20, 30);
        vLay->setSpacing(10);  // 항목 간 간격 (기본 spacing)
        vLay->setAlignment(Qt::AlignTop); // 위쪽 정렬


        // === 제목 ===


        QLabel* title = new QLabel(listNames[i], rightArea);
        title->setObjectName("title");

        if(baseKey == "voltage") {
            if(listNames[i] == "RMS") {title->setText("RMS Voltage");}
            else if(listNames[i] == "Fundamental") { title->setText("Fundamental Voltage");}
            else if (listNames[i] == "THD %") { title->setText("Total Harmonic Distortion");}
            else if(listNames[i] == "Residual") { title->setText("Residual Voltage");}

            if(listNames[i] == "RMS") {
                QHBoxLayout* titleLay = new QHBoxLayout();

                rmsLLBtn = new QPushButton("L-L");
                rmsLLBtn->setCheckable(true);
                rmsLLBtn->setObjectName("harmBtn");
                rmsLNBtn = new QPushButton("L-N");
                rmsLNBtn->setCheckable(true);
                rmsLNBtn->setObjectName("harmBtn");

                QButtonGroup* btnGroup = new QButtonGroup();
                btnGroup->addButton(rmsLLBtn);
                btnGroup->addButton(rmsLNBtn);
                btnGroup->setExclusive(true);

                rmsLLBtn->setChecked(true);

                titleLay->addWidget(title);
                titleLay->addStretch(1);
                titleLay->addWidget(rmsLLBtn);
                titleLay->addWidget(rmsLNBtn);

                vLay->addLayout(titleLay);

                connect(rmsLLBtn, &QPushButton::toggled, this, [this](bool checked){
                    if (lastOsl)
                        updateOneSec(lastOsl);
                });
            }
            else if (listNames[i] == "Fundamental"){
                QHBoxLayout* titleLay = new QHBoxLayout();

                fundLLBtn = new QPushButton("L-L");
                fundLLBtn->setCheckable(true);
                fundLLBtn->setObjectName("harmBtn");
                fundLNBtn = new QPushButton("L-N");
                fundLNBtn->setCheckable(true);
                fundLNBtn->setObjectName("harmBtn");

                QButtonGroup* btnGroup = new QButtonGroup();
                btnGroup->addButton(fundLLBtn);
                btnGroup->addButton(fundLNBtn);
                btnGroup->setExclusive(true);

                fundLLBtn->setChecked(true);

                titleLay->addWidget(title);
                titleLay->addStretch(1);
                titleLay->addWidget(fundLLBtn);
                titleLay->addWidget(fundLNBtn);

                vLay->addLayout(titleLay);

                connect(fundLLBtn, &QPushButton::toggled, this, [this](bool checked){
                    if (lastOsl)
                        updateOneSec(lastOsl);
                });

            }
            else if (listNames[i] == "THD %"){
                QHBoxLayout* titleLay = new QHBoxLayout();

                thdLLBtn = new QPushButton("L-L");
                thdLLBtn->setCheckable(true);
                thdLLBtn->setObjectName("harmBtn");
                thdLNBtn = new QPushButton("L-N");
                thdLNBtn->setCheckable(true);
                thdLNBtn->setObjectName("harmBtn");

                QButtonGroup* btnGroup = new QButtonGroup();
                btnGroup->addButton(thdLLBtn);
                btnGroup->addButton(thdLNBtn);
                btnGroup->setExclusive(true);

                thdLLBtn->setChecked(true);

                titleLay->addWidget(title);
                titleLay->addStretch(1);
                titleLay->addWidget(thdLLBtn);
                titleLay->addWidget(thdLNBtn);

                vLay->addLayout(titleLay);

                connect(thdLLBtn, &QPushButton::toggled, this, [this](bool checked){
                    if (lastOsl)
                        updateOneSec(lastOsl);
                });

            }
            else{
                vLay->addWidget(title);
            }

        }
        if(baseKey == "current") {
            if(listNames[i] == "RMS") { title->setText("RMS Current");}
            else if(listNames[i] == "Fundamental") { title->setText("Fundamental Current");}
            else if(listNames[i] == "Residual") { title->setText("Residual Current");}
            vLay->addWidget(title);
        }
        if(baseKey == "power"){
            if(listNames[i] == "Active(P)" ) { title->setText( "Active Power"); }
            else if(listNames[i] == "Reactive(Q)" ) { title->setText( "Reactive Power"); }
            else if(listNames[i] == "Apparent(S)" ) { title->setText( "Apparent Power"); }
            else if(listNames[i] == "PF" ) { title->setText( "Power Factor");}
            vLay->addWidget(title);
        }
        if(baseKey == "analysis"){
            if (listNames[i] == "Volt. Symm." ) {  title->setText( "Symmentrical Component");}
            else if (listNames[i] == "Volt. Unbal. %" ) { title->setText( "Voltage Unbalance");}
            else if (listNames[i] == "Curr. Symm." ) { title->setText( "Curr. Symm. Component");}
            else if (listNames[i] == "Curr. Unbal. %" ) { title->setText( "Current Unbalance");}

            if(listNames[i] =="Volt. Symm.") {
                QHBoxLayout* titleLay = new QHBoxLayout();

                symmLLBtn = new QPushButton("L-L");
                symmLLBtn->setCheckable(true);
                symmLLBtn->setObjectName("harmBtn");
                symmLNBtn = new QPushButton("L-N");
                symmLNBtn->setCheckable(true);
                symmLNBtn->setObjectName("harmBtn");

                QButtonGroup* btnGroup = new QButtonGroup();
                btnGroup->addButton(symmLLBtn);
                btnGroup->addButton(symmLNBtn);
                btnGroup->setExclusive(true);

                symmLLBtn->setChecked(true);

                titleLay->addWidget(title);
                titleLay->addStretch(1);
                titleLay->addWidget(symmLLBtn);
                titleLay->addWidget(symmLNBtn);

                vLay->addLayout(titleLay);

                connect(symmLLBtn, &QPushButton::toggled, this, [this](bool checked){
                    if (lastOsl)
                        updateOneSec(lastOsl);
                });
            }else{
                vLay->addWidget(title);
            }
        }
        if(baseKey == "analysis" && listNames[i] == "Phasor"){
            title->setText("Phasor [Vector Diagram]");

            // === 한 줄짜리 수평 레이아웃 ===
            QHBoxLayout* topRow = new QHBoxLayout();

            vLay->setSpacing(5);
            // 왼쪽 라벨
            topRow->addWidget(title);
            topRow->addStretch();  // 운데 여백 확보 (라벨과 체크박스 사이 벌리기)

            // 오른쪽 체크박스 2개
            phasorVolCheck = new QCheckBox("Voltage", rightArea);
            phasorCurCheck = new QCheckBox("Current", rightArea);
            phasorVolCheck->setChecked(true);
            phasorCurCheck->setChecked(true);
            phasorVolCheck->setObjectName("customCheckBox");
            phasorCurCheck->setObjectName("customCheckBox");

            // 체크박스 오른쪽 정렬 (순서대로 Voltage, Current)
            topRow->addWidget(phasorVolCheck, 0, Qt::AlignRight);
            topRow->addWidget(phasorCurCheck, 0, Qt::AlignRight);

            // === 이제 전체 수직 레이아웃에 추가 ===
            vLay->addLayout(topRow);
        }
        if(baseKey == "analysis" && listNames[i] == "Waveform"){
            title->setText(listNames[i]);

            // === 한 줄짜리 수평 레이아웃 ===
            QHBoxLayout* topRow = new QHBoxLayout();

            vLay->setSpacing(5);
            // 왼쪽 라벨
            topRow->addWidget(title);
            topRow->addStretch();  // 운데 여백 확보 (라벨과 체크박스 사이 벌리기)

            // 오른쪽 체크박스 2개
            waveVolCheck = new QCheckBox("Voltage", rightArea);
            waveCurCheck = new QCheckBox("Current", rightArea);
            waveVolCheck->setChecked(true);
            waveCurCheck->setChecked(true);
            waveVolCheck->setObjectName("customCheckBox");
            waveCurCheck->setObjectName("customCheckBox");

            // 체크박스 오른쪽 정렬 (순서대로 Voltage, Current)
            topRow->addWidget(waveVolCheck, 0, Qt::AlignRight);
            topRow->addWidget(waveCurCheck, 0, Qt::AlignRight);

            // === 이제 전체 수직 레이아웃에 추가 ===
            vLay->addLayout(topRow);
        }
        if(baseKey == "analysis" && listNames[i] == "Harmonics") {
            title->setText(listNames[i]);

            QHBoxLayout* topRow = new QHBoxLayout();

            vLay->setSpacing(5);
            topRow->addWidget(title);
            topRow->addStretch();

            harmVoltBtn = new QPushButton("Voltage", rightArea);
            harmCurrBtn = new QPushButton("Current", rightArea);
            harmVoltBtn->setCheckable(true);
            harmCurrBtn->setCheckable(true);
            QButtonGroup* harmGroup = new QButtonGroup(this);
            harmGroup->addButton(harmVoltBtn);
            harmGroup->addButton(harmCurrBtn);
            harmGroup->setExclusive(true);
            harmVoltBtn->setChecked(true);
            harmVoltBtn->setObjectName("harmBtn");
            harmCurrBtn->setObjectName("harmBtn");

            topRow->addWidget(harmVoltBtn, 0, Qt::AlignRight);
            topRow->addWidget(harmCurrBtn, 0, Qt::AlignRight);


            // === 이제 전체 수직 레이아웃에 추가 ===
            vLay->addLayout(topRow);

        }


        // 제목 아래 첫 구분선
        QFrame* titleLine = new QFrame(rightArea);
        titleLine->setFrameShape(QFrame::HLine);
        titleLine->setFrameShadow(QFrame::Plain);
        titleLine->setStyleSheet("background-color:#cccccc; height:1px;");
        vLay->addWidget(titleLine);

        QVector<QLabel*> valueLabels;

        QStringList fields;
        QString unit;

        // === 전압/전류 구분 ===
        if (baseKey == "voltage") {
            if (listNames[i] == "RMS" || listNames[i] == "Fundamental") {
                fields = {"A", "B", "C", "Average"};
                unit = "V";
            }
            else if (listNames[i] == "THD %") {
                fields = {"A", "B", "C"};
                unit = "%";
            }
            else if (listNames[i] == "Frequency") {
                fields = {"Frequency"};
                unit = "Hz";
            }
            else if (listNames[i] == "Residual") {
                fields = {"Residual"};
                unit = "V";
            }
        }
        else if (baseKey == "current") {
            if (listNames[i] == "RMS" || listNames[i] == "Fundamental") {
                fields = {"A", "B", "C", "Average"};
                unit = "A";
            }
            else if (listNames[i] == "THD %") {
                fields = {"A", "B", "C"};
                unit = "%";
            }
            else if (listNames[i] == "Frequency") {
                fields = {"Frequency"};
                unit = "Hz";
            }
            else if (listNames[i] == "Residual") {
                fields = {"Residual"};
                unit = "A";
            }
        }
        else if (baseKey == "power") {
            if (listNames[i] == "Active(P)") {
                fields = {"A", "B", "C", "Total"};
                unit = "kW";
            }
            else if (listNames[i] == "Reactive(Q)") {
                fields = {"A", "B", "C", "Total"};
                unit = "kVAR";
            }
            else if (listNames[i] == "Apparent(S)") {
                fields = {"A", "B", "C", "Total"};
                unit = "kVA";
            }
            else if (listNames[i] == "PF") {
                fields = {"A", "B", "C", "Total"};
                unit = "";
            }
            else if (listNames[i] == "Energy") {
                fields = {"Energy"};
                unit = "kWh";
            }
        }
        else if(baseKey == "analysis") {
            if(listNames[i] == "Volt. Symm.") {
                fields = {"Positive-\nSquence", "Negative-\nSequence", "Zero-\nSequence"};
                unit = "V";
            }
            if(listNames[i] == "Volt. Unbal. %") {
                fields = {"NEMA", "Negative-\nSequence", "Zero-\nSequence"};
                unit = "%";
            }
            if(listNames[i] == "Curr. Symm.") {
                fields = {"Positive-\nSquence", "Negative-\nSequence", "Zero-\nSequence"};
                unit = "A";
            }
            if(listNames[i] == "Curr. Unbal. %") {
                fields = {"NEMA", "Negative-\nSequence", "Zero-\nSequence"};
                unit = "%";
            }

        }


        if (baseKey == "analysis" && listNames[i] == "Phasor") {
            // QWidget을 만들어서 삽입한다.

            phWidget = new a3700n_phasor(dataMng, phasorVolCheck->isChecked(), phasorCurCheck->isChecked());
            vLay->addWidget(phWidget);

            connect(phasorVolCheck, &QCheckBox::toggled, phWidget, &a3700n_phasor::setVolCheck);
            connect(phasorCurCheck, &QCheckBox::toggled, phWidget, &a3700n_phasor::setCurCheck);

        }

        else if (baseKey == "analysis" && listNames[i] == "Harmonics") {

            harmWidget = new a3700n_harm(dataMng);
            vLay->addSpacing(-10);
            vLay->addWidget(harmWidget);

            connect(harmVoltBtn, &QPushButton::toggled, harmWidget, &a3700n_harm::setVoltCheck);
            connect(harmCurrBtn, &QPushButton::toggled, harmWidget, &a3700n_harm::setCurrCheck);

        }

        else if (baseKey == "analysis" && listNames[i] == "Waveform") {
            //waveform 그리기 -> 데이터 연동?
            //Widget을 만들어서 보여준다..

            a3700n_wave* waveWidget = new a3700n_wave(dataMng);
            vLay->addSpacing(-10);
            vLay->addWidget(waveWidget);

            connect(waveVolCheck, &QCheckBox::toggled, waveWidget, &a3700n_wave::setVolCheck);
            connect(waveCurCheck, &QCheckBox::toggled, waveWidget, &a3700n_wave::setCorCheck);
        }
        else {
            // === 각 항목 생성 ===
            for (int j = 0; j < fields.size(); ++j) {
                QHBoxLayout* row = new QHBoxLayout();

                QLabel* name = new QLabel(fields[j], rightArea);
                QLabel* val = new QLabel("--", rightArea);
                QLabel* u = new QLabel(unit, rightArea);


                name->setObjectName("name");
                val->setObjectName("val");
                u->setObjectName("u");

                if (name->text() == "Positive-\nSquence" || name->text() == "Negative-\nSequence" || name->text() == "Zero-\nSequence")
                    name->setObjectName("sequenceName");

                name->setFixedWidth(120);
                val->setFixedWidth(150);
                u->setFixedWidth(70);

                row->addSpacing(20);
                row->addWidget(name);
                row->addStretch();
                row->addWidget(val);
                row->addWidget(u);
                vLay->addLayout(row);

                valueLabels.append(val);

                // 항목 아래 구분선 추가 (마지막 제외)
                if (j != fields.size() - 1) {
                    QFrame* line = new QFrame(rightArea);
                    line->setFrameShape(QFrame::HLine);
                    line->setFrameShadow(QFrame::Plain);
                    line->setStyleSheet("background-color:#e0e0e0; height:1px;");
                    vLay->addWidget(line);
                }
            }
        }

        // === 라벨 리스트 저장 ===
        rightArea->setProperty("valueLabels", QVariant::fromValue(valueLabels));
        rightStack->addWidget(rightArea);
    }


    // === 리스트 클릭 시 스택 전환 ===
    connect(listWidget, &QListWidget::currentRowChanged,
            rightStack, &QStackedWidget::setCurrentIndex);

    mainLay->addWidget(rightStack);
    mainLay->setStretch(0, 1);
    mainLay->setStretch(1, 4);

    page->setProperty("stack", QVariant::fromValue((QObject*)rightStack));
    page->setProperty("dataKey", listNames[0]);
    return page;

}

void A3700N_GUI::updateOneSec(dataManagement::oneSecList* osl) //d여기서 voltage, current Check버튼 값 보내기
{
    if (!osl || !mainStack) return;

    lastOsl = osl; //마지막 데이터 저장;

    auto ph = dataMng.calcPhasor();

    harmWidget->updateHarmData(osl);
    phWidget->updatePhasor(ph);

    // mainStack의 모든 페이지 순회 (voltage, current 등)
    for (int p = 0; p < mainStack->count(); ++p) {
        QWidget* page = mainStack->widget(p);
        QString dataKey = page->property("dataKey").toString(); // "voltage" or "current"
        QStackedWidget* stack = qobject_cast<QStackedWidget*>(page->property("stack").value<QObject*>());
        if (!stack) continue;

        // 각 page 안의 stacked 페이지 순회
        for (int i = 0; i < stack->count(); ++i) {
            QWidget* w = stack->widget(i);
            QString itemKey = w->property("itemKey").toString(); // "RMS", "THD %", "Residual" 등

            QVector<double> vals;

            // === 데이터 매칭 ===
            if (dataKey == "voltage") {
                if (itemKey == "RMS" && rmsLLBtn->isChecked()) vals = osl->voltage.RMS_LL;
                else if (itemKey == "RMS" && rmsLNBtn->isChecked()) vals = osl->voltage.RMS;
                else if (itemKey == "Fundamental" && fundLLBtn->isChecked()) vals = osl->voltage.Fundamental_LL;
                else if (itemKey == "Fundamental" && fundLNBtn->isChecked()) vals = osl->voltage.Fundamental;
                else if (itemKey == "THD %" && thdLLBtn->isChecked()) vals = osl->voltage.THDLL;
                else if (itemKey == "THD %" && thdLNBtn->isChecked()) vals = osl->voltage.THD;
                else if (itemKey == "Frequency") vals = {osl->voltage.Frequency};
                else if (itemKey == "Residual") vals = {osl->voltage.Residual};
            }
            else if (dataKey == "current") {
                if (itemKey == "RMS") vals = osl->current.RMS;
                else if (itemKey == "Fundamental") vals = osl->current.Fundamental;
                else if (itemKey == "THD %") vals = osl->current.THD;
                else if (itemKey == "Residual") vals = {osl->current.Residual};
            }
            else if (dataKey == "power") {
                if (itemKey == "Active(P)") vals = osl->power.active;
                else if (itemKey == "Reactive(Q)") vals = osl->power.reactive;
                else if (itemKey == "Apparent(S)") vals = osl->power.apparent;
                else if (itemKey == "PF") vals = osl->power.pf;
                else if (itemKey == "Energy") vals = {osl->power.energy};
            }
            else if (dataKey == "analysis") {
                if (itemKey == "Volt. Symm." && symmLLBtn->isChecked()) vals = osl->symmUnbal.voltSymm_LL;
                else if (itemKey == "Volt. Symm." && symmLNBtn->isChecked()) vals = osl->symmUnbal.voltSymm;
                else if (itemKey == "Volt. Unbal. %") vals = osl->symmUnbal.voltUnbal;
                else if (itemKey == "Curr. Symm.") vals = osl->symmUnbal.currsymm;
                else if (itemKey == "Curr. Unbal. %") vals = osl->symmUnbal.currUnbal;
            }


            // === 라벨 갱신 ===
            QVector<QLabel*> labels = w->property("valueLabels").value<QVector<QLabel*>>();
            if (labels.isEmpty() || vals.isEmpty()) continue;

            int n = std::min(labels.size(), vals.size());
            for (int j = 0; j < n; ++j) {
                if(itemKey == "Volt. Unbal. %" || itemKey == "Curr. Unbal. %" || itemKey == "THD %") labels[j]->setText(QString::number(vals[j], 'f', 1));
                else labels[j]->setText(QString::number(vals[j], 'f', 3));
            }
        }
    }
}






void A3700N_GUI::loadQSS(const QString& path)
{
    QFile file(path);
    if (file.open(QFile::ReadOnly)) {
        QString style = QString::fromUtf8(file.readAll());
        this->setStyleSheet(style); // 현재 파일에만 적용한다.
        file.close();
    } else {
        qWarning() << "QSS 파일을 찾을 수 없습니다:" << path;
    }
}


void A3700N_GUI::setMainWindow(MainWindow* mainWin){
    mainWindow = mainWin;
}

void A3700N_GUI::setInputWindow(inputMainWindow* inputWin){
    inputWindow = inputWin;
}

void A3700N_GUI::setOneSecWindow(oneSecMainWindow* oneSecWin){
    oneSecWindow = oneSecWin;
}
