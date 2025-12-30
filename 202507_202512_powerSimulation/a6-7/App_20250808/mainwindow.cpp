#include "mainwindow.h"
#include "inputwidget.h"
#include "graphwidget.h"
#include <QBoxLayout>
#include <QMdiSubWindow>
#include <QApplication>

MainWindow::MainWindow(dataManagement& dataMng, QWidget* parent)
    :QMainWindow(parent)
    , iSubWindow(new QMdiSubWindow())
    , gSubWindow(new QMdiSubWindow())
    , settings("mydb.sqlite")
{

    // 메뉴바 추가
    QMenuBar* menubar = menuBar();
    QMenu* settingMenu = menubar->addMenu("설정");

    QAction* saveAction = new QAction("설정 저장", this);
    QAction* loadAction = new QAction("설정 복구", this);

    settingMenu->addAction(saveAction);
    settingMenu->addAction(loadAction);

    this->setWindowTitle("QT기반 전력계측 시뮬레이션 SW");
    mdiarea = new QMdiArea(this); //객체 생성
    setCentralWidget(mdiarea);
    mdiarea->setMinimumSize(1280, 1024);

    settings.load(dataMng);

    //inputwidget 생성
    inputWidget *iWidget = new inputWidget(dataMng);
    iSubWindow->setWidget(iWidget);
    iSubWindow->setWindowTitle("입력창");
    iSubWindow->setGeometry(0, 0, width() / 3, height());
    mdiarea->addSubWindow(iSubWindow);
    iSubWindow->setWindowFlags(Qt::SubWindow | Qt::WindowTitleHint | Qt::WindowMinMaxButtonsHint); //윈도우창의 닫기 버튼제거
    iSubWindow->show();

    //graphWidget 생성
    graphWidget *gWidget = new graphWidget(dataMng);
    gSubWindow->setWidget(gWidget);
    gSubWindow->setWindowTitle("그래프창");
    gSubWindow->setGeometry(width() / 3, 0, width() * 2 / 3, height());
    mdiarea->addSubWindow(gSubWindow);
    gSubWindow->setWindowFlags(Qt::SubWindow | Qt::WindowTitleHint | Qt::WindowMinMaxButtonsHint); //윈도우 창의 닫기 버튼제거
    gSubWindow->show();


    // 저장하기 버튼 시그널
    connect(saveAction, &QAction::triggered, this, [&, this]() {
        settings.save(dataMng);
        QMessageBox::information(this, "저장 완료", "설정이 성공적으로 저장되었습니다.");
    });


    //불러오기 버튼 시그널
    connect(loadAction, &QAction::triggered, this, [this, iWidget, &dataMng, gWidget]() {
        settings.load(dataMng);
        if (iWidget)
            iWidget->refreshFromData();

        if (gWidget)
            gWidget->refreshFromData();

    });


    // 창을 정중앙에서 띄우도록 하기 위해서 추가
    resize(1280, 1024);

    QRect screenRect = QApplication::primaryScreen()->availableGeometry();
    int x = screenRect.left() + (screenRect.width() - width()) / 2;
    int y = screenRect.top() + (screenRect.height() - height()) / 2;
    move(x, y);

}

//창의 크기 변경 시 안의 서브윈도우의 크기도 일정하게 변경됨
void MainWindow::showEvent(QShowEvent *event)
{
    QMainWindow::showEvent(event);

    int mdiHeight = mdiarea->height();
    int mdiWidth = mdiarea->width();

    iSubWindow->setGeometry(0, 0, mdiWidth / 3, mdiHeight);
    gSubWindow->setGeometry(mdiWidth / 3, 0, mdiWidth * 2 / 3, mdiHeight);
}

MainWindow::~MainWindow() {}
