#include "phasorwidget.h"
#include<QString>


phasorWidget::phasorWidget(dataManagement& dataMng)
    :dataMng(dataMng)
{
    setMinimumSize(220, 180);

    setAttribute(Qt::WA_OpaquePaintEvent, true);

    // rmsDataChanged 시그널에 맞춰 업데이트(한 주기마다)
    connect(&dataMng, &dataManagement::rmsDataChanged,
            this, &phasorWidget::updatePhasor);

    cbV = new QCheckBox( this);
    cbI = new QCheckBox(this);
    cbV->setChecked(true);
    cbI->setChecked(true);
    connect(cbV, &QCheckBox::toggled, this, &phasorWidget::toggleV);
    connect(cbI, &QCheckBox::toggled, this, &phasorWidget::toggleI);

    cbVH = new QCheckBox(this);
    cbIH = new QCheckBox(this);
    cbVH->setChecked(true);
    cbIH->setChecked(true);
    connect(cbVH, &QCheckBox::toggled, this, [&](bool on){ showVH = on; update(); });
    connect(cbIH, &QCheckBox::toggled, this, [&](bool on){ showIH = on; update(); });


    // 초기 텍스트
    refreshLegend();
}


void phasorWidget::resizeEvent(QResizeEvent* e) {
    QWidget::resizeEvent(e);
    const int m = 8;
    // 폭 가장 긴 것 기준 오른쪽 정렬
    int x = width() - m;
    // 위에서부터 줄줄이 내리기
    auto place = [&](QCheckBox* cb, int y){
        if (!cb) return y;
        cb->move(x - cb->sizeHint().width(), y);
        return y + cb->sizeHint().height() + 4;
    };
    int y = m;
    y = place(cbV,  y);
    y = place(cbI,  y);
    y = place(cbVH, y);
    y = place(cbIH, y);
}

void phasorWidget::updatePhasor(dataManagement::measure_data) {
    auto ph = dataMng.calcPhasor();

    // 기본파
    vX = ph.vReal;  vY = ph.vImag;
    cX = ph.cReal;  cY = ph.cImag;

    // 고조파
    hvX = ph.hvReal; hvY = ph.hvImag;
    hcX = ph.hcReal; hcY = ph.hcImag;

    refreshLegend();
    update();
}


//체크박스 텍스트 리
void phasorWidget::refreshLegend()
{
    auto ph = dataMng.calcPhasor();
    // 기본파
    QString vTxt = QString("전압 %1, %2°")
                       .arg(ph.vMag, 0, 'f', 2)
                       .arg(ph.vPhaseDeg, 0, 'f', 0);
    QString iTxt = QString("전류 %1, %2°")
                       .arg(ph.cMag, 0, 'f', 2)
                       .arg(ph.cPhaseDeg, 0, 'f', 0);

    // 고조파
    QString vhTxt = QString("전압 h=%1  %2, %3°")
                        .arg(ph.vOrderUsed)                       // calcPhasorHarmonic에서 채움
                        .arg(ph.hvMag, 0, 'f', 2)
                        .arg(ph.hvPhaseDeg, 0, 'f', 0);
    QString ihTxt = QString("전류 h=%1  %2, %3°")
                        .arg(ph.cOrderUsed)
                        .arg(ph.hcMag, 0, 'f', 2)
                        .arg(ph.hcPhaseDeg, 0, 'f', 0);

    cbV->setText(vTxt);
    cbI->setText(iTxt);
    cbVH->setText(vhTxt);
    cbIH->setText(ihTxt);

    cbV->adjustSize(); cbI->adjustSize();
    cbVH->adjustSize(); cbIH->adjustSize();

    // 배치는 resizeEvent에서 처리
}




void phasorWidget::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setRenderHint(QPainter::SmoothPixmapTransform, true);

    const int W = width();
    const int H = height();
    const QPointF C(W*0.5, H*0.5);
    const int Rpx    = qMin(W, H)/2 - pad;   // 전압 원
    const int Rinner = Rpx * 2 / 3;          // 전류 원

    // 배경/축
    p.fillRect(rect(), QColor(245,245,245));
    p.setPen(QPen(QColor(210,210,210), 1));
    p.drawEllipse(C, Rpx, Rpx);
    p.drawEllipse(C, Rinner, Rinner);
    p.drawLine(QPointF(pad, C.y()), QPointF(W - pad, C.y()));
    p.drawLine(QPointF(C.x(), pad), QPointF(C.x(), H - pad));

    // 오토스케일: 기본파/고조파 중 큰 쪽에 맞춤
    const double vMag = std::hypot(vX,  vY);
    const double iMag = std::hypot(cX,  cY);

    const double sV = Rpx    / qMax(1.0, vMag * 1.1);
    const double sC = Rinner / qMax(1.0, iMag * 1.1);

    const double hvMag = std::hypot(hvX, hvY);
    const double hcMag = std::hypot(hcX, hcY);

    const double sVH = Rpx    / qMax(1.0, hvMag * 1.1);   // 전압-고조파
    const double sIH = Rinner / qMax(1.0, hcMag * 1.1);   // 전류-고조파

    auto drawArrow = [&](double x, double y, double s, const QColor& color, int width, bool dashed=false, qreal alpha=1.0)
    {
        QPen pen(color, width);
        if (dashed) pen.setStyle(Qt::DashLine);
        QColor c = color; c.setAlphaF(std::clamp(alpha, 0.0, 1.0));
        pen.setColor(c);
        p.setPen(pen);

        const QPointF P(C.x() + x*s, C.y() - y*s); // y 반전
        p.drawLine(C, P);

        // 화살촉
        const double ang = std::atan2(y, x);
        const QPointF dir(std::cos(ang), -std::sin(ang));
        const QPointF nrm(-dir.y(), dir.x());
        const double ah = 10.0, aw = 6.0;
        QPolygonF tri;
        tri << P
            << (P - dir*ah + nrm*(aw*0.5))
            << (P - dir*ah - nrm*(aw*0.5));
        p.setBrush(c);
        p.drawPolygon(tri);
    };

    // 기본파
    if (showV) drawArrow(vX,  vY,  sV, QColor(30,80,200),   3);
    if (showI) drawArrow(cX,  cY,  sC, QColor(220,90,30),   2);

    // 고조파(반투명/점선으로 구분)
    if (showVH) drawArrow(hvX, hvY, sVH, QColor(30,80,200),  2, /*dashed=*/true);
    if (showIH) drawArrow(hcX, hcY, sIH, QColor(220,90,30),  2, /*dashed=*/true);
}
