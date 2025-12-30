#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QWidget>
#include "dataManagement.h"
#include <QMdiArea>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(dataManagement& dataMng, QWidget* parent = nullptr); //생성자, data_management& data_mng =>참조, data_management 객체를 받음
    ~MainWindow(); //소멸자,main window가 외부 데이터 관리 객체와 직접 연동되게 할 수 있음
protected :
    void resizeEvent(QResizeEvent *event) override;

private:
    QMdiArea *mdiarea;
    QMdiSubWindow *iSubWindow;
    QMdiSubWindow *gSubWindow;
};
#endif // MAINWINDOW_H
