#include "mainwindow.h"
#include "inputwidget.h"
#include "graphwidget.h"
#include <QBoxLayout>
#include <QMdiArea>
#include <QMdiSubWindow>

MainWindow::MainWindow(dataManagement& dataMng)
{
    this->resize(800, 600);
    QMdiArea *mdiarea = new QMdiArea(this); //객체 생성

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(mdiarea);
    setLayout(layout);

    QMdiSubWindow *iSubWindow = new QMdiSubWindow();
    inputWidget *iWidget = new inputWidget(dataMng);
    iSubWindow->setWidget(iWidget);
    mdiarea->addSubWindow(iSubWindow);
    iSubWindow->show();

    QMdiSubWindow *gSubWindow = new QMdiSubWindow();
    graphWidget *gWidget = new graphWidget(dataMng);
    gSubWindow->setWidget(gWidget);
    mdiarea->addSubWindow(gSubWindow);
    gSubWindow->show();
}

MainWindow::~MainWindow() {}
