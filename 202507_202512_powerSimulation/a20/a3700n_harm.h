#ifndef A37__N_HARM_H
#define A37__N_HARM_H

#include <QWidget>
#include <QPushButton>
#include "dataManagement.h"

class a3700n_harm : public QWidget
{
    Q_OBJECT
public:
    a3700n_harm(dataManagement& dataMng);

private:
    dataManagement& dataMng;

    void loadQSS(const QString& listName);

    QPushButton* autoBtn = nullptr;
    QPushButton* plusBtn = nullptr;
    QPushButton* minusBtn = nullptr;

};

#endif // A37__N_HARM_H
