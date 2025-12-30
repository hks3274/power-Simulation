#include "mainwindow.h"
#include <QBoxLayout>
#include <QMdiSubWindow>
#include <QApplication>

MainWindow::MainWindow(dataManagement& dataMng, QWidget* parent)
    :QMainWindow(parent)
    , settings("mydb.sqlite")
    , m_data(dataMng)
{

    // 메뉴바 추가
    QMenuBar* menubar = menuBar();
    QMenu* settingMenu = menubar->addMenu("설정");

    QAction* saveAction = new QAction("설정 저장", this);
    QAction* loadAction = new QAction("설정 복구", this);

    settingMenu->addAction(saveAction);
    settingMenu->addAction(loadAction);



    // 3) 오른쪽 끝에 ‘시작/일시정지’ 토글 버튼 (코너 위젯 방식)
    runButton = new QToolButton(menubar);
    runButton->setCheckable(true);
    runButton->setAutoRaise(true);
    runButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    runButton->setText("일시정지");                 // 체크되면 ▶ 재개
    runButton->setShortcut(QKeySequence(Qt::Key_Space));

    runButton->setStyleSheet(
        "QToolButton { padding:4px 10px; border:1px solid #cfcfcf; border-radius:6px; }"
        "QToolButton:hover { background-color:#e9e9e9; }"
        "QToolButton:checked { background-color:#444; color:#fff; border-color:#444; }"
        "QToolButton:pressed { background-color:#d0d0d0; }"
        );

    // 핵심 한 줄: 메뉴바 오른쪽 위 코너에 꽂기
    menubar->setCornerWidget(runButton, Qt::TopRightCorner);


    this->setWindowTitle("QT기반 전력계측 시뮬레이션 SW");
    mdiarea = new QMdiArea(this); //객체 생성
    setCentralWidget(mdiarea);
    mdiarea->setMinimumSize(1024, 768);

    settings.load(dataMng);

    // inputwidget 생성
    iWidget = new inputWidget(m_data);
    iSubWindow = mdiarea->addSubWindow(iWidget);
    iSubWindow->setWindowTitle("입력창");
    iSubWindow->setWindowFlags(Qt::SubWindow | Qt::WindowTitleHint | Qt::WindowMinMaxButtonsHint);
    iSubWindow->show();

    // graphWidget 생성
    gWidget = new graphWidget(m_data);
    gSubWindow = mdiarea->addSubWindow(gWidget);
    gSubWindow->setWindowTitle("그래프창");
    gSubWindow->setWindowFlags(Qt::SubWindow | Qt::WindowTitleHint | Qt::WindowMinMaxButtonsHint);
    gSubWindow->show();

    //전압RMS, 전류RMS, 전력 값 그래프 서브윈도우창 생성
    rmsGWidget = new rmsGraphWidget(m_data);
    rmsGSubWindow = mdiarea->addSubWindow(rmsGWidget);
    rmsGSubWindow->setWindowTitle("RMS 그래프창");
    rmsGSubWindow->setWindowFlags(Qt::SubWindow | Qt::WindowTitleHint | Qt::WindowMinMaxButtonsHint);
    rmsGSubWindow->show();


    // 4) 동작 연결
    connect(runButton, &QToolButton::toggled, this, [this](bool checked){
        // checked == true  → 사용자가 버튼을 눌러 ‘일시정지’ 상태로 전환
        m_data.setPaused(checked);
        runButton->setText(checked ? "다시시작" : "일시정지");
        statusBar()->showMessage(checked ? "일시정지됨" : "다시시작됨", 1200);
    });

    // 5) dataManagement에서 상태가 바뀌어도 버튼과 동기화
    connect(&m_data, &dataManagement::pausedChanged, this, [this](bool p){
        if (runButton->isChecked() != p) runButton->setChecked(p);
        runButton->setText(p ? "다시시작" : "일시정지");
    });


    // 저장하기 버튼 시그널
    connect(saveAction, &QAction::triggered, this, [this]() {
        settings.save(m_data);
        QMessageBox::information(this, "저장 완료", "설정이 성공적으로 저장되었습니다.");
    });


    //불러오기 버튼 시그널
    connect(loadAction, &QAction::triggered, this, [this]() {
        settings.load(m_data);

        if (iWidget)
            iWidget->refreshFromData();

        if (gWidget)
            gWidget->refreshFromData();

        if (rmsGWidget)
            rmsGWidget->refreshFromData();

    });

    // 창을 정중앙에서 띄우도록 하기 위해서 추가
    resize(1080, 720);

    QRect screenRect = QApplication::primaryScreen()->availableGeometry();
    int x = screenRect.left() + (screenRect.width() - width()) / 2;
    int y = screenRect.top() + (screenRect.height() - height()) / 2;
    move(x, y);

}

// 창의 크기 변경 시 안의 서브윈도우의 크기도 일정하게 변경됨
void MainWindow::showEvent(QShowEvent *event)
{
    QMainWindow::showEvent(event);

    int mdiHeight = mdiarea->height();
    int mdiWidth = mdiarea->width();

    // 왼쪽 입력창 (1/3)
    int wLeft = mdiWidth / 3;
    int wRight = mdiWidth - wLeft;

    // 위/아래 분할
    int hTop = mdiHeight / 3;         // 위쪽: 1/3
    int hBottom = mdiHeight - hTop;   // 아래쪽: 2/3

    iSubWindow->setGeometry(0, 0, wLeft, mdiHeight);     // 왼쪽 입력창
    gSubWindow->setGeometry(wLeft, 0, wRight, hTop);     // 오른쪽 위 (그래프)
    rmsGSubWindow->setGeometry(wLeft, hTop, wRight, hBottom); // 오른쪽 아래 (RMS 그래프)
}

MainWindow::~MainWindow() {}
