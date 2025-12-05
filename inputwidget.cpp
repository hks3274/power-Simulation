#include "inputwidget.h"

#include <QBoxLayout>

inputWidget::inputWidget(dataManagement& dataMng) //생성자를 호출할 때 넘겨는주는 인자(argument), 생성자의 매개변수
    : dataMng(dataMng) // 멤버 초기화 리스트,  클래스 내부에 있는 멤버 변수 dataMng를 매개변수 dataMng로 초기화
{
    auto vl = new QVBoxLayout(this);
    vl->setContentsMargins(8, 8, 8, 8);
    vl->setSpacing(8);

    // ===== 상 A =====
    {
        // 전압 A : 크기만
        vASlideEdit = new slideEdit("전압 A (V)", dataMng.getVAValue(), 500.0, -500.0);
        vl->addWidget(vASlideEdit);

        // 전류 A : 크기
        iASlideEdit = new slideEdit("전류 A (A)", dataMng.getIAValue(), 20.0, -20.0);
        vl->addWidget(iASlideEdit);

        iADialEdit = new dialEdit("전류 A 위상(°)", dataMng.getIAPhaseDeg());
        vl->addWidget(iADialEdit);


        // A상 크기 연동
        connect(vASlideEdit, &slideEdit::valueChanged, this, [&](int value) {
            dataMng.setVAValue(value);
        });

        connect(iASlideEdit, &slideEdit::valueChanged, this,  [&](int value) {
            dataMng.setIAValue(value);
        });

        connect(iADialEdit, &dialEdit::valueChanged, this, [&](int value) {

            qDebug() << "IA 위상 : " << value;
            dataMng.setIAPhaseDeg(value);
        });

    // ===== 상 B =====
    {
        // 전압 B : 크기
        vBSlideEdit = new slideEdit("전압 B (V)", dataMng.getVBValue(), 500.0, -500.0);
        vl->addWidget(vBSlideEdit);

        // 전압 B : 위상
        vBDialEdit = new dialEdit("전압 B 위상(°)", dataMng.getVBPhaseDeg());
        vl->addWidget(vBDialEdit);

        // 전류 B : 크기
        iBSlideEdit = new slideEdit("전류 B (A)", dataMng.getIBValue(), 20.0, -20.0);
        vl->addWidget(iBSlideEdit);

        // 전류 B : 위상
        iBDialEdit = new dialEdit("전류 B 위상(°)", dataMng.getIBPhaseDeg());
        vl->addWidget(iBDialEdit);

        //  B상 크기 연동
        connect(vBSlideEdit, &slideEdit::valueChanged, this, [&](int value) {
            dataMng.setVBValue(value);
        });

        connect(iBSlideEdit, &slideEdit::valueChanged, this,  [&](int value) {
            dataMng.setIBValue(value);
        });

        connect(vBDialEdit, &dialEdit::valueChanged, this,  [&](int value) {
            dataMng.setVBPhaseDeg(value);
        });

        connect(iBDialEdit, &dialEdit::valueChanged, this,  [&](int value) {
            dataMng.setIBPhaseDeg(value);
        });
    }

    // ===== 상 C =====
    {
        // 전압 C : 크기
        vCSlideEdit = new slideEdit("전압 C (V)", dataMng.getVCValue(), 500.0, -500.0);
        vl->addWidget(vCSlideEdit);

        // 전압 C :위상
        vCDialEdit = new dialEdit("전압 C 위상(°)", dataMng.getVCPhaseDeg());
        vl->addWidget(vCDialEdit);


        // 전류 C : 크기
        iCSlideEdit = new slideEdit("전류 C (A)", dataMng.getICValue(), 20.0, -20.0);
        vl->addWidget(iCSlideEdit);

        // 전류 C : 위상
        iCDialEdit = new dialEdit("전류 C 위상(°)", dataMng.getICPhaseDeg());
        vl->addWidget(iCDialEdit);


        //  C상 크기 연동
        connect(vCSlideEdit, &slideEdit::valueChanged, this, [&](int value) {
            dataMng.setVCValue(value);
        });

        connect(iCSlideEdit, &slideEdit::valueChanged, this,  [&](int value) {
            dataMng.setICValue(value);
        });

        connect(vCDialEdit, &dialEdit::valueChanged, this,  [&](int value) {
            dataMng.setVCPhaseDeg(value);
        });

        connect(iCDialEdit, &dialEdit::valueChanged, this,  [&](int value) {
            dataMng.setICPhaseDeg(value);
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
    if (iADialEdit)  iADialEdit->setDialValue(dataMng.getIAPhaseDeg());

    // ===== B상 =====
    if (vBSlideEdit) vBSlideEdit->setValue(dataMng.getVBValue());
    if (iBSlideEdit) iBSlideEdit->setValue(dataMng.getIBValue());
    if (vBDialEdit) vBDialEdit->setDialValue(dataMng.getVBPhaseDeg());
    if (iBDialEdit) iBDialEdit->setDialValue(dataMng.getIBPhaseDeg());

    // ===== C상 =====
    if (vCSlideEdit) vCSlideEdit->setValue(dataMng.getVCValue());
    if (iCSlideEdit) iCSlideEdit->setValue(dataMng.getICValue());
    if (vCDialEdit) vCDialEdit->setDialValue(dataMng.getVCPhaseDeg());
    if (iCDialEdit) iCDialEdit->setDialValue(dataMng.getICPhaseDeg());
}
