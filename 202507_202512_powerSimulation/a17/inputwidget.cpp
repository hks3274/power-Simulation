#include "inputwidget.h"

#include <QBoxLayout>

inputWidget::inputWidget(dataManagement& dataMng) //생성자를 호출할 때 넘겨는주는 인자(argument), 생성자의 매개변수
    : dataMng(dataMng) // 멤버 초기화 리스트,  클래스 내부에 있는 멤버 변수 dataMng를 매개변수 dataMng로 초기화
{
    auto vl = new QVBoxLayout(this);
    vl->setContentsMargins(8, 8, 8, 8);
    vl->setSpacing(8);

    QFont boldFont("나눔고딕", 10, QFont::Bold);

    // ===== 상 A =====
    {
        // 전압 A : 크기만
        vASlideEdit = new slideEdit("전압 A (V)", dataMng.getVAValue(), 500.0, -500.0);
        vl->addWidget(vASlideEdit);

        // 전류 A : 크기
        iASlideEdit = new slideEdit("전류 A (A)", dataMng.getIAValue(), 20.0, -20.0);
        vl->addWidget(iASlideEdit);

        // 전류 A : 위상 (아래 줄에 다이얼)
        {
            auto outerFrame = new QFrame(this);
            outerFrame->setFrameStyle(QFrame::Box | QFrame::Plain);
            outerFrame->setLineWidth(1);
            outerFrame->setObjectName("phaseFrameA_I");
            outerFrame->setStyleSheet("#phaseFrameA_I { border: 1px solid #ccc; border-radius: 5px; background-color: #eee; }");

            auto dialLayout = new QHBoxLayout(outerFrame);

            QLabel* phaseLabel = new QLabel("전류 A 위상(°)");
            phaseLabel->setFont(boldFont);

            iAPhaseDial = new QDial();
            iAPhaseDial->setRange(-180, 180);
            iAPhaseDial->setValue(dataMng.getIAPhaseDeg());
            iAPhaseDial->setNotchesVisible(true);

            iAPhaseDisp = new QLabel(QString::number(dataMng.getIAPhaseDeg()) + "°");
            iAPhaseDisp->setMinimumWidth(40);
            iAPhaseDisp->setAlignment(Qt::AlignCenter);

            dialLayout->addWidget(phaseLabel);
            dialLayout->addWidget(iAPhaseDial);
            dialLayout->addWidget(iAPhaseDisp);

            vl->addWidget(outerFrame);

            // A상 연동
            connect(iAPhaseDial, &QDial::valueChanged, this, [&, this](int deg){
                iAPhaseDisp->setText(QString::number(deg) + "°");
                dataMng.setIAPhaseDeg(deg); });
        }

        // A상 크기 연동
        connect(vASlideEdit, &slideEdit::valueChanged, this, [&](int value) {
            dataMng.setVAValue(value);
        });

        connect(iASlideEdit, &slideEdit::valueChanged, this,  [&](int value) {
            dataMng.setIAValue(value);
        });

    // ===== 상 B =====
    {
        // 전압 B : 크기
        vBSlideEdit = new slideEdit("전압 B (V)", dataMng.getVBValue(), 500.0, -500.0);
        vl->addWidget(vBSlideEdit);

        // 전압 B : 위상
        {
            auto frame = new QFrame(this);
            frame->setFrameStyle(QFrame::Box | QFrame::Plain);
            frame->setLineWidth(1);
            frame->setObjectName("phaseFrameB_V");
            frame->setStyleSheet("#phaseFrameB_V { border: 1px solid #ccc; border-radius: 5px; background-color: #eee; }");

            auto lay = new QHBoxLayout(frame);
            auto lbl = new QLabel("전압 B 위상(°)"); lbl->setFont(boldFont);
            vBPhaseDial = new QDial();
            vBPhaseDial->setRange(-180, 180);
            vBPhaseDial->setValue(dataMng.getVBPhaseDeg());
            vBPhaseDial->setNotchesVisible(true);

            vBPhaseDisp = new QLabel(QString::number(dataMng.getVBPhaseDeg())+"°");
            vBPhaseDisp->setMinimumWidth(40);
            vBPhaseDisp->setAlignment(Qt::AlignCenter);

            lay->addWidget(lbl);
            lay->addWidget(vBPhaseDial);
            lay->addWidget(vBPhaseDisp);

            vl->addWidget(frame);

            connect(vBPhaseDial, &QDial::valueChanged, this, [&, this](int deg){
                vBPhaseDisp->setText(QString::number(deg) + "°");
                dataMng.setVBPhaseDeg(deg);
            });
        }

        // 전류 B : 크기
        iBSlideEdit = new slideEdit("전류 B (A)", dataMng.getIBValue(), 20.0, -20.0);
        vl->addWidget(iBSlideEdit);

        // 전류 B : 위상
        {
            auto frame = new QFrame(this);
            frame->setFrameStyle(QFrame::Box | QFrame::Plain);
            frame->setLineWidth(1);
            frame->setObjectName("phaseFrameB_I");
            frame->setStyleSheet("#phaseFrameB_I { border: 1px solid #ccc; border-radius: 5px; background-color: #eee; }");

            auto lay = new QHBoxLayout(frame);
            auto lbl = new QLabel("전류 B 위상(°)"); lbl->setFont(boldFont);
            iBPhaseDial = new QDial();
            iBPhaseDial->setRange(-180, 180);
            iBPhaseDial->setValue(dataMng.getIBPhaseDeg());
            iBPhaseDial->setNotchesVisible(true);

            iBPhaseDisp = new QLabel(QString::number(dataMng.getIBPhaseDeg())+"°");
            iBPhaseDisp->setMinimumWidth(40);
            iBPhaseDisp->setAlignment(Qt::AlignCenter);

            lay->addWidget(lbl); lay->addWidget(iBPhaseDial); lay->addWidget(iBPhaseDisp);
            vl->addWidget(frame);

            connect(iBPhaseDial, &QDial::valueChanged, this, [&, this](int deg){
                iBPhaseDisp->setText(QString::number(deg) + "°");
                dataMng.setIBPhaseDeg(deg);
            });
        }


        //  B상 크기 연동
        connect(vBSlideEdit, &slideEdit::valueChanged, this, [&](int value) {
            dataMng.setVBValue(value);
        });

        connect(iBSlideEdit, &slideEdit::valueChanged, this,  [&](int value) {
            dataMng.setIBValue(value);
        });
    }

    // ===== 상 C =====
    {
        // 전압 C : 크기
        vCSlideEdit = new slideEdit("전압 C (V)", dataMng.getVCValue(), 500.0, -500.0);
        vl->addWidget(vCSlideEdit);

        // 전압 C : 위상
        {
            auto frame = new QFrame(this);
            frame->setFrameStyle(QFrame::Box | QFrame::Plain);
            frame->setLineWidth(1);
            frame->setObjectName("phaseFrameC_V");
            frame->setStyleSheet("#phaseFrameC_V { border: 1px solid #ccc; border-radius: 5px; background-color: #eee; }");

            auto lay = new QHBoxLayout(frame);
            auto lbl = new QLabel("전압 C 위상(°)"); lbl->setFont(boldFont);
            vCPhaseDial = new QDial();
            vCPhaseDial->setRange(-180, 180);
            vCPhaseDial->setValue(dataMng.getVCPhaseDeg());
            vCPhaseDial->setNotchesVisible(true);

            vCPhaseDisp = new QLabel(QString::number(dataMng.getVCPhaseDeg())+"°");
            vCPhaseDisp->setMinimumWidth(40);
            vCPhaseDisp->setAlignment(Qt::AlignCenter);

            lay->addWidget(lbl); lay->addWidget(vCPhaseDial); lay->addWidget(vCPhaseDisp);
            vl->addWidget(frame);

            connect(vCPhaseDial, &QDial::valueChanged, this, [&, this](int deg){
                vCPhaseDisp->setText(QString::number(deg) + "°");
                dataMng.setVCPhaseDeg(deg);
            });
        }

        // 전류 C : 크기
        iCSlideEdit = new slideEdit("전류 C (A)", dataMng.getICValue(), 20.0, -20.0);
        vl->addWidget(iCSlideEdit);

        // 전류 C : 위상
        {
            auto frame = new QFrame(this);
            frame->setFrameStyle(QFrame::Box | QFrame::Plain);
            frame->setLineWidth(1);
            frame->setObjectName("phaseFrameC_I");
            frame->setStyleSheet("#phaseFrameC_I { border: 1px solid #ccc; border-radius: 5px; background-color: #eee; }");

            auto lay = new QHBoxLayout(frame);
            auto lbl = new QLabel("전류 C 위상(°)"); lbl->setFont(boldFont);
            iCPhaseDial = new QDial();
            iCPhaseDial->setRange(-180, 180);
            iCPhaseDial->setValue(dataMng.getICPhaseDeg());
            iCPhaseDial->setNotchesVisible(true);

            iCPhaseDisp = new QLabel(QString::number(dataMng.getICPhaseDeg())+"°");
            iCPhaseDisp->setMinimumWidth(40);
            iCPhaseDisp->setAlignment(Qt::AlignCenter);

            lay->addWidget(lbl); lay->addWidget(iCPhaseDial); lay->addWidget(iCPhaseDisp);
            vl->addWidget(frame);

            connect(iCPhaseDial, &QDial::valueChanged, this, [&, this](int deg){
                iCPhaseDisp->setText(QString::number(deg) + "°");
                dataMng.setICPhaseDeg(deg);
            });
        }

        //  C상 크기 연동
        connect(vCSlideEdit, &slideEdit::valueChanged, this, [&](int value) {
            dataMng.setVCValue(value);
        });

        connect(iCSlideEdit, &slideEdit::valueChanged, this,  [&](int value) {
            dataMng.setICValue(value);
        });
    }

    vl->addStretch(1);
    }
}

