#ifndef GRAPHSETTING_H
#define GRAPHSETTING_H
#include "datamanagement.h"
#include <QDialog>
#include <QSpinBox>

class graphSetting: public QDialog
{
    Q_OBJECT
public:
    explicit graphSetting(dataManagement& dataMng);

private slots:
    void graphSettingChange();
private:
    dataManagement& dataMng;

    QSpinBox *widthSpinBox;
};

#endif // GRAPHSETTING_H
