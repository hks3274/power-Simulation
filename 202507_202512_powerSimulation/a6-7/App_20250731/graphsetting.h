#ifndef GRAPHSETTING_H
#define GRAPHSETTING_H
#include "datamanagement.h"
#include <QDialog>
#include <QDoubleSpinBox>
#include <QButtonGroup>

class graphSetting: public QDialog
{
    Q_OBJECT
public:
    explicit graphSetting(dataManagement& dataMng, QWidget* parent = nullptr);

private slots:
    void graphSettingChange();
    //automode로 들어가는 것을 만들어라 우선은 회의준비 ㅎㅎ

private:
    dataManagement& dataMng;

    QDoubleSpinBox *widthSpinBox;

    QButtonGroup* updateModeGroup;
};

#endif // GRAPHSETTING_H
