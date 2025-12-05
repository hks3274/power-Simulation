#include "a3700n_phasor.h"
#include <QtMath>
#include <QPainter>
#include <QLabel>
#include <QFrame>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGraphicseffect>
#include <QButtonGroup>

// --- 색상 지정 ---
const QColor VOLT_R(255,   0,   0);   // A상 전압: 빨강
const QColor VOLT_B(  0,   0, 255);   // B상 전압: 파랑
const QColor VOLT_G(  0, 160,   0);   // C상 전압: 초록

const QColor CURR_R(235, 133, 133);   // A상 전류: 연빨강
const QColor CURR_B(133, 184, 235);   // B상 전류: 연파랑
const QColor CURR_G(133, 235, 133);   // C상 전류: 연초록


a3700n_phasor::a3700n_phasor(dataManagement &dataMng, bool volCheck, bool curCheck)
    : dataMng(dataMng), volCheck(volCheck), curCheck(curCheck)
{
    setMinimumSize(600, 280);

    // --- 전체 레이아웃 ---
    QHBoxLayout* mainLay = new QHBoxLayout(this);
    mainLay->setContentsMargins(10, 0, 10, 10);
    mainLay->setSpacing(10);

    // (왼쪽) Phasor 영역
    QWidget* dummy = new QWidget(this);
    dummy->setMinimumSize(150, 150);
    dummy->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    dummy->setAttribute(Qt::WA_TranslucentBackground);
    dummy->setStyleSheet("background: transparent; border: none;");
    mainLay->addWidget(dummy, 1);

    // (오른쪽) 수치표
    QWidget* infoArea = new QWidget(this);
    QVBoxLayout* infoLay = new QVBoxLayout(infoArea);
    infoLay->setContentsMargins(10, 0, 10, 10);
    infoLay->setSpacing(6);

    // --- Voltage 그룹 ---
    voltGroup = new QWidget(this);
    QVBoxLayout* voltLay = new QVBoxLayout(voltGroup);
    voltLay->setContentsMargins(0, 0, 0, 0);
    voltLay->setSpacing(6);

    QHBoxLayout* vTitleHBoxLay = new QHBoxLayout();

    QLabel* voltTitle = new QLabel("Voltage");
    voltTitle->setStyleSheet("font-weight:bold; margin-top:10px; font-size: 15px; qproperty-alignment: 'AlignLeft | AlignVCenter';");
    voltTitle->setFixedHeight(30);

    vLNBtn = new QPushButton("VLN");
    vLNBtn->setCheckable(true);
    vLNBtn->setObjectName("harmBtn");
    vLNBtn->setStyleSheet("min-height: 20px; padding : 0px;");

    vLLBtn = new QPushButton("VLL");
    vLLBtn->setCheckable(true);
    vLLBtn->setObjectName("harmBtn");
    vLLBtn->setStyleSheet("min-height: 20px; padding : 0px;");

    QButtonGroup* btnGroup = new QButtonGroup();
    btnGroup->addButton(vLNBtn);
    btnGroup->addButton(vLLBtn);
    btnGroup->setExclusive(true);

    vLLBtn->setChecked(true);

    vTitleHBoxLay->addWidget(voltTitle);
    vTitleHBoxLay->addStretch(1);
    vTitleHBoxLay->addWidget(vLNBtn);
    vTitleHBoxLay->addWidget(vLLBtn);

    voltLay->addLayout(vTitleHBoxLay);

    QFrame* lineV = new QFrame();
    lineV->setFrameShape(QFrame::HLine);
    lineV->setStyleSheet("background-color:#333; height:1px;");
    voltLay->addWidget(lineV);

    auto addRow = [&](QString name, QString unit, QVector<QLabel*>& magList,
                      QVector<QLabel*>& phaseList, QVBoxLayout* targetLay)
    {
        QHBoxLayout* row = new QHBoxLayout();
        QLabel* n = new QLabel(name);

        QString style = "font-size:14px;"
                        "qproperty-alignment: 'AlignHCenter | AlignVCenter';"
                        "color : white;"
                        "border-radius: 2px;";
        if(name == "A" && unit == "V") style += "background-color:#FF0000;";
        if(name == "B" && unit == "V") style += "background-color:#0000FF;";
        if(name == "C" && unit == "V") style += "background-color:#00A000;";
        if(name == "A" && unit == "A") style += "background-color:#EB8585;";
        if(name == "B" && unit == "A") style += "background-color:#85B8EB;";
        if(name == "C" && unit == "A") style += "background-color:#85EB85;";
        n->setStyleSheet(style);

        QLabel* mag = new QLabel("--");
        mag->setStyleSheet("color:#377FEF; font-size:22px; font-weight:bold;");
        QLabel* phase = new QLabel("--");
        phase->setStyleSheet("color:#377FEF; font-size:22px; font-weight:bold; qproperty-alignment: 'AlignRight | AlignVCenter';");

        QLabel* u = new QLabel(unit);
        u->setStyleSheet("font-weight:bold; font-size:12px; qproperty-alignment: 'AlignHCenter | AlignBottom';");

        n->setFixedWidth(25);
        mag->setFixedWidth(80);
        phase->setFixedWidth(70);

        row->addWidget(n);
        row->addWidget(mag);
        row->addWidget(u);
        row->addStretch();
        row->addWidget(phase);
        targetLay->addLayout(row);

        magList.append(mag);
        phaseList.append(phase);
    };

    // Voltage rows
    for (auto n : {"A", "B", "C"}) addRow(n, "V", voltMagLabels, voltPhaseLabels, voltLay);
    infoLay->addWidget(voltGroup);

    // --- Current 그룹 ---
    currGroup = new QWidget(this);
    QVBoxLayout* currLay = new QVBoxLayout(currGroup);
    currLay->setContentsMargins(0, 0, 0, 0);
    currLay->setSpacing(6);

    QLabel* currTitle = new QLabel("Current");
    currTitle->setStyleSheet("font-weight:bold; margin-top:10px; font-size: 15px; qproperty-alignment: 'AlignLeft | AlignVCenter';");
    currTitle->setFixedHeight(30);
    currLay->addWidget(currTitle);

    QFrame* lineC = new QFrame();
    lineC->setFrameShape(QFrame::HLine);
    lineC->setStyleSheet("background-color:#333; height:1px;");
    currLay->addWidget(lineC);

    for (auto n : {"A", "B", "C"}) addRow(n, "A", currMagLabels, currPhaseLabels, currLay);
    infoLay->addWidget(currGroup);
    infoLay->addStretch();

    mainLay->addWidget(infoArea, 1);

    phasorCalc = new phasorCalculator(dataMng);

    connect(vLLBtn, &QPushButton::toggled, this, [this](){
        nowUpdate(lastPh);
    });

    connect(&dataMng, &dataManagement::rmsDataChanged,
            this, & a3700n_phasor::updatePhasor);

}


