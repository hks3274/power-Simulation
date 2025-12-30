#ifndef A37__N_GUI_H
#define A37__N_GUI_H

#include <QMainWindow>
#include <QObject>

#include <QTabWidget>
#include <QStackedWidget>
#include "dataManagement.h"

class A3700N_GUI : public QMainWindow {
    Q_OBJECT
public:
    A3700N_GUI(dataManagement& dataMng);

public slots:
    void updateOneSec(dataManagement::oneSecList* osl);

private:
    dataManagement& dataMng;

    QStackedWidget* mainStack;


    void loadQSS(const QString& path);

    QWidget* createPage(const QStringList& listName);

};

#endif // A37__N_GUI_H
