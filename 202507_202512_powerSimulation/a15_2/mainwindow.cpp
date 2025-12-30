// mainwindow.cpp
#include "mainwindow.h"
#include <QApplication>
#include <QScreen>
#include <QMenuBar>
#include <QMenu>
#include <QDockWidget>
#include <QTimer>
#include <QCloseEvent>

// 필요 위젯 헤더
#include "graphwidget.h"
#include "rmsgraphwidget.h"
#include "phasorwidget.h"

MainWindow::MainWindow(dataManagement& dataMng, QWidget* parent)
    : QMainWindow(parent)
    , settings("mydb.sqlite")
    , m_data(dataMng)
{
    // 메뉴바
    QMenuBar* menubar = menuBar();
    QMenu* viewMenu = menubar->addMenu("보기");

    // 중앙 더미 위젯 (Dock용 중앙 영역)
    centralDummy = new QWidget(this);
    centralDummy->setFixedSize(1, 1);
    setCentralWidget(centralDummy);

    setDockNestingEnabled(true);
    setWindowTitle("QT기반 전력계측 시뮬레이션 SW");
    resize(1200, 1000);

    // 설정 로드
    settings.load(m_data);

    // --- 실제 위젯 생성 ---
    gWidget    = new graphWidget(m_data);
    rmsGWidget = new rmsGraphWidget(m_data);
    pWidget    = new phasorWidget(m_data);

    // --- Dock 생성 ---
    midTopDock = new QDockWidget("그래프창", this);
    midTopDock->setWidget(gWidget);
    midTopDock->setAllowedAreas(Qt::AllDockWidgetAreas);

    midBotDock = new QDockWidget("RMS 그래프창", this);
    midBotDock->setWidget(rmsGWidget);
    midBotDock->setAllowedAreas(Qt::AllDockWidgetAreas);

    rightBotDock = new QDockWidget("Phasor 값", this);
    rightBotDock->setWidget(pWidget);
    rightBotDock->setAllowedAreas(Qt::AllDockWidgetAreas);

    // 크기 정책 (가운데 위젯들이 넓게)
    midTopDock->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    midBotDock->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    gWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    rmsGWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // --- 레이아웃 배치 ---
    // 1) 상단: 그래프창 단독
    addDockWidget(Qt::LeftDockWidgetArea, midTopDock);
    // 2) 상단 아래: RMS(세로 분할)
    splitDockWidget(midTopDock, midBotDock, Qt::Vertical);
    // 3) 하단 영역을 좌우로 분할: [RMS | Phasor]
    splitDockWidget(midBotDock, rightBotDock, Qt::Horizontal);

    // --- 크기 비율 적용 ---
    auto applyRatios = [this]{
        // 세로: [상단 그래프 : 하단 영역] = 2 : 1
        resizeDocks({midTopDock, midBotDock}, {1, 1}, Qt::Vertical);
        // 가로(하단): [RMS : Phasor] = 1 : 1
        resizeDocks({midBotDock, rightBotDock}, {3, 1}, Qt::Horizontal);
    };
    applyRatios();
    QTimer::singleShot(0, this, applyRatios); // 초기 타이밍 보정

    // 보기 메뉴 토글
    viewMenu->addAction(midTopDock->toggleViewAction());
    viewMenu->addAction(midBotDock->toggleViewAction());
    viewMenu->addAction(rightBotDock->toggleViewAction());

    // 시작 위치 화면 중앙
    const QRect screenRect = QApplication::primaryScreen()->availableGeometry();
    const int x = screenRect.left() + (screenRect.width() - width()) / 2;
    const int y = screenRect.top()  + (screenRect.height() - height()) / 2;
    move(x, y);
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    m_data.setTimeScale(1);  // MainWindow 닫을 때만
    QMainWindow::closeEvent(event);
}

// inputMainWindow 신호 등에서 호출할 “불러온 값 반영” 핸들러
void MainWindow::doLoadAction()
{
    if (gWidget)    gWidget->refreshFromData();
    if (rmsGWidget) rmsGWidget->refreshFromData();
}

MainWindow::~MainWindow() {}
