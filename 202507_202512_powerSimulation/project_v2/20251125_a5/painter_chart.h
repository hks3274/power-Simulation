#ifndef PAINTER_CHART_H
#define PAINTER_CHART_H

#include <QWidget>
#include <QPainter>
#include <QString>

class painter_chart :public QWidget
{
    Q_OBJECT
public:
    painter_chart(QWidget* parent);

    void updateData(const QVector<QPointF> &pts);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QVector<QPointF> m_plotPoints;

    double windowSec    = 5.0;
    double m_latestTime = 0.0;
};

#endif // PAINTER_CHART_H