// 데이터 업데이트
void a3700n_phasor::updatePhasor(measure_data m)
{
    auto elapsed = m.t - prev_sumTime;

    auto ph = phasorCalc->calcPhasor();

    lastPh = ph;

    if(elapsed >= std::chrono::milliseconds(995)){
        nowUpdate(lastPh);

        prev_sumTime = m.t;

    }
}


void a3700n_phasor::nowUpdate(phasor_data ph) {

    if(vLLBtn->isChecked()){
        voltMagLabels[0]->setText(QString::number(ph.vALLMag, 'f', 2));
        voltMagLabels[1]->setText(QString::number(ph.vBLLMag, 'f', 2));
        voltMagLabels[2]->setText(QString::number(ph.vCLLMag, 'f', 2));
        voltPhaseLabels[0]->setText(QString::number(ph.vALLPhaseDeg, 'f', 1)+"˚");
        voltPhaseLabels[1]->setText(QString::number(ph.vBLLPhaseDeg, 'f', 1)+"˚");
        voltPhaseLabels[2]->setText(QString::number(ph.vCLLPhaseDeg, 'f', 1)+"˚");

        vAX = ph.vALLReal; vAY = ph.vALLImag;
        vBX = ph.vBLLReal; vBY = ph.vBLLImag;
        vCX = ph.vCLLReal; vCY = ph.vCLLImag;

    }else {
        voltMagLabels[0]->setText(QString::number(ph.vAMag, 'f', 2));
        voltMagLabels[1]->setText(QString::number(ph.vBMag, 'f', 2));
        voltMagLabels[2]->setText(QString::number(ph.vCMag, 'f', 2));
        voltPhaseLabels[0]->setText(QString::number(ph.vAPhaseDeg, 'f', 1)+"˚");
        voltPhaseLabels[1]->setText(QString::number(ph.vBPhaseDeg, 'f', 1)+"˚");
        voltPhaseLabels[2]->setText(QString::number(ph.vCPhaseDeg, 'f', 1)+"˚");

        vAX = ph.vAReal; vAY = ph.vAImag;
        vBX = ph.vBReal; vBY = ph.vBImag;
        vCX = ph.vCReal; vCY = ph.vCImag;
    }


    currMagLabels[0]->setText(QString::number(ph.cAMag, 'f', 3));
    currMagLabels[1]->setText(QString::number(ph.cBMag, 'f', 3));
    currMagLabels[2]->setText(QString::number(ph.cCMag, 'f', 3));
    currPhaseLabels[0]->setText(QString::number(ph.cAPhaseDeg, 'f', 1)+"˚");
    currPhaseLabels[1]->setText(QString::number(ph.cBPhaseDeg, 'f', 1)+"˚");
    currPhaseLabels[2]->setText(QString::number(ph.cCPhaseDeg, 'f', 1)+"˚");


    cAX = ph.cAReal; cAY = ph.cAImag;
    cBX = ph.cBReal; cBY = ph.cBImag;
    cCX = ph.cCReal; cCY = ph.cCImag;

    update();
    repaint();
}


