#ifndef PHASORWIDGET_H
#define PHASORWIDGET_H

#include "dataManagement.h"
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QPainter>
#include <QCheckBox>

class phasorWidget : public QWidget
{
    Q_OBJECT
public:
    phasorWidget(dataManagement& dataMng);
    void setPhasorVisible(const QString& key, bool visible);

protected:
    void paintEvent(QPaintEvent*) override;

private slots:
    void updatePhasor(dataManagement::measure_data);

private:
    dataManagement& dataMng;

    // 3상 기본파(복소)
    double vAX{0}, vAY{0}, vBX{0}, vBY{0}, vCX{0}, vCY{0};
    double cAX{0}, cAY{0}, cBX{0}, cBY{0}, cCX{0}, cCY{0};

    // 고조파(A상만)
    double hvX{0.0}, hvY{0.0}, hcX{0.0}, hcY{0.0};

    int pad{12};

    // 개별 표시 토글
    bool showVa{true}, showVb{true}, showVc{true};
    bool showIa{true}, showIb{true}, showIc{true};
    bool showVH{true}, showIH{true};


    void refreshLegend();
};

#endif // PHASORWIDGET_H
