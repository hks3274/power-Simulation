#ifndef INPUTMAINWINDOW_H
#define INPUTMAINWINDOW_H

#include "dataManagement.h"
#include "frequencytracker.h"
#include "harmonicwidget.h"
#include "inputsettingwidget.h"
#include "inputwidget.h"
#include "mainwindow.h"
#include "onesecmainwindow.h"
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
    inputMainWindow(dataManagement& dataMng,
                    MainWindow* mainWin = nullptr,
                    oneSecMainWindow* oneSecWin = nullptr,
                    A3700N_GUI* a3700Win = nullptr,
                    QWidget* parent = nullptr);

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

    QDockWidget*     leftDock    = nullptr;
    QDockWidget*     rightBotDock   = nullptr;
    QDockWidget*     rightTopDock   = nullptr;

    MainWindow* mainWindow;
    oneSecMainWindow* oneSecWindow;
    A3700N_GUI* a3700Window;
};

#endif // INPUTMAINWINDOW_H
