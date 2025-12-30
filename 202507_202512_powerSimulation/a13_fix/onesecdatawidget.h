#ifndef ONESECDATAWIDGET_H
#define ONESECDATAWIDGET_H

#include "dataManagement.h"
#include <QWidget>

class oneSecDataWidget : public QWidget
{
    Q_OBJECT
public:
    oneSecDataWidget(dataManagement& dataMng);

private:
    dataManagement& dataMng;
};

#endif // ONESECDATAWIDGET_H
