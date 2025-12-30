#include "inputwidget.h"
#include <QBoxLayout>
#include <QLabel>
#include <Qt>
#include <QDial>

inputWidget::inputWidget(dataManagement& dataMng) //생성자를 호출할 때 넘겨는주는 인자(argument), 생성자의 매개변수
    : dataMng(dataMng) // 멤버 초기화 리스트,  클래스 내부에 있는 멤버 변수 dataMng를 매개변수 dataMng로 초기화
{
    this->setMinimumSize(200, 600);
    auto vl = new QVBoxLayout(this);

    {
        auto hl = new QHBoxLayout();
        auto name = new QLabel("입력값");
        auto sign = new QLabel("V");
        auto phase = new QLabel("˚");

        auto initVal = dataMng.getInputValue();
        //dataVal = new QLineEdit(initVal);
        dataVal = new QSlider(Qt::Horizontal);
        dataVal->setValue(initVal);
        dataVal->setRange(-500,500);


        hl->addWidget(name);
        hl->addWidget(dataVal);
        hl->addWidget(sign);


        vl->addLayout(hl);

        //connect(dataVal, &QLineEdit::returnPressed, this, &inputWidget::valueChanged);
        connect(dataVal, &QSlider::valueChanged, this, &inputWidget::inputValueChanged);
    }

    {
        auto hl = new QHBoxLayout();
        auto name = new QLabel("시간간격");
        auto sign = new QLabel("s");

        auto initVal = dataMng.getTimeInterval();
        //timeInterval = new QLineEdit(init_val);
        timeInterval = new QSlider(Qt::Horizontal);
        timeInterval->setValue(initVal * 10);
        timeInterval->setRange(1, 10);

        hl->addWidget(name);
        hl->addWidget(timeInterval);
        hl->addWidget(sign);

        vl->addLayout(hl);

        //connect(timeInterval, &QLineEdit::returnPressed, this, &inputWidget::intervalChanged);
        connect(timeInterval, &QSlider::valueChanged, this, &inputWidget::intervalChanged);
    }

}

void inputWidget::inputValueChanged()
{
    float val = static_cast<float>(dataVal->value());

    qDebug() << "입력값 : " << val;
    dataMng.setInputValue(val);
}

void inputWidget::intervalChanged()
{
    float val = static_cast<float>(timeInterval->value())/10.0;

    qDebug() << "시간간격 : " << val;
    dataMng.setTimeInterval(val);
}