// paintEvent에서 직접 그림
void a3700n_phasor::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setRenderHint(QPainter::SmoothPixmapTransform, true);


    const int W = width(), H = height();
    const QPointF C(W * 0.25, H * 0.5);  // 중심 (왼쪽으로 약간)
    const int pad = 12;

    // 원 크기
    const int Rpx    = qMin(W, H) * 0.48 - pad;   // 외부 원 (전압)
    const int Rinner = Rpx * 0.7;                // 내부 원 (전류)

    // --- 배경/축 ---
    p.fillRect(rect(), QColor(255, 255, 255));
    p.setPen(QPen(QColor(210, 210, 210), 1));

    // 외부/내부 원
    p.drawEllipse(C, Rpx, Rpx);
    p.drawEllipse(C, Rinner, Rinner);

    // 수평축 (외부 원까지만)
    p.drawLine(QPointF(C.x() - Rpx, C.y()),
               QPointF(C.x() + Rpx, C.y()));

    // 수직축 (외부 원 전체 길이)
    p.drawLine(QPointF(C.x(), C.y() - Rpx),
               QPointF(C.x(), C.y() + Rpx));

    // --- 고정 길이 (위상만 표시용) ---
    const double sV  = Rpx * 0.99;
    const double sC  = Rinner * 0.99;

    // --- 유틸 함수 ---
    auto phase = [](double x, double y){ return std::atan2(y, x); };
    auto dirX  = [&](double x, double y){ return std::cos(phase(x, y)); };
    auto dirY  = [&](double x, double y){ return std::sin(phase(x, y)); };
    auto valid = [](double x, double y){ return std::hypot(x, y) > 1e-12; };

    // --- 화살표 그리기 ---
    auto drawArrow = [&](double x, double y, double s, const QColor& color,
                         int width, bool dashed = false, qreal alpha = 1.0,
                         const QString& label = QString())
    {
        if (!valid(x, y)) return;
        QColor c = color; c.setAlphaF(std::clamp(alpha, 0.0, 1.0));
        QPen pen(c, width);
        if (dashed) pen.setStyle(Qt::DashLine);
        p.setPen(pen);

        const QPointF P(C.x() + x * s, C.y() - y * s); // y 반전
        p.drawLine(C, P);

        // --- 화살촉 (삼각형) ---
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


    // --- 기본파: 전압 ---
    if (volCheck) {
        drawArrow(dirX(vAX, vAY), dirY(vAX, vAY), sV, VOLT_R, 3, false, 1.0, "");
        drawArrow(dirX(vBX, vBY), dirY(vBX, vBY), sV, VOLT_B, 3, false, 1.0, "");
        drawArrow(dirX(vCX, vCY), dirY(vCX, vCY), sV, VOLT_G, 3, false, 1.0, "");
    }

    // --- 기본파: 전류 ---
    if (curCheck) {
        drawArrow(dirX(cAX, cAY), dirY(cAX, cAY), sC, CURR_R, 2, false, 1.0, "");
        drawArrow(dirX(cBX, cBY), dirY(cBX, cBY), sC, CURR_B, 2, false, 1.0, "");
        drawArrow(dirX(cCX, cCY), dirY(cCX, cCY), sC, CURR_G, 2, false, 1.0, "");
    }

}


//투명도를 조정해서 안보이게 하기..
void a3700n_phasor::setVolCheck(bool enabled)
{
    volCheck = enabled;
    if (voltGroup) {
        auto eff = new QGraphicsOpacityEffect(this);
        eff->setOpacity(enabled ? 1.0 : 0.0);
        voltGroup->setGraphicsEffect(eff);
    }
    update();
}

void a3700n_phasor::setCurCheck(bool enabled)
{
    curCheck = enabled;
    if (currGroup) {
        auto eff = new QGraphicsOpacityEffect(this);
        eff->setOpacity(enabled ? 1.0 : 0.0);
        currGroup->setGraphicsEffect(eff);
    }
    update();
}
