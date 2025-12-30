#ifndef A37__N_GUI_H
#define A37__N_GUI_H

#include <QMainWindow>
#include <QObject>

#include <QTabWidget>
#include <QStackedWidget>
#include <QCheckBox>
#include <QPushButton>
#include "a3700n_harm.h"
#include "dataManagement.h"

class inputMainWindow;
class MainWindow;
class oneSecMainWindow;

class A3700N_GUI : public QMainWindow {
    Q_OBJECT
public:
    A3700N_GUI(dataManagement& dataMng);

    void setMainWindow(MainWindow* mainWin);
    void setInputWindow(inputMainWindow* inputWin);
    void setOneSecWindow(oneSecMainWindow* oneSecWin);

public slots:
    void updateOneSec(dataManagement::oneSecList* osl);

private:
    dataManagement& dataMng;

    QStackedWidget* mainStack;

    void loadQSS(const QString& path);

    QWidget* createPage(const QStringList& listName);

    inputMainWindow* inputWindow = nullptr;
    MainWindow* mainWindow = nullptr;
    oneSecMainWindow* oneSecWindow = nullptr;

    //phasor checkbox
    QCheckBox* phasorVolCheck; // false -> phasor의 voltage 값 X.
    QCheckBox* phasorCurCheck; // false -> phasor의 current 값 X.

    //waveform CheckBox
    QCheckBox* waveVolCheck; // false -> wave의 voltage 값 X.
    QCheckBox* waveCurCheck; // false -> wave의 current 값 X.


    QPushButton* harmVoltBtn;
    QPushButton* harmCurrBtn;

    a3700n_harm* harmWidget;

};

#endif // A37__N_GUI_H
