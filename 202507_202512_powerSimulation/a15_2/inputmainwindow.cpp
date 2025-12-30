#include "inputmainwindow.h"

#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QToolButton>
#include <QMessageBox>
#include <QDockWidget>
#include <QScreen>
#include <QApplication>

#include "inputwidget.h"
#include "harmonicwidget.h"
#include "frequencytracker.h"

inputMainWindow::inputMainWindow(dataManagement& dataMng, QWidget* parent)
    : QMainWindow(parent)
    , settings("mydb.sqlite")
    , dataMng(dataMng)
{
    // ----- 메뉴바 -----
    QMenuBar* menubar = menuBar();
    QMenu* settingMenu = menubar->addMenu("설정");

    QAction* saveAction = new QAction("설정 저장", this);
    QAction* loadAction = new QAction("설정 불러오기", this);
    settingMenu->addAction(saveAction);
    settingMenu->addAction(loadAction);

    QMenu* trackMenu = menubar->addMenu("주파수 추적");
    QAction* startTrack = new QAction("자동 추적 시작", this);
    QAction* stopTrack  = new QAction("자동 추적 중지", this);
    trackMenu->addAction(startTrack);
    trackMenu->addAction(stopTrack);

    QMenu* viewMenu = menubar->addMenu("보기");

    // ----- 우측 상단 코너: 일시정지 버튼 -----
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

    // ----- 타이틀/크기 및 설정 로드 -----
    setWindowTitle("입력/고조파 설정");
    resize(600, 1000);
    settings.load(dataMng);
    setDockNestingEnabled(true);

    // ----- 실제 위젯 생성 -----
    iWidget = new inputWidget(dataMng);     // 입력창 (3상 입력)
    hWidget = new harmonicwidget(dataMng);  // 고조파창
    iSWidget = new inputSettingWidget(dataMng); //입력 설정 창 (주파수, cps, spc 입력)

    // ----- Dock 생성 -----
    leftDock = new QDockWidget("3상 전압 전류 입력창", this);
    leftDock->setWidget(iWidget);
    leftDock->setAllowedAreas(Qt::AllDockWidgetAreas);

    // 오른쪽 상단 ‘빈 공간’ 도크
    rightTopDock = new QDockWidget("설정 입력창", this);
    rightTopDock->setWidget(iSWidget);
    rightTopDock->setAllowedAreas(Qt::AllDockWidgetAreas);


    // 오른쪽 하단 ‘고조파 설정’ 도크
    rightBotDock = new QDockWidget("고조파 설정창", this);
    rightBotDock->setWidget(hWidget);
    rightBotDock->setAllowedAreas(Qt::AllDockWidgetAreas);

    // 중앙 더미 위젯
    QWidget* centralDummy = new QWidget(this);
    centralDummy->setFixedSize(1, 1);
    setCentralWidget(centralDummy);

    // ----- 배치 -----
    // 1) 왼쪽 입력 도크 배치
    addDockWidget(Qt::LeftDockWidgetArea, leftDock);

    // 2) 왼쪽 오른쪽으로 '오른쪽 상단(빈공간)' 도크를 수평 분할해 체인 형성
    splitDockWidget(leftDock, rightTopDock, Qt::Horizontal);

    // 3) '오른쪽 상단(빈공간)' 아래로 '오른쪽 하단(고조파)'를 수직 분할
    splitDockWidget(rightTopDock, rightBotDock, Qt::Vertical);

    // ----- 비율 적용 -----
    //  - 가로: [왼쪽 입력 : 오른쪽 영역] = 2 : 1 (원하면 1:1로 바꿔도 됨)
    //  - 세로(오른쪽): [위 빈공간 : 아래 고조파] = 1 : 1
    auto applyRatios = [this]() {
        resizeDocks({leftDock, rightTopDock}, {5, 4}, Qt::Horizontal);
        resizeDocks({rightTopDock, rightBotDock}, {1, 1}, Qt::Vertical);
    };
    applyRatios();
    QTimer::singleShot(0, this, applyRatios);

    // 보기 메뉴 토글 등록
    viewMenu->addAction(leftDock->toggleViewAction());
    viewMenu->addAction(rightTopDock->toggleViewAction());
    viewMenu->addAction(rightBotDock->toggleViewAction());

    // ----- Tracker -----
    tracker = new frequencytracker(dataMng);

    connect(startTrack, &QAction::triggered, this, [this]() {
        tracker->start();
        statusBar()->showMessage("자동 주파수 추적 시작", 1200);
    });
    connect(stopTrack, &QAction::triggered, this, [this]() {
        tracker->stop();
        statusBar()->showMessage("자동 주파수 추적 중지", 1200);
    });

    // ----- 설정 저장/불러오기 -----
    connect(saveAction, &QAction::triggered, this, [&, this]() {
        settings.save(dataMng);
        QMessageBox::information(this, "저장 완료", "설정이 성공적으로 저장되었습니다.");
    });
    connect(loadAction, &QAction::triggered, this, [&, this]() {
        settings.load(dataMng);
        if (iWidget) iWidget->refreshFromData();
        if (iSWidget) iSWidget ->refreshFromData();
        if (hWidget) hWidget->refreshFromData();
        emit loadActionPushed();  // 그래프 메인윈도우로 전달
    });

    // ----- 일시정지 토글 -----
    connect(runButton, &QToolButton::toggled, this, [&, this](bool checked) {
        dataMng.setPaused(checked);
        syncPauseButton(checked);
        statusBar()->showMessage(checked ? "일시정지됨" : "다시시작됨", 1200);
    });
    connect(&dataMng, &dataManagement::pausedChanged, this, [this](bool p) {
        if (runButton->isChecked() != p) runButton->setChecked(p);
        syncPauseButton(p);
    });

    // ----- tracker → inputWidget (cps 업데이트) -----
    connect(tracker, &frequencytracker::cpsChanged,
            iSWidget, &inputSettingWidget::setCpsFromTracker);

    // 화면 중앙 정렬
    const QRect screenRect = QApplication::primaryScreen()->availableGeometry();
    const int x = screenRect.left() + (screenRect.width() - width()) / 2;
    const int y = screenRect.top() + (screenRect.height() - height()) / 2;
    move(x, y);
}

void inputMainWindow::syncPauseButton(bool paused)
{
    runButton->setText(paused ? "다시시작" : "일시정지");
}

inputMainWindow::~inputMainWindow() {}
