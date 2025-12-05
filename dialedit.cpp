#include "dialedit.h"
#include <QLabel>
#include <QFrame>
#include <QBoxLayout>
#include <QFont>


dialEdit::dialEdit(QString lblName, int value)
{
    QFont boldFont("나눔고딕", 10, QFont::Bold);

    auto frame = new QFrame(this);
    frame->setFrameStyle(QFrame::Box | QFrame::Plain);
    frame->setLineWidth(1);
    frame->setObjectName("phaseFrameA_I");
    frame->setStyleSheet("#phaseFrameA_I { border: 1px solid #ccc; border-radius: 5px; background-color: #eee; }");

    auto lay = new QHBoxLayout(frame);
    QLabel* lbl = new QLabel(lblName);
    lbl->setFont(boldFont);

    phaseDial = new QDial(frame);
    phaseDial->setRange(-180, 180);
    phaseDial->setValue(value);
    phaseDial->setNotchesVisible(true);

    phaseSpin = new QSpinBox(frame);
    phaseSpin->setRange(-180, 180);
    phaseSpin->setValue(value);
    phaseSpin->setSuffix("°");

    lay->addWidget(lbl);
    lay->addWidget(phaseDial);
    lay->addWidget(phaseSpin);

    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    mainLayout->addWidget(frame);

    // === 연동 ===
    connect(phaseDial, &QDial::valueChanged, this, &dialEdit::onDialChanged);
    connect(phaseSpin, &QSpinBox::valueChanged, this, &dialEdit::onSpinChanged);

}

void dialEdit::onDialChanged(int deg) {
    phaseSpin->setValue(deg);
}

void dialEdit::onSpinChanged(int deg) {
    phaseDial->setValue(deg);
    qDebug() << deg;
    emit valueChanged(deg);
}


void dialEdit::setDialValue(int newVal)
{
    if (phaseDial) {
        phaseDial->setValue(newVal);
    }
}
