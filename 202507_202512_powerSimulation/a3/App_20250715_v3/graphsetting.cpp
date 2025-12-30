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

        QLabel *graphWidthLabel = new QLabel();
        graphWidthLabel->setText("그래프 폭(ms)");  // setText()로 텍스트 지정

        // 스핀박스 생성
        widthSpinBox = new QSpinBox();
        widthSpinBox->setMinimum(1000);
        widthSpinBox->setMaximum(10000);
        widthSpinBox->setValue(dataMng.getGraphWidth());

        hlayout->addWidget(graphWidthLabel);
        hlayout->addWidget(widthSpinBox);

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

void graphSetting::graphSettingChange() {
    int gWidth = widthSpinBox->value();
    dataMng.setGraphWidth(gWidth);
}

