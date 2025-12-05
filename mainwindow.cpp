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
#include "inputmainwindow.h"
#include "onesecmainwindow.h"
#include "rmsgraphwidget.h"
#include "phasorwidget.h"


MainWindow::MainWindow(dataManagement& dataMng,QWidget* parent)
    : QMainWindow(parent)
    , dataMng(dataMng)
{
    // 메뉴바
    QMenuBar* menubar = menuBar();
    QMenu* viewMenu = menubar->addMenu("보기");

    QMenu* windowMenu = menubar->addMenu("창열기");
    // 액션 생성
    QAction* openInputWin    = new QAction("입력창 열기", this);
    QAction* openOneSecWin  = new QAction("1초 데이터창 열기", this);
    QAction* openA3700Win  = new QAction("A3700창 열기", this);

    // 메뉴에 추가
    windowMenu->addAction(openInputWin);
    windowMenu->addAction(openOneSecWin);
    windowMenu->addAction(openA3700Win);

    connect(openInputWin, &QAction::triggered, [this]() {
        if (inputWindow) {
            inputWindow->show();
            inputWindow->raise();
            inputWindow->activateWindow();
        }
    });

    connect(openOneSecWin, &QAction::triggered, [this]() {
        if (oneSecWindow) {
            oneSecWindow->show();
            oneSecWindow->raise();
            oneSecWindow->activateWindow();
        }
    });

    connect(openA3700Win, &QAction::triggered, [this]() {
        if (a3700Window) {
            a3700Window->show();
            a3700Window->raise();
            a3700Window->activateWindow();
        }
    });


    // 중앙 더미 위젯 (Dock용 중앙 영역)
    /*centralDummy = new QWidget(this);
    centralDummy->setFixedSize(1, 1);
    setCentralWidget(centralDummy);*/

    setDockNestingEnabled(true);
    setWindowTitle("QT기반 전력계측 시뮬레이션 SW");
    resize(1200, 1000);

    // --- 실제 위젯 생성 ---
    gWidget    = new graphWidget(dataMng);
    rmsGWidget = new rmsGraphWidget(dataMng);
    pWidget    = new phasorWidget(dataMng);
    showWidget = new show_hide_check_widget(dataMng);


    // --- Dock 생성 ---
    midTopDock = new QDockWidget("3상 그래프창", this);
    midTopDock->setWidget(gWidget);
    midTopDock->setAllowedAreas(Qt::AllDockWidgetAreas);

    midBotDock = new QDockWidget("RMS 그래프창", this);
    midBotDock->setWidget(rmsGWidget);
    midBotDock->setAllowedAreas(Qt::AllDockWidgetAreas);

    rightTopDock = new QDockWidget("phasor", this);
    rightTopDock->setWidget(pWidget);
    rightTopDock->setAllowedAreas(Qt::AllDockWidgetAreas);

    rightBotDock = new QDockWidget("그래프 표시 관리", this);
    rightBotDock->setWidget(showWidget);
    rightBotDock->setAllowedAreas(Qt::AllDockWidgetAreas);


    // 크기 정책 (가운데 위젯들이 넓게)
    midTopDock->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    midBotDock->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    gWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    rmsGWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // --- 레이아웃 배치 ---
    // 1) 상단 두 개 나란히
    addDockWidget(Qt::TopDockWidgetArea, midTopDock);
    splitDockWidget(midTopDock, rightTopDock, Qt::Horizontal);

    // 2) 하단 두 개 나란히
    addDockWidget(Qt::BottomDockWidgetArea, midBotDock);
    splitDockWidget(midBotDock, rightBotDock, Qt::Horizontal);


    showWidget->connectTargets(gWidget, rmsGWidget, pWidget);


    // --- 크기 비율 적용 ---
    auto applyRatios = [this]{
        // 위아래 1:1
        resizeDocks({midTopDock, midBotDock}, {1, 1}, Qt::Vertical);
        // 좌우 1:1
        resizeDocks({midTopDock, rightTopDock}, {3, 1}, Qt::Horizontal);
        resizeDocks({midBotDock, rightBotDock}, {3, 1}, Qt::Horizontal);
    };
    applyRatios();
    QTimer::singleShot(0, this, applyRatios);

    // 보기 메뉴 토글
    viewMenu->addAction(midTopDock->toggleViewAction());
    viewMenu->addAction(rightTopDock->toggleViewAction());
    viewMenu->addAction(midBotDock->toggleViewAction());
    viewMenu->addAction(rightBotDock->toggleViewAction());

    // 시작 위치 화면 중앙
    const QRect screenRect = QApplication::primaryScreen()->availableGeometry();
    const int x = screenRect.left() + (screenRect.width() - width()) / 2;
    const int y = screenRect.top()  + (screenRect.height() - height()) / 2;
    move(x, y);
}

// void MainWindow::closeEvent(QCloseEvent* event)
// {
//     m_data.setTimeScale(1);  // MainWindow 닫을 때만
//     QMainWindow::closeEvent(event);
// }

// inputMainWindow 신호 등에서 호출할 “불러온 값 반영” 핸들러
void MainWindow::doLoadAction()
{
    if (gWidget)    gWidget->refreshFromData();
    if (rmsGWidget) rmsGWidget->refreshFromData();
}

void MainWindow::setInputWindow(inputMainWindow* inputWin) {
    inputWindow = inputWin;
}
void MainWindow::setOneSecWindow(oneSecMainWindow* oneSecWin) {
    oneSecWindow = oneSecWin;
}

void MainWindow::setA3700Window(A3700N_GUI* a3700Win) {
    a3700Window = a3700Win;
}


MainWindow::~MainWindow() {}
