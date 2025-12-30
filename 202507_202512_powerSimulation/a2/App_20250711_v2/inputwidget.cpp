#include "inputwidget.h"
#include "slideedit.h"
#include <QBoxLayout>
#include <QLabel>
#include <Qt>
#include <QDial>

inputWidget::inputWidget(dataManagement& dataMng) //생성자를 호출할 때 넘겨는주는 인자(argument), 생성자의 매개변수
    : dataMng(dataMng) // 멤버 초기화 리스트,  클래스 내부에 있는 멤버 변수 dataMng를 매개변수 dataMng로 초기화
{
    this->setMinimumSize(200, 600);
    auto vl = new QVBoxLayout(this);


    slideEdit* vSlideEdit = new slideEdit("전압값(V)", dataMng.getInputValue(), 500, -500);
    vl->addWidget(vSlideEdit);

    slideEdit* tSlideEdit = new slideEdit("시간간격(ms)", dataMng.getTimeInterval(), 1000, 100);
    vl->addWidget(tSlideEdit);


    connect(vSlideEdit, &slideEdit::valueChanged, this, &inputWidget::inputChanged);
    connect(tSlideEdit, &slideEdit::valueChanged, this, &inputWidget::intervalChanged);


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
