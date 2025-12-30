#ifndef INPUT_WIDGET_H
#define INPUT_WIDGET_H

#include "datamanagement.h"
#include <QWidget>

class input_widget: public QWidget
{
    Q_OBJECT
public:
    explicit input_widget(datamanagement& dataMng);

private:
    datamanagement& dataMng;
};

#endif // INPUT_WIDGET_H
