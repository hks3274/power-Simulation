#ifndef SHOW_HIDE_CHECK_WIDGET_H
#define SHOW_HIDE_CHECK_WIDGET_H

#include "dataManagement.h"
#include "graphwidget.h"
#include "phasorwidget.h"
#include "rmsgraphwidget.h"
#include <QWidget>
#include <QCheckBox>
#include <QGroupBox>
#include <QBoxLayout>
#include <QToolBox>

class show_hide_check_widget:  public QWidget
{
    Q_OBJECT
public:
    explicit show_hide_check_widget(dataManagement& dataMng,
                                    QWidget* parent = nullptr);
    void connectTargets(graphWidget* g, rmsGraphWidget* r, phasorWidget* p);


private:
    dataManagement& dataMng;

    QToolBox* accordion = nullptr;

    // 그래프 위젯 포인터
    graphWidget* gWidget = nullptr;
    rmsGraphWidget* rWidget = nullptr;
    phasorWidget* pWidget = nullptr;

    // 체크박스 테이블 저장용
    QMap<QString, QCheckBox*> chkMap;

    QWidget* createThreePhasePage();
    QWidget* createRmsPage();
    QWidget* createPhasorPage();
};

#endif // SHOW_HIDE_CHECK_WIDGET_H
