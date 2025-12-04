//mainwindow.h
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenuBar>
#include <QToolButton>
#include <QDockWidget>
#include "a3700n_gui.h"
#include "dataManagement.h"
#include "graphwidget.h"
#include "phasorwidget.h"
#include "rmsgraphwidget.h"
#include "settingstorage.h"
#include "show_hide_check_widget.h"

class inputMainWindow;
class oneSecMainWindow;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(dataManagement& dataMng, QWidget* parent = nullptr);
    ~MainWindow();

    void setInputWindow(inputMainWindow* inputWin);
    void setOneSecWindow(oneSecMainWindow* oneSecWin);
    void setA3700Window(A3700N_GUI* a3700Win);

protected:
     void closeEvent(QCloseEvent* event) override;

public slots:
    void doLoadAction();

private:
    // 중앙 위젯은 꼭 필요하므로 더미 QWidget 하나 둠
    QWidget* centralDummy = nullptr;

    // 각 Dock
    QDockWidget* midTopDock = nullptr; // 그래프(파형)
    QDockWidget* midBotDock = nullptr; // RMS 그래프
    QDockWidget* rightTopDock = nullptr;
    QDockWidget* rightBotDock = nullptr; //빈공간(혹시라도 나중에 뭐가 들어오면 들어가도록)
    //QDockWidget* leftZero = nullptr;

    // 각 실제 위젯
    graphWidget*    gWidget    = nullptr;
    rmsGraphWidget* rmsGWidget = nullptr;
    phasorWidget*   pWidget    = nullptr;
    show_hide_check_widget* showWidget = nullptr;

    // 기타
    settingStorage settings;
    dataManagement& m_data;
    QToolButton*   runButton = nullptr;

    inputMainWindow* inputWindow = nullptr;
    oneSecMainWindow* oneSecWindow = nullptr;
    A3700N_GUI*  a3700Window = nullptr;

};

#endif // MAINWINDOW_H
