#include "phasorwidget.h"
#include <QString>
#include <QPainter>
#include <QtMath>

phasorWidget::phasorWidget(dataManagement& dataMng)
    : dataMng(dataMng)
{
    setMinimumSize(220, 180);
    setAttribute(Qt::WA_OpaquePaintEvent, true);

    // 한 주기마다 갱신
    connect(&dataMng, &dataManagement::rmsDataChanged,
            this, &phasorWidget::updatePhasor);

    // 초기 상태
    showVa = showVb = showVc = true;
    showIa = showIb = showIc = true;
    showVH = showIH = true;
}

void phasorWidget::updatePhasor(dataManagement::measure_data)
{
    auto ph = dataMng.calcPhasor();

    // 기본파(3상)
    vAX = ph.vAReal; vAY = ph.vAImag;
    vBX = ph.vBReal; vBY = ph.vBImag;
    vCX = ph.vCReal; vCY = ph.vCImag;

    cAX = ph.cAReal; cAY = ph.cAImag;
    cBX = ph.cBReal; cBY = ph.cBImag;
    cCX = ph.cCReal; cCY = ph.cCImag;

    // 고조파(A상만)
    hvX = ph.hvReal; hvY = ph.hvImag;
    hcX = ph.hcReal; hcY = ph.hcImag;

    update();
}

// 외부 체크박스 제어용
void phasorWidget::setPhasorVisible(const QString& key, bool visible)
{
    // 전압
    if (key.contains("전압_A")) showVa = visible;
    else if (key.contains("전압_B")) showVb = visible;
    else if (key.contains("전압_C")) showVc = visible;

    // 전류
    else if (key.contains("전류_A")) showIa = visible;
    else if (key.contains("전류_B")) showIb = visible;
    else if (key.contains("전류_C")) showIc = visible;

    // 고조파 (A상만)
    else if (key.contains("전압_고조파")) showVH = visible;
    else if (key.contains("전류_고조파")) showIH = visible;

    update();
}

void phasorWidget::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setRenderHint(QPainter::SmoothPixmapTransform, true);

    const int W = width(), H = height();
    const QPointF C(W * 0.5, H * 0.5);
    const int Rpx = qMin(W, H) / 2.5 - pad;
    const int Rinner = Rpx * 0.7;

    // 배경/축
    p.fillRect(rect(), QColor(245, 245, 245));
    p.setPen(QPen(QColor(210, 210, 210), 1));
    p.drawEllipse(C, Rpx, Rpx);
    p.drawEllipse(C, Rinner, Rinner);
    p.drawLine(QPointF(pad, C.y()), QPointF(W - pad, C.y()));
    p.drawLine(QPointF(C.x(), pad), QPointF(C.x(), H - pad));

    // 위상 기반 고정 길이
    const double sV  = Rpx * 0.90;
    const double sC  = Rinner * 0.90;
    const double sVH = Rpx * 0.85;
    const double sIH = Rinner * 0.85;

    auto phase = [](double x, double y){ return std::atan2(y, x); };
    auto dirX  = [&](double x, double y){ return std::cos(phase(x, y)); };
    auto dirY  = [&](double x, double y){ return std::sin(phase(x, y)); };
    auto valid = [](double x, double y){ return std::hypot(x, y) > 1e-12; };

    auto drawArrow = [&](double x, double y, double s, const QColor& color,
                         int width, bool dashed = false, qreal alpha = 1.0,
                         const QString& label = QString())
    {
        QColor c = color; c.setAlphaF(std::clamp(alpha, 0.0, 1.0));
        QPen pen(c, width);
        if (dashed) pen.setStyle(Qt::DashLine);
        p.setPen(pen);

        const QPointF P(C.x() + x * s, C.y() - y * s); // y 반전
        p.drawLine(C, P);

        // 화살촉
        const double ang = std::atan2(y, x);
        const QPointF dir(std::cos(ang), -std::sin(ang));
        const QPointF nrm(-dir.y(), dir.x());
        const double ah = 10.0, aw = 6.0;
        QPolygonF tri;
        tri << P
            << (P - dir * ah + nrm * (aw * 0.5))
            << (P - dir * ah - nrm * (aw * 0.5));
        p.setBrush(c);
        p.drawPolygon(tri);

        if (!label.isEmpty()) {
            QFont f = p.font(); f.setPointSizeF(f.pointSizeF() * 0.9);
            p.setFont(f);
            p.setPen(QPen(QColor(60, 60, 60, 200)));
            p.drawText(P + QPointF(6, -4), label);
        }
    };

    // 색상 팔레트
    const QColor VOLT_R(255, 0, 0), VOLT_B(0, 0, 255), VOLT_G(0, 160, 0);
    const QColor CURR_R(235,133,133), CURR_B(133,184,235), CURR_G(133,235,133);

    // 기본파: 전압(3상)
    if (showVa && valid(vAX, vAY)) drawArrow(dirX(vAX, vAY), dirY(vAX, vAY), sV, VOLT_R, 3, false, 1.0, "Va");
    if (showVb && valid(vBX, vBY)) drawArrow(dirX(vBX, vBY), dirY(vBX, vBY), sV, VOLT_B, 3, false, 1.0, "Vb");
    if (showVc && valid(vCX, vCY)) drawArrow(dirX(vCX, vCY), dirY(vCX, vCY), sV, VOLT_G, 3, false, 1.0, "Vc");

    // 기본파: 전류(3상)
    if (showIa && valid(cAX, cAY)) drawArrow(dirX(cAX, cAY), dirY(cAX, cAY), sC, CURR_R, 2, false, 1.0, "Ia");
    if (showIb && valid(cBX, cBY)) drawArrow(dirX(cBX, cBY), dirY(cBX, cBY), sC, CURR_B, 2, false, 1.0, "Ib");
    if (showIc && valid(cCX, cCY)) drawArrow(dirX(cCX, cCY), dirY(cCX, cCY), sC, CURR_G, 2, false, 1.0, "Ic");

    // 고조파(A상만)
    if (showVH && valid(hvX, hvY)) drawArrow(dirX(hvX, hvY), dirY(hvX, hvY), sVH, VOLT_R, 2, true, 0.8, "Vh");
    if (showIH && valid(hcX, hcY)) drawArrow(dirX(hcX, hcY), dirY(hcX, hcY), sIH, CURR_R, 2, true, 0.8, "Ih");
}
