#ifndef OUTPUT_WIDGET_H
#define OUTPUT_WIDGET_H

#include "datamanagement.h"
#include "painter_chart.h"
#include <QWidget>

#include <QPainter>
#include <QPen>
#include <QFont>
#include <QRect>

class output_widget : public QWidget
{
    Q_OBJECT
public:
    output_widget(datamanagement& dataMng);

private slots:
    void updateGraph(QVector<QPair<double, double>> data);

private:
    datamanagement& dataMng;

    painter_chart*  pChart = nullptr;

    QVector<QPointF> points;

    double simul_time = 0.0;
};

#endif // OUTPUT_WIDGET_H
