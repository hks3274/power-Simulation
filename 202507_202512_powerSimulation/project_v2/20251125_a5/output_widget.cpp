#include "output_widget.h"
#include <QBoxLayout>


output_widget::output_widget(datamanagement& dataMng)
    : dataMng(dataMng)
{
    setMinimumSize(800, 500);

    pChart = new painter_chart(this);

    QVBoxLayout* lay = new QVBoxLayout(this);
    lay->setContentsMargins(0,0,0,0);
    lay->addWidget(pChart);

    connect(&dataMng, &datamanagement::dataChanged, this, &output_widget::updateGraph);
}

void output_widget::updateGraph(QVector<QPair<double, double>> data) {

    points.clear();

    for(int i = 0; i < dataMng.data.size(); ++i) {
        double v = data[i].first;
        double t = data[i].second;
        points.append(QPointF(t, v));
    }

    pChart->updateData(points);
}


