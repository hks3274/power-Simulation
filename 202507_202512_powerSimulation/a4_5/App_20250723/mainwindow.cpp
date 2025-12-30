#include "mainwindow.h"
#include "inputwidget.h"
#include "graphwidget.h"
#include <QBoxLayout>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QApplication>
#include <QDebug>

MainWindow::MainWindow(dataManagement& dataMng, QWidget* parent)
    :QMainWindow(parent)
    , iSubWindow(new QMdiSubWindow())
    , gSubWindow(new QMdiSubWindow())
{
    this->setWindowTitle("QT기반 전력계측 시뮬레이션 SW");
    mdiarea = new QMdiArea(this); //객체 생성
    setCentralWidget(mdiarea);
    mdiarea->setMinimumSize(1080, 720);

    //inputwidget 생성
    inputWidget *iWidget = new inputWidget(dataMng);
    iSubWindow->setWidget(iWidget);
    iSubWindow->setWindowTitle("입력창");
    iSubWindow->setGeometry(0, 0, width() / 3, height());
    mdiarea->addSubWindow(iSubWindow);
    iSubWindow->show();
    iSubWindow->setWindowFlags(Qt::SubWindow | Qt::WindowTitleHint | Qt::WindowMinMaxButtonsHint);
    iSubWindow->show();

    //graphWidget 생성
    graphWidget *gWidget = new graphWidget(dataMng);
    gSubWindow->setWidget(gWidget);
    gSubWindow->setWindowTitle("그래프창");
    gSubWindow->setGeometry(width() / 3, 0, width() * 2 / 3, height());
    mdiarea->addSubWindow(gSubWindow);
    gSubWindow->show();
    gSubWindow->setWindowFlags(Qt::SubWindow | Qt::WindowTitleHint | Qt::WindowMinMaxButtonsHint);
    gSubWindow->show();


    // 창을 정중앙에서 띄우도록 하기 위해서 추가
    resize(1080, 720);

    QRect screenRect = QApplication::primaryScreen()->availableGeometry();
    int x = screenRect.left() + (screenRect.width() - width()) / 2;
    int y = screenRect.top() + (screenRect.height() - height()) / 2;
    move(x, y);
    // qDebug() << "hello";
}

//창의 크기 변경 시 안의 서브윈도우의 크기도 일정하게 변경됨
void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);

    iSubWindow->setGeometry(0, 0, width() / 3, height()); // width(), height()는 각 Qwidget의 크기를 의미함
    gSubWindow->setGeometry(width() / 3, 0, width() * 2 / 3, height());
}

MainWindow::~MainWindow() {}
