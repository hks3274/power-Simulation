#include "phasorwidget.h"
#include<QString>


phasorWidget::phasorWidget(dataManagement& dataMng)
    : dataMng(dataMng)
{
    setMinimumSize(220, 180);
    setAttribute(Qt::WA_OpaquePaintEvent, true);

    // 한 주기마다 갱신
    connect(&dataMng, &dataManagement::rmsDataChanged,
            this, &phasorWidget::updatePhasor);

    // -------- 체크박스 생성 --------
    cbVa = new QCheckBox(this);
    cbVb = new QCheckBox(this);
    cbVc = new QCheckBox(this);
    cbIa = new QCheckBox(this);
    cbIb = new QCheckBox(this);
    cbIc = new QCheckBox(this);
    cbVH = new QCheckBox(this);
    cbIH = new QCheckBox(this);

    // 기본 상태
    cbVa->setChecked(true);
    cbVb->setChecked(true);
    cbVc->setChecked(true);
    cbIa->setChecked(true);
    cbIb->setChecked(true);
    cbIc->setChecked(true);
    cbVH->setChecked(true);
    cbIH->setChecked(true);

    // 토글 연결 (람다로 간단히)
    connect(cbVa, &QCheckBox::toggled, this, [&](bool on){ showVa = on; update(); });
    connect(cbVb, &QCheckBox::toggled, this, [&](bool on){ showVb = on; update(); });
    connect(cbVc, &QCheckBox::toggled, this, [&](bool on){ showVc = on; update(); });
    connect(cbIa, &QCheckBox::toggled, this, [&](bool on){ showIa = on; update(); });
    connect(cbIb, &QCheckBox::toggled, this, [&](bool on){ showIb = on; update(); });
    connect(cbIc, &QCheckBox::toggled, this, [&](bool on){ showIc = on; update(); });
    connect(cbVH, &QCheckBox::toggled, this, [&](bool on){ showVH = on; update(); });
    connect(cbIH, &QCheckBox::toggled, this, [&](bool on){ showIH = on; update(); });

    // 초기 텍스트
    refreshLegend();
}

void phasorWidget::resizeEvent(QResizeEvent* e) {
    QWidget::resizeEvent(e);

    // 여백/간격
    const int m   = 8;   // 바깥 여백
    const int sX  = 10;  // 체크박스 가로 간격
    const int sY  = 6;   // 행 간 세로 간격

    // 폰트 작게(여기서도 한 번 더 보정)
    QFont small = font();
    small.setPointSize(8);
    for (QCheckBox* cb : {cbVa,cbVb,cbVc,cbIa,cbIb,cbIc,cbVH,cbIH}) {
        if (!cb) continue;
        cb->setFont(small);
        cb->adjustSize();
    }

    // 한 행을 좌->우로 배치하는 헬퍼
    auto placeRow = [&](int y, std::initializer_list<QCheckBox*> cbs)->int {
        int x = m;
        int rowH = 0;
        for (QCheckBox* cb : cbs) {
            if (!cb) continue;
            // 줄바꿈(폭이 너무 좁을 때 자동 개행)
            if (x + cb->sizeHint().width() + m > width()) {
                x = m;
                y += rowH + sY;
                rowH = 0;
            }
            cb->move(x, y);
            x += cb->sizeHint().width() + sX;
            rowH = std::max(rowH, cb->sizeHint().height());
        }
        return y + rowH; // 다음 행의 y 시작값 반환
    };

    int y = m;
    y = placeRow(y, { cbVa, cbVb, cbVc });  // 전압: A B C
    y += sY;
    y = placeRow(y, { cbIa, cbIb, cbIc });  // 전류: A B C
    y += sY;
    y = placeRow(y, { cbVH, cbIH });        // 고조파: V/I
}

void phasorWidget::updatePhasor(dataManagement::measure_data) {
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

    refreshLegend();
    update();
}


