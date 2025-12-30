#include "graphsetting.h"
#include <QBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QGroupBox>

graphSetting::graphSetting(dataManagement& dataMng,QWidget* parent)
    :QDialog(parent), dataMng(dataMng)
{
    setWindowTitle("Graph Settings");
    resize(400, 300);
    QVBoxLayout *layout = new QVBoxLayout(this);

    {
        QHBoxLayout *hlayout = new QHBoxLayout();

        QLabel *graphWidthLabel = new QLabel();
        graphWidthLabel->setText("그래프 폭");  // setText()로 텍스트 지정

        // 스핀박스 생성
        widthSpinBox = new QDoubleSpinBox();
        widthSpinBox->setMinimum(0.01);
        widthSpinBox->setMaximum(10.0);
        widthSpinBox->setValue(dataMng.getGraphWidth());

        hlayout->addWidget(graphWidthLabel);
        hlayout->addWidget(widthSpinBox);

        layout-> addLayout(hlayout);
    }

    {
        QGroupBox* modeGroupBox = new QGroupBox("화면 갱신 모드");
        modeGroupBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        QHBoxLayout* groupBoxLayout = new QHBoxLayout(modeGroupBox);

        updateModeGroup = new QButtonGroup(this);
        oneSampleBtn = new QRadioButton("1 Sample");
        halfCycleBtn = new QRadioButton("Half Cycle");
        fullCycleBtn = new QRadioButton("1 Cycle");

        updateModeGroup->addButton(oneSampleBtn, 0);
        updateModeGroup->addButton(halfCycleBtn, 1);
        updateModeGroup->addButton(fullCycleBtn, 2);

        groupBoxLayout->addWidget(oneSampleBtn);
        groupBoxLayout->addWidget(halfCycleBtn);
        groupBoxLayout->addWidget(fullCycleBtn);

        // 현재 설정 반영
        switch (dataMng.getUpdateMode()) {
        case updateMode::OneSample:
            oneSampleBtn->setChecked(true);
            break;
        case updateMode::HalfCycle:
            halfCycleBtn->setChecked(true);
            break;
        case updateMode::FullCycle:
            fullCycleBtn->setChecked(true);
            break;
        }

        // 기존 layout에 추가
        layout->addWidget(modeGroupBox);
    }


    {
        QHBoxLayout *hlayout = new QHBoxLayout();

        QPushButton *settingBtn = new QPushButton("설정하기"); //다이얼로그에서 QPushButton 중 첫 번째 버튼이 명시적으로 setDefault(true)가 없어도 Qt는 내부적으로 그걸 default로 간주한다.(그래서 닫기버튼은 먼저 위치시키면 닫기버튼이 눌릴 수 있는 가능성이 있음)
        settingBtn->setDefault(true); //따라서, 다음과 같이 default로 지정을 해줘야 나중에 코드를 봐도 명확하게 알 수 있다.
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

    double gWidth = widthSpinBox->value();
    dataMng.setGraphWidth(gWidth);

    int index = updateModeGroup->checkedId();

    switch (index) {
    case 0:
        dataMng.setUpdateMode(updateMode::OneSample);
        break;
    case 1:
        dataMng.setUpdateMode(updateMode::HalfCycle);
        break;
    case 2:
        dataMng.setUpdateMode(updateMode::FullCycle);
        break;
    }

    accept();// 입력값을 변경했다면 설정 창을 닫아라.
}


void graphSetting::refreshFromData()
{

    qDebug() << "[REFRESH] 들어옴";
    qDebug() << "현재 updateMode = " << static_cast<int>(dataMng.getUpdateMode());
    widthSpinBox->setValue(dataMng.getGraphWidth());

    // 현재 모드에 따라 체크
    switch (dataMng.getUpdateMode()) {
    case updateMode::OneSample:
        oneSampleBtn->setChecked(true);
        qDebug() << "OneSample 버튼 상태:" << oneSampleBtn->isChecked();
        break;
    case updateMode::HalfCycle:
        halfCycleBtn->setChecked(true);
        qDebug() << "HalfCycle 버튼 상태:" << halfCycleBtn->isChecked();
        break;
    case updateMode::FullCycle:
        fullCycleBtn->setChecked(true);
        qDebug() << "FullCycle 버튼 상태:" << fullCycleBtn->isChecked();
        break;
    }
}