//inputWidget 저장된 값을 불러올 때 새로고침?
void inputWidget::refreshFromData()
{
    // ===== A상 =====
    if (vASlideEdit) vASlideEdit->setValue(dataMng.getVAValue());
    if (iASlideEdit) iASlideEdit->setValue(dataMng.getIAValue());
    if (iAPhaseDial) iAPhaseDial->setValue(dataMng.getIAPhaseDeg());
    if (iAPhaseDisp) iAPhaseDisp->setText(QString::number(dataMng.getIAPhaseDeg()) + "°");

    // ===== B상 =====
    if (vBSlideEdit) vBSlideEdit->setValue(dataMng.getVBValue());
    if (iBSlideEdit) iBSlideEdit->setValue(dataMng.getIBValue());
    if (vBPhaseDial) vBPhaseDial->setValue(dataMng.getVBPhaseDeg());
    if (vBPhaseDisp) vBPhaseDisp->setText(QString::number(dataMng.getVBPhaseDeg()) + "°");
    if (iBPhaseDial) iBPhaseDial->setValue(dataMng.getIBPhaseDeg());
    if (iBPhaseDisp) iBPhaseDisp->setText(QString::number(dataMng.getIBPhaseDeg()) + "°");

    // ===== C상 =====
    if (vCSlideEdit) vCSlideEdit->setValue(dataMng.getVCValue());
    if (iCSlideEdit) iCSlideEdit->setValue(dataMng.getICValue());
    if (vCPhaseDial) vCPhaseDial->setValue(dataMng.getVCPhaseDeg());
    if (vCPhaseDisp) vCPhaseDisp->setText(QString::number(dataMng.getVCPhaseDeg()) + "°");
    if (iCPhaseDial) iCPhaseDial->setValue(dataMng.getICPhaseDeg());
    if (iCPhaseDisp) iCPhaseDisp->setText(QString::number(dataMng.getICPhaseDeg()) + "°");
}

