//mainwindow.h
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenuBar>
#include <QToolButton>
#include <QDockWidget>

#include "dataManagement.h"
#include "frequencytracker.h"
#include "inputwidget.h"
#include "graphwidget.h"
#include "phasorwidget.h"
#include "rmsgraphwidget.h"
#include "settingstorage.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(dataManagement& dataMng, QWidget* parent = nullptr);
    ~MainWindow();

protected:

private:
    // 중앙 위젯은 꼭 필요하므로 더미 QWidget 하나 둠
    QWidget* centralDummy = nullptr;

    // 각 Dock
    QDockWidget* leftDock   = nullptr; // 입력
    QDockWidget* midTopDock = nullptr; // 그래프(파형)
    QDockWidget* midBotDock = nullptr; // RMS 그래프
    QDockWidget* rightTopDock  = nullptr; // Phasor
    QDockWidget* rightBotDock = nullptr; //빈공간(혹시라도 나중에 뭐가 들어오면 들어가도록)
    //QDockWidget* leftZero = nullptr;

    // 각 실제 위젯
    inputWidget*    iWidget    = nullptr;
    graphWidget*    gWidget    = nullptr;
    rmsGraphWidget* rmsGWidget = nullptr;
    phasorWidget*   pWidget    = nullptr;

    // 기타
    settingStorage settings;
    dataManagement& m_data;
    QToolButton*   runButton = nullptr;

    // 초기 배치(비율 포함)
    void setupDocksAndLayout();
    void syncPauseButton(bool paused);
    frequencytracker* tracker;

};

#endif // MAINWINDOW_H
