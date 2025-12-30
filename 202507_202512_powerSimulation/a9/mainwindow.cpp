//mainwindow.cpp
#include "mainwindow.h"
#include <QApplication>
#include <QScreen>
#include <QMessageBox>

MainWindow::MainWindow(dataManagement& dataMng, QWidget* parent)
    : QMainWindow(parent)
    , settings("mydb.sqlite")
    , m_data(dataMng)
{
    // 메뉴바 및 우측 코너 토글 버튼
    QMenuBar* menubar = menuBar();
    QMenu* settingMenu = menubar->addMenu("설정");

    QAction* saveAction = new QAction("설정 저장", this);
    QAction* loadAction = new QAction("설정 복구", this);
    settingMenu->addAction(saveAction);
    settingMenu->addAction(loadAction);

    QMenu* viewMenu = menubar->addMenu("보기");

    runButton = new QToolButton(menubar);
    runButton->setCheckable(true);
    runButton->setAutoRaise(true);
    runButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    runButton->setText("일시정지");
    runButton->setShortcut(QKeySequence(Qt::Key_Space));
    runButton->setStyleSheet(
        "QToolButton { padding:4px 10px; border:1px solid #cfcfcf; border-radius:6px; }"
        "QToolButton:hover { background-color:#e9e9e9; }"
        "QToolButton:checked { background-color:#444; color:#fff; border-color:#444; }"
        "QToolButton:pressed { background-color:#d0d0d0; }"
        );
    menubar->setCornerWidget(runButton, Qt::TopRightCorner);

    // 중앙 더미 위젯 지정 (Dock 레이아웃의 중앙 영역)
    centralDummy = new QWidget(this);
    centralDummy->setFixedSize(1, 1);
    setCentralWidget(centralDummy);

    setDockNestingEnabled(true);


    this->setWindowTitle("QT기반 전력계측 시뮬레이션 SW");
    resize(1280, 720);

    settings.load(m_data);

    // 실제 위젯 생성
    iWidget    = new inputWidget(m_data);
    gWidget    = new graphWidget(m_data);
    rmsGWidget = new rmsGraphWidget(m_data);
    pWidget    = new phasorWidget(m_data);


    // Dock 생성 및 위젯 장착
    leftDock   = new QDockWidget("입력창", this);
    leftDock->setWidget(iWidget);
    leftDock->setAllowedAreas(Qt::AllDockWidgetAreas);

    midTopDock = new QDockWidget("그래프창", this);
    midTopDock->setWidget(gWidget);
    midTopDock->setAllowedAreas(Qt::AllDockWidgetAreas);

    midBotDock = new QDockWidget("RMS 그래프창", this);
    midBotDock->setWidget(rmsGWidget);
    midBotDock->setAllowedAreas(Qt::AllDockWidgetAreas);

    rightTopDock  = new QDockWidget("Phasor 값", this);
    rightTopDock->setWidget(pWidget);
    rightTopDock->setAllowedAreas(Qt::AllDockWidgetAreas);

    rightBotDock = new QDockWidget("빈 공간", this);
    rightBotDock->setWidget(new QWidget(rightBotDock));

    // 1px 앵커 도크 생성(왼쪽 전용, 닫기/이동 불가, 타이틀바 숨김)
    auto leftZero = new QDockWidget(this);
    leftZero->setObjectName("LeftZeroAnchor");
    leftZero->setAllowedAreas(Qt::LeftDockWidgetArea);
    leftZero->setFeatures(QDockWidget::NoDockWidgetFeatures);
    leftZero->setTitleBarWidget(new QWidget(leftZero));

    auto spacer = new QWidget(leftZero);
    spacer->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    spacer->setMinimumWidth(10);   // ← 0 말고 1px
    spacer->setMaximumWidth(10);
    leftZero->setWidget(spacer);

    midTopDock->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    midBotDock->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    gWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    rmsGWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // 사이즈 정책/상한 — 가운데가 커지도록
    midTopDock->setMinimumWidth(800);
    midBotDock->setMinimumWidth(800);

    // 앵커를 먼저 왼쪽에 붙이고, 그 오른쪽에 입력창 분할
    addDockWidget(Qt::LeftDockWidgetArea, leftZero);
    splitDockWidget(leftZero, leftDock, Qt::Horizontal);

    // 가운데 열을 좌의 오른쪽으로 수평 분할 → [left | midTop]
    splitDockWidget(leftDock, midTopDock, Qt::Horizontal);

    // 우 열을 ‘가운데’의 오른쪽으로 수평 분할 → [left | midTop | right] 같은 체인 완성
    splitDockWidget(midTopDock, rightTopDock, Qt::Horizontal);

    // 가운데, 오른쪽 열 상/하 분할
    splitDockWidget(midTopDock, midBotDock, Qt::Vertical);
    splitDockWidget(rightTopDock, rightBotDock, Qt::Vertical);

    // 5) 비율 적용
    auto applyRatios = [this]{
        // 가로 1:2:1
        resizeDocks({leftDock, midTopDock, rightTopDock}, {1, 2, 1}, Qt::Horizontal);
        // 가운데 세로 1:2
        resizeDocks({midTopDock, midBotDock}, {1, 2}, Qt::Vertical);
        resizeDocks({rightTopDock, rightBotDock}, {1, 2}, Qt::Vertical);
    };

    // 즉시 1회 + 한 틱 뒤에 한 번 더 (초기 레이아웃 타이밍 보정)
    applyRatios();
    QTimer::singleShot(0, this, applyRatios);

    viewMenu->addAction(leftDock->toggleViewAction());
    viewMenu->addAction(midTopDock->toggleViewAction());
    viewMenu->addAction(midBotDock->toggleViewAction());
    viewMenu->addAction(rightTopDock->toggleViewAction());
    viewMenu->addAction(rightBotDock->toggleViewAction());

    // 버튼/데이터 동작 연결
    connect(runButton, &QToolButton::toggled, this, [this](bool checked){
        m_data.setPaused(checked);
        syncPauseButton(checked);
        statusBar()->showMessage(checked ? "일시정지됨" : "다시시작됨", 1200);
    });
    connect(&m_data, &dataManagement::pausedChanged, this, [this](bool p){
        if (runButton->isChecked() != p) runButton->setChecked(p);
        syncPauseButton(p);
    });

    connect(saveAction, &QAction::triggered, this, [this]() {
        settings.save(m_data);
        QMessageBox::information(this, "저장 완료", "설정이 성공적으로 저장되었습니다.");
    });
    connect(loadAction, &QAction::triggered, this, [this]() {
        settings.load(m_data);
        if (iWidget)    iWidget->refreshFromData();
        if (gWidget)    gWidget->refreshFromData();
        if (rmsGWidget) rmsGWidget->refreshFromData();
    });

    // 시작 위치 화면 중앙
    QRect screenRect = QApplication::primaryScreen()->availableGeometry();
    int x = screenRect.left() + (screenRect.width() - width()) / 2;
    int y = screenRect.top() + (screenRect.height() - height()) / 2;
    move(x, y);
}

void MainWindow::syncPauseButton(bool paused) {
    runButton->setText(paused ? "다시시작" : "일시정지");
}

MainWindow::~MainWindow() {}
