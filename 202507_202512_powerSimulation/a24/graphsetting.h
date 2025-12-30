#ifndef GRAPHSETTING_H
#define GRAPHSETTING_H
#include "dataManagement.h"
#include <QDialog>
#include <QDoubleSpinBox>
#include <QButtonGroup>
#include <qradiobutton.h>

class graphSetting: public QDialog
{
    Q_OBJECT
public:
    explicit graphSetting(dataManagement& dataMng, QWidget* parent = nullptr);

    void refreshFromData();

private slots:
    void graphSettingChange();

private:
    dataManagement& dataMng;

    QDoubleSpinBox *widthSpinBox;

    QButtonGroup* updateModeGroup;

    QRadioButton* oneSampleBtn;
    QRadioButton* halfCycleBtn;
    QRadioButton* fullCycleBtn;


};

#endif // GRAPHSETTING_H
