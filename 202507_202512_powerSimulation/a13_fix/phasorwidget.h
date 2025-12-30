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

protected:
    void paintEvent(QPaintEvent*) override;
    void resizeEvent(QResizeEvent*) override;

private slots:
    void updatePhasor(dataManagement::measure_data);

    void toggleV(bool on) { showV = on; update(); }    // ← 전압 화살표 토글
    void toggleI(bool on) { showI = on; update(); }    // ← 전류 화살표 토글

private:
    dataManagement& dataMng;

    double vX{0.0}, vY{0.0}, cX{0.0}, cY{0.0};
    double hvX{0.0}, hvY{0.0}, hcX{0.0}, hcY{0.0};
    int pad{12};

    bool showV{true};
    bool showI{true};
    bool showVH{true};
    bool showIH{true};
    QCheckBox* cbV{nullptr};
    QCheckBox* cbI{nullptr};
    QCheckBox* cbVH{nullptr};
    QCheckBox* cbIH{nullptr};
    void refreshLegend();
};

#endif // PHASORWIDGET_H
