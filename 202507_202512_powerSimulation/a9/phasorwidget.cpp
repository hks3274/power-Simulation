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

    // 초기 텍스트
    refreshLegend();
}


void phasorWidget::resizeEvent(QResizeEvent* e) {
    QWidget::resizeEvent(e);
    const int m = 8;            // margin
    cbV->move(width() - cbV->sizeHint().width() - m, m);
    cbI->move(width() - cbI->sizeHint().width() - m,
               m + cbV->sizeHint().height() + 4);
}

void phasorWidget::updatePhasor(dataManagement::measure_data)
{
    auto ph = dataMng.calcPhasor();   // vReal/vImag, cReal/cImag  값 가져옴
    vX = ph.vReal;  vY = ph.vImag;
    cX = ph.cReal;  cY = ph.cImag;

    refreshLegend();
    update();
}


//체크박스 텍스트 리
void phasorWidget::refreshLegend()
{
    // 진폭(피크)은 dataMng에서, 위상은 phasor(x,y)로 계산
    auto ph = dataMng.calcPhasor();

    // 텍스트에 위상/크기 쓰고 싶으면:
    QString vTxt = QString("전압 %1V, %2°")
                       .arg(ph.vMag, 0, 'f', 2)
                       .arg(ph.vPhaseDeg, 0, 'f', 0);
    QString iTxt = QString("전류 %1V, %2°")
                       .arg(ph.cMag, 0, 'f', 2)
                       .arg(ph.cPhaseDeg, 0, 'f', 0);

    // 텍스트 적용 + 실제 위젯 크기 재산출
    cbV->setText(vTxt);
    cbI->setText(iTxt);
    cbV->adjustSize();
    cbI->adjustSize();

    // 우상단에 다시 배치(오른쪽에 딱 맞게)
    const int m = 8;
    const int maxW = std::max(cbV->width(), cbI->width());
    const int x = width() - maxW - m;
    cbV->move(x, m);
    cbI->move(x, m + cbV->height() + 4);
}



void phasorWidget::paintEvent(QPaintEvent*)
{
    QPainter p(this); // 위젯 표면에 그림을 그리겠다 선언
    p.setRenderHint(QPainter::Antialiasing, true); // 곡선을 부드럽게

    const int W = width(); // 위젯의 폭
    const int H = height(); //위젯의 높이
    const QPointF C(W*0.5, H*0.5); // 화면의 중심정, 원점
    const int Rpx = qMin(W, H)/2 - pad; //그릴 큰 원의 반지름, 위젯의 짧은 변 기준으로 반지름을 잡고, pad만큼 여백 빼서 안 잘리게 함.
    const int Rinner = Rpx *2 / 3;     // 안쪽(전류) 원 반지름


    //간단한 오토스케일: 벡터 안 원에 살짝 여유 잇게 들어오도록함
    const double vMag = std::hypot(vX, vY);
    const double iMag = std::hypot(cX, cY);
    const double sV = Rpx    / qMax(1.0, vMag * 1.1);   // 전압은 큰 원 안
    const double sC = Rinner / qMax(1.0, iMag * 1.1);   // 전류는 작은 원 안

    //배경
    p.fillRect(rect(), QColor(245,245,245));

    //원/축
    p.setPen(QPen(QColor(210,210,210), 1));
    p.drawEllipse(C, Rpx, Rpx);           // 바깥 원(전압)
    p.drawEllipse(C, Rinner, Rinner);     // 안쪽 원(전류)
    p.drawLine(QPointF(pad, C.y()), QPointF(W - pad, C.y())); // x축
    p.drawLine(QPointF(C.x(), pad), QPointF(C.x(), H - pad)); // y축

    auto drawArrow = [&](double x, double y, double s, const QColor& color, int width)
    {
        const QPointF P(C.x() + x*s, C.y() - y*s); // y 반전 (Qt는 아래쪽이 +이기 때문에)
        p.setPen(QPen(color, width));
        p.drawLine(C, P);

        // 화살촉
        const double ang = std::atan2(y, x); // atan2(y,x) 좌표 (y,x)의 극각도를 반환하는 함수 => 결과가 -180° ~ +180° 로 줌
        const QPointF dir(std::cos(ang), -std::sin(ang)); // y반전 반영
        const QPointF nrm(-dir.y(), dir.x());
        const double ah = 10.0, aw = 6.0;
        QPolygonF tri;
        tri << P
            << (P - dir*ah + nrm*(aw*0.5))
            << (P - dir*ah - nrm*(aw*0.5));
        p.setBrush(color);
        p.drawPolygon(tri);
    };

    // 전압(파랑)
    if (showV) drawArrow(vX, vY, sV, QColor(30,80,200), 3);
    // 전류(주황)
    if (showI) drawArrow(cX, cY, sC, QColor(220,90,30), 2);

}
