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
#include "settingdialog.h"

inputMainWindow::inputMainWindow(dataManagement& dataMng,
                                 MainWindow* mainWin,
                                 oneSecMainWindow* oneSecWin,
                                 A3700N_GUI *a3700Win,
                                 QWidget* parent)
    : QMainWindow(parent)
    , settings("mydb.sqlite")
    , dataMng(dataMng)
    , mainWindow(mainWin)
    , oneSecWindow(oneSecWin)
    , a3700Window(a3700Win)
{
    // ----- 메뉴바 (간소화: 보기, 창열기만 유지) -----
    QMenuBar* menubar = menuBar();
    QMenu* viewMenu = menubar->addMenu("보기");
    QMenu* windowMenu = menubar->addMenu("창열기");

    QAction* openMainWin   = new QAction("그래프창 열기", this);
    QAction* openOneSecWin = new QAction("1초 데이터창 열기", this);
    QAction* openA3700Win = new QAction("A3700창 열기", this);
    windowMenu->addAction(openMainWin);
    windowMenu->addAction(openOneSecWin);
    windowMenu->addAction(openA3700Win);

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

    connect(openA3700Win, &QAction::triggered, [this]() {
        if (a3700Window) {
            a3700Window->show();
            a3700Window->raise();
            a3700Window->activateWindow();
        }
    });


    // ----- 메뉴바 아래 QToolBar (가운데 정렬 + 4개 버튼 실제 기능 연결) -----
    QToolBar* topToolBar = new QToolBar("상단 버튼바", this);
    topToolBar->setMovable(false);
    topToolBar->setIconSize(QSize(25, 25));
    topToolBar->setStyleSheet(
        "QToolBar { spacing: 8px; padding: 5px; background-color: #f5f3f3; }"
        "QToolButton { padding: 3px 10px; border: 1px solid #ccc; border-radius: 5px; font-size: 9pt;background-color:#fafafa; }"
        "QToolButton:hover { background-color:#6b9dcf; color: white;}"
        "QToolButton:checked { background-color: #0063AF; color: white; border: 1px solid #0063AF; }"
        "QToolButton:pressed { background-color: #ccc; }"
        );

    QWidget* leftSpacer = new QWidget();
    leftSpacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    QWidget* rightSpacer = new QWidget();
    rightSpacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    QWidget* centerWidget = new QWidget();
    QHBoxLayout* layout = new QHBoxLayout(centerWidget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(12);

    // QToolButton* btnSave = new QToolButton();
    // QToolButton* btnLoad = new QToolButton();
    QToolButton* btnSetting = new QToolButton();
    QToolButton* btnTrack = new QToolButton();
    QToolButton* btnRun = new QToolButton();

    // btnSave->setText("저장");
    // btnLoad->setText("불러오기");
    btnSetting->setText("설정");
    btnTrack->setText("주파수 추적");
    btnRun->setText("일시정지");

    // btnSave->setFixedSize(70, 28);
    // btnLoad->setFixedSize(70, 28);
    btnSetting->setFixedSize(50, 28);
    btnTrack->setFixedSize(100, 28);
    btnRun->setFixedSize(70, 28);

    for (auto* b : {/*btnSave, btnLoad,*/btnSetting , btnTrack, btnRun}) {
        layout->addWidget(b);
    }

    topToolBar->addWidget(leftSpacer);
    topToolBar->addWidget(centerWidget);
    topToolBar->addWidget(rightSpacer);
    addToolBar(Qt::TopToolBarArea, topToolBar);

    // ----- 실제 기능 연결 -----
    tracker = new frequencytracker(dataMng);


    connect(btnSetting, &QToolButton::clicked, this, [this, &dataMng]() {
        // 설정 다이얼로그 생성
        settingDialog dlg(settings, dataMng, this);

        // 다이얼로그가 닫힐 때 (불러오기 완료 시 accept())
        connect(&dlg, &QDialog::accepted, this, [this]() {
            // 불러오기 후 각 위젯 새로고침
            if (iWidget)  iWidget->refreshFromData();
            if (iSWidget) iSWidget->refreshFromData();
            if (hWidget)  hWidget->refreshFromData();
            emit loadActionPushed();
            QMessageBox::information(this, "불러오기 완료", "설정이 적용되었습니다.");
        });

        dlg.exec();  // 다이얼로그 실행 (닫힐 때 accepted 발생)
    });


    btnTrack->setCheckable(true);
    btnRun->setCheckable(true);

    // --- 주파수 추적 버튼 (start/stop 토글) ---
    connect(btnTrack, &QToolButton::clicked, this, [this, btnTrack]() {
        bool tracking = btnTrack->isChecked(); // 체크상태로 판별
        if (tracking) {
            tracker->start();
            btnTrack->setText("추적 중지");
            statusBar()->showMessage("자동 주파수 추적 시작", 1200);
        } else {
            tracker->stop();
            btnTrack->setText("주파수 추적");
            statusBar()->showMessage("자동 주파수 추적 중지", 1200);
        }
    });

    // --- 시작 버튼 (일시정지 토글) ---
    connect(btnRun, &QToolButton::clicked, this, [this, btnRun, &dataMng]() {
        bool paused = !dataMng.isPaused();
        dataMng.setPaused(paused);
        btnRun->setChecked(paused); // checked 상태도 반영
        btnRun->setText(paused ? "시작" : "일시정지");
        statusBar()->showMessage(paused ? "일시정지됨" : "다시시작됨", 1200);
    });

    // ----- 타이틀/기본 세팅 -----
    setWindowTitle("입력/고조파 설정");
    resize(600, 1000);
    settings.loadByName(settings.getLastLoadedName(), dataMng);
    setDockNestingEnabled(true);

    // ----- 실제 위젯 생성 -----
    iWidget = new inputWidget(dataMng); // 입력창 (3상 입력)
    hWidget = new harmonicwidget(dataMng);  // 고조파창
    iSWidget = new inputSettingWidget(dataMng); //입력 설정 창 (주파수, cps, spc 입력)

    // ----- Dock 생성 -----
    leftDock = new QDockWidget("3상 전압 전류 입력창", this);
    leftDock->setWidget(iWidget);
    rightTopDock = new QDockWidget("설정 입력창", this);
    rightTopDock->setWidget(iSWidget);
    rightBotDock = new QDockWidget("고조파 설정창", this);
    rightBotDock->setWidget(hWidget);

    QWidget* centralDummy = new QWidget(this);
    centralDummy->setFixedSize(0, 0);
    setCentralWidget(centralDummy);
    addDockWidget(Qt::LeftDockWidgetArea, leftDock);
    splitDockWidget(leftDock, rightTopDock, Qt::Horizontal);
    splitDockWidget(rightTopDock, rightBotDock, Qt::Vertical);

    auto applyRatios = [this]() {
        resizeDocks({leftDock, rightTopDock}, {5, 4}, Qt::Horizontal);
        resizeDocks({rightTopDock, rightBotDock}, {1, 1}, Qt::Vertical);
    };
    applyRatios();
    QTimer::singleShot(0, this, applyRatios);

    // 보기 메뉴 토글
    viewMenu->addAction(leftDock->toggleViewAction());
    viewMenu->addAction(rightTopDock->toggleViewAction());
    viewMenu->addAction(rightBotDock->toggleViewAction());
    viewMenu->addAction(topToolBar->toggleViewAction());

    // 화면 중앙 정렬
    const QRect screenRect = QApplication::primaryScreen()->availableGeometry();
    move(screenRect.center() - rect().center());

    connect(tracker, &frequencytracker::cpsChanged,
            iSWidget, &inputSettingWidget::setCpsFromTracker);

}


inputMainWindow::~inputMainWindow() {}
