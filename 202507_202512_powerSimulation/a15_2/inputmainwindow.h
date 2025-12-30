#ifndef INPUTMAINWINDOW_H
#define INPUTMAINWINDOW_H

#include "dataManagement.h"
#include "frequencytracker.h"
#include "harmonicwidget.h"
#include "inputsettingwidget.h"
#include "inputwidget.h"
#include "settingstorage.h"
#include <QMainWindow>
#include <QObject>
#include <QToolbutton>
#include <QMenuBar>
#include <QStatusBar>
#include <QMessageBox>
#include <QSplitter>
#include <QScreen>
#include <QApplication>

class inputMainWindow : public QMainWindow {
    Q_OBJECT
public:
    inputMainWindow(dataManagement& dataMng, QWidget* parent = nullptr);
    ~inputMainWindow();

signals:
    void loadActionPushed();

private :
    dataManagement& dataMng;

    //메뉴바
    settingStorage settings;
    QToolButton*   runButton = nullptr;

    frequencytracker* tracker;

    // 각 실제 위젯
    inputWidget*    iWidget    = nullptr;
    harmonicwidget* hWidget    = nullptr;
    inputSettingWidget* iSWidget    = nullptr;

    void syncPauseButton(bool paused);

    QDockWidget*     leftDock    = nullptr;
    QDockWidget*     rightBotDock   = nullptr;
    QDockWidget*     rightTopDock   = nullptr;
};

#endif // INPUTMAINWINDOW_H
