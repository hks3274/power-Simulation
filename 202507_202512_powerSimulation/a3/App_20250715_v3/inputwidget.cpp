#include "inputwidget.h"

#include <QBoxLayout>
#include <Qt>



inputWidget::inputWidget(dataManagement& dataMng) //생성자를 호출할 때 넘겨는주는 인자(argument), 생성자의 매개변수
    : dataMng(dataMng) // 멤버 초기화 리스트,  클래스 내부에 있는 멤버 변수 dataMng를 매개변수 dataMng로 초기화
{
    auto vl = new QVBoxLayout(this);

    connect(&autoTimer, &QTimer::timeout, this, &inputWidget::autoTimerUpdate);


    {
        vSlideEdit = new slideEdit("전압값(V)", dataMng.getInputValue(), 500, -500);
        vl->addWidget(vSlideEdit);

        tSlideEdit = new slideEdit("시간간격(ms)", dataMng.getTimeInterval(), 1000, 100);
        vl->addWidget(tSlideEdit);

        connect(vSlideEdit, &slideEdit::valueChanged, this, &inputWidget::inputChanged);
        connect(tSlideEdit, &slideEdit::valueChanged, this, &inputWidget::intervalChanged);
    }

    {
        auto hl = new QHBoxLayout();


        //각도 지정 다이얼
        vDial = new QDial();
        vDial->setMinimum(0);
        vDial->setMaximum(360);
        vDial->setWrapping(true);
        vDial->setMinimumSize(200, 200);

        auto vlayout = new QVBoxLayout();

        //자동모드 체크박스
        aLabel = new QLabel("각도: ");

        automode = new QCheckBox();
        automode->setText("automode");


        auto hlayout = new QHBoxLayout();

        //초당 회전수
        QLabel *rps = new QLabel("RPS");

        //초당 회전수 입력
        rpsSpinBox =  new QDoubleSpinBox();
        rpsSpinBox->setRange(0.1, 2.0);
        rpsSpinBox->setDecimals(2);
        rpsSpinBox->setSingleStep(0.1);
        rpsSpinBox->setValue(dataMng.getRpsValue());

        hlayout->addWidget(rps);
        hlayout->addWidget(rpsSpinBox);

        vlayout->addWidget(aLabel);
        vlayout->addWidget(automode);
        vlayout->addLayout(hlayout);
        // 20px 만큼 공백 줄 추가
        vlayout->addSpacing(100);


        hl->addWidget(vDial);
        hl->addLayout(vlayout);

        vl->addLayout(hl);

        //angle값을 넘긴다.
        connect(vDial, &QDial::valueChanged, this, &inputWidget::inputAngleChange);

        //automode 체크시
        connect(automode, &QCheckBox::toggled, this, [=](bool checked){
            if (checked) {
                autoTimer.start(30);
                vDial->setEnabled(false);  // 자동 모드일 때 다이얼 비활성화 (수동 조작 차단)
            } else {
                autoTimer.stop();
                vDial->setEnabled(true);   // 수동 모드일 때 다이얼 활성화
            }
        });
    }


}

void inputWidget::inputChanged(float newVal)
{
    qDebug() << "입력값 : " << newVal;
    dataMng.setInputValue(newVal);
}

void inputWidget::intervalChanged(float newVal)
{
    qDebug() << "시간간격 : " << newVal;
    dataMng.setTimeInterval(newVal);
}

void inputWidget::inputAngleChange(float newVal)
{
    dataMng.setInputAngle(newVal);
    aLabel->setText("각도: " + QString::number(newVal) + "˚");
}

void inputWidget::autoTimerUpdate()
{
    // 현재 각도 읽기
    int angle = vDial->value();
    double rps = rpsSpinBox->value();

    // 각도 증가량 계산
    double deltaAngle = 360.0 * rps * (autoTimer.interval() / 1000.0);
    angle = static_cast<int>(angle + deltaAngle) % 360;

    // 시그널 차단 후 값 세팅 (valueChanged 신호 발생 X)
    vDial->blockSignals(true);
    vDial->setValue(angle);
    vDial->blockSignals(false);

    // 직접 데이터 및 UI 업데이트
    dataMng.setInputAngle(angle);
    aLabel->setText("각도: " + QString::number(angle) + "˚");
}