//상별 라벨
void phasorWidget::refreshLegend() {
    auto ph = dataMng.calcPhasor();

    // 전압(Va/Vb/Vc)
    QString vaTxt = QString("Va %1, %2°").arg(ph.vAMag, 0, 'f', 2).arg(ph.vAPhaseDeg, 0, 'f', 0);
    QString vbTxt = QString("Vb %1, %2°").arg(ph.vBMag, 0, 'f', 2).arg(ph.vBPhaseDeg, 0, 'f', 0);
    QString vcTxt = QString("Vc %1, %2°").arg(ph.vCMag, 0, 'f', 2).arg(ph.vCPhaseDeg, 0, 'f', 0);

    // 전류(Ia/Ib/Ic)
    QString iaTxt = QString("Ia %1, %2°").arg(ph.cAMag, 0, 'f', 2).arg(ph.cAPhaseDeg, 0, 'f', 0);
    QString ibTxt = QString("Ib %1, %2°").arg(ph.cBMag, 0, 'f', 2).arg(ph.cBPhaseDeg, 0, 'f', 0);
    QString icTxt = QString("Ic %1, %2°").arg(ph.cCMag, 0, 'f', 2).arg(ph.cCPhaseDeg, 0, 'f', 0);

    // 고조파(A상만)
    QString vhTxt = QString("Vh h=%1, %2, %3°")
                        .arg(ph.vOrderUsed)
                        .arg(ph.hvMag, 0, 'f', 2)
                        .arg(ph.hvPhaseDeg, 0, 'f', 0);
    QString ihTxt = QString("Ih h=%1, %2, %3°")
                        .arg(ph.cOrderUsed)
                        .arg(ph.hcMag, 0, 'f', 2)
                        .arg(ph.hcPhaseDeg, 0, 'f', 0);

    cbVa->setText(vaTxt);
    cbVb->setText(vbTxt);
    cbVc->setText(vcTxt);
    cbIa->setText(iaTxt);
    cbIb->setText(ibTxt);
    cbIc->setText(icTxt);
    cbVH->setText(vhTxt);
    cbIH->setText(ihTxt);

    QFont small = cbVa->font();
    small.setPointSize(8);
    for (QCheckBox* cb : {cbVa,cbVb,cbVc,cbIa,cbIb,cbIc,cbVH,cbIH}) {
        cb->setFont(small);
        cb->adjustSize();
    }
}



void phasorWidget::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setRenderHint(QPainter::SmoothPixmapTransform, true);

    const int W = width(), H = height();
    const QPointF C(W*0.5, H*0.5);
    const int Rpx = qMin(W, H)/2 - pad;   // 전압 원
    const int Rinner = Rpx * 2 / 3;       // 전류 원

    // 배경/축
    p.fillRect(rect(), QColor(245,245,245));
    p.setPen(QPen(QColor(210,210,210), 1));
    p.drawEllipse(C, Rpx, Rpx);
    p.drawEllipse(C, Rinner, Rinner);
    p.drawLine(QPointF(pad, C.y()), QPointF(W - pad, C.y()));
    p.drawLine(QPointF(C.x(), pad), QPointF(C.x(), H - pad));

    // 위상 기반 고정 길이
    const double sV  = Rpx    * 0.90;
    const double sC  = Rinner * 0.90;
    const double sVH = Rpx    * 0.85;
    const double sIH = Rinner * 0.85;

    auto phase = [](double x, double y){ return std::atan2(y, x); };
    auto dirX  = [&](double x, double y){ return std::cos(phase(x,y)); };
    auto dirY  = [&](double x, double y){ return std::sin(phase(x,y)); };
    auto valid = [](double x, double y){ return std::hypot(x,y) > 1e-12; };

    auto drawArrow = [&](double x, double y, double s, const QColor& color, int width,
                         bool dashed=false, qreal alpha=1.0, const QString& label=QString())
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

        if (!label.isEmpty()) {
            QFont f = p.font(); f.setPointSizeF(f.pointSizeF()*0.9);
            p.setFont(f);
            p.setPen(QPen(QColor(60,60,60,200)));
            p.drawText(P + QPointF(6,-4), label);
        }
    };

    // 색상 팔레트
    const QColor VOLT_R(255, 0, 0), VOLT_B(0, 0, 255), VOLT_G(0, 160, 0);
    const QColor CURR_R(235,133,133), CURR_B(133,184,235), CURR_G(133,235,133);

    // 기본파: 전압(3상)
    if (showVa && valid(vAX,vAY)) drawArrow(dirX(vAX,vAY), dirY(vAX,vAY), sV, VOLT_R, 3, false, 1.0, "Va");
    if (showVb && valid(vBX,vBY)) drawArrow(dirX(vBX,vBY), dirY(vBX,vBY), sV, VOLT_B, 3, false, 1.0, "Vb");
    if (showVc && valid(vCX,vCY)) drawArrow(dirX(vCX,vCY), dirY(vCX,vCY), sV, VOLT_G, 3, false, 1.0, "Vc");

    // 기본파: 전류(3상)
    if (showIa && valid(cAX,cAY)) drawArrow(dirX(cAX,cAY), dirY(cAX,cAY), sC, CURR_R, 2, false, 1.0, "Ia");
    if (showIb && valid(cBX,cBY)) drawArrow(dirX(cBX,cBY), dirY(cBX,cBY), sC, CURR_B, 2, false, 1.0, "Ib");
    if (showIc && valid(cCX,cCY)) drawArrow(dirX(cCX,cCY), dirY(cCX,cCY), sC, CURR_G, 2, false, 1.0, "Ic");

    // 고조파(A상만)
    if (showVH && valid(hvX,hvY)) drawArrow(dirX(hvX,hvY), dirY(hvX,hvY), sVH, VOLT_R, 2, /*dashed=*/true, 0.8, "Vh");
    if (showIH && valid(hcX,hcY)) drawArrow(dirX(hcX,hcY), dirY(hcX,hcY), sIH,  CURR_R, 2, /*dashed=*/true, 0.8, "Ih");
}



