#ifndef GRAPHSETTING_H
#define GRAPHSETTING_H
#include "datamanagement.h"
#include <QDialog>
#include <QDoubleSpinBox>

class graphSetting: public QDialog
{
    Q_OBJECT
public:
    explicit graphSetting(dataManagement& dataMng, QWidget* parent = nullptr);

private slots:
    void graphSettingChange();
private:
    dataManagement& dataMng;

    QDoubleSpinBox *widthSpinBox;
};

#endif // GRAPHSETTING_H
