#ifndef A37__N_GUI_H
#define A37__N_GUI_H

#include <QMainWindow>
#include <QObject>

#include <QTabWidget>
#include <QStackedWidget>
#include <QCheckBox>
#include <QPushButton>
#include "a3700n_harm.h"
#include "a3700n_phasor.h"
#include "a3700n_wave.h"
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
    void updateOneSec(oneSecList* osl);

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


    //LLBtn LNBtn
    QPushButton* rmsLLBtn;          //VRMS의 LL버튼
    QPushButton* rmsLNBtn;          //VRMS의 LN버튼

    QPushButton* fundLLBtn;         //fund 의 LL버튼
    QPushButton* fundLNBtn;         //fund 의 LN버튼

    QPushButton* thdLLBtn;          // THD의 LL버튼
    QPushButton* thdLNBtn;          // THD의 LN버튼

    QPushButton* symmLLBtn;         // symm의 LL버튼
    QPushButton* symmLNBtn;         // symm의 LN버튼

    bool isMinMode = false;
    bool isMaxMode = false;

    QList<QPushButton*> allMinBtns;
    QList<QPushButton*> allMaxBtns;

    void createLineBtn();

    a3700n_harm* harmWidget;
    a3700n_phasor* phWidget;
    a3700n_wave* waveWidget;

    oneSecList* lastOsl = nullptr; //마지막 값 기억

    void syncMinMaxButtons();
};

#endif // A37__N_GUI_H
