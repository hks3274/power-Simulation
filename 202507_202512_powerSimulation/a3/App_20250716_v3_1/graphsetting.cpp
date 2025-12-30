#include "graphsetting.h"
#include <QBoxLayout>
#include <QLabel>
#include <QPushButton>

graphSetting::graphSetting(dataManagement& dataMng)
    :dataMng(dataMng)
{
    setWindowTitle("Graph Settings");
    resize(400, 300);
    QVBoxLayout *layout = new QVBoxLayout(this);

    {
        QHBoxLayout *hlayout = new QHBoxLayout(this);

        QLabel *intervalLabel = new QLabel();
        intervalLabel->setText("시간 간격");  // setText()로 텍스트 지정

        // 스핀박스 생성
        intervalSpinBox = new QDoubleSpinBox();
        intervalSpinBox->setMinimum(0.01);
        intervalSpinBox->setMaximum(1.0);
        intervalSpinBox->setSingleStep(0.01);
        intervalSpinBox->setValue(dataMng.getTimeInterval());

        QLabel *sLabel = new QLabel("s");

        hlayout->addWidget(intervalLabel);
        hlayout->addWidget(intervalSpinBox);
        hlayout->addWidget(sLabel);

        layout-> addLayout(hlayout);
    }


    {
        QHBoxLayout *hlayout = new QHBoxLayout(this);

        QLabel *graphWidthLabel = new QLabel();
        graphWidthLabel->setText("그래프 폭 ");  // setText()로 텍스트 지정

        // 스핀박스 생성
        widthSpinBox = new QSpinBox();
        widthSpinBox->setMinimum(1);
        widthSpinBox->setMaximum(100);
        widthSpinBox->setValue(dataMng.getGraphWidth());

        QLabel *sLabel = new QLabel("s");

        hlayout->addWidget(graphWidthLabel);
        hlayout->addWidget(widthSpinBox);
        hlayout->addWidget(sLabel);

        layout-> addLayout(hlayout);
    }

    {

        QHBoxLayout *hlayout = new QHBoxLayout(this);

        QPushButton *settingBtn = new QPushButton("설정하기");
        connect(settingBtn, &QPushButton::clicked, this, &graphSetting::graphSettingChange);

        //닫기 버튼
        QPushButton *closeBtn = new QPushButton("닫기");
        connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);

        hlayout->addWidget(settingBtn);
        hlayout->addWidget(closeBtn);

        layout->addLayout(hlayout);
    }
}


//설정을 눌렀을 때
void graphSetting::graphSettingChange() {
    //그래프폭 값 변경
    int gWidth = widthSpinBox->value();
    dataMng.setGraphWidth(gWidth);


    //시간 간격 값 변경
    float interval_msec = intervalSpinBox->value();
    dataMng.setTimeInterval(interval_msec);
}

