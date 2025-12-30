#include "a3700n_harm.h"
#include "qcustomplot.h"

#include <QHBoxLayout>
#include <QComboBox>
#include <QCheckBox>
#include <QEvent>
#include <QDir>
#include <QListView>
#include <QLabel>

a3700n_harm::a3700n_harm(dataManagement& dataMng)
    : dataMng(dataMng)
{
    setFixedSize(600, 310);
    loadQSS(":/styles/a3700n_harm_style.qss");

    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    QHBoxLayout* topbarLay = new QHBoxLayout();


    QComboBox* valueCombo = new QComboBox();
    valueCombo->addItem("Voltage");
    valueCombo->addItem("[%]RMS");
    valueCombo->addItem("[%Fund]");
    valueCombo->setFixedWidth(120);
    valueCombo->setObjectName("harmCombo");

    QComboBox* viewCombo = new QComboBox();
    viewCombo->addItem("Graph");
    viewCombo->addItem("Text");
    viewCombo->setFixedWidth(120);
    viewCombo->setObjectName("harmCombo");

    topbarLay->addWidget(valueCombo);
    topbarLay->addWidget(viewCombo);

    QCheckBox* fundCheck = new QCheckBox("Fund.");
    fundCheck->setFixedWidth(80);
    fundCheck->setChecked(true);
    fundCheck->setObjectName("customCheckBox");

    topbarLay->addStretch(2);
    topbarLay->addWidget(fundCheck);
    topbarLay->addStretch(1);

    QCheckBox* ACheck = new QCheckBox("A");
    ACheck->setFixedWidth(50);
    ACheck->setChecked(true);
    ACheck->setObjectName("ACheck");

    QCheckBox* BCheck = new QCheckBox("B");
    BCheck->setFixedWidth(50);
    BCheck->setChecked(true);
    BCheck->setObjectName("BCheck");

    QCheckBox* CCheck = new QCheckBox("C");
    CCheck->setFixedWidth(50);
    CCheck->setChecked(true);
    CCheck->setObjectName("CCheck");


    topbarLay->addWidget(ACheck);
    topbarLay->addWidget(BCheck);
    topbarLay->addWidget(CCheck);

    //-------아래부분----------------------

    QHBoxLayout* bottomLay = new QHBoxLayout();

    QWidget* outerBox = new QWidget();
    outerBox->setObjectName("outerBox");
    outerBox->setContentsMargins(2, 2, 2, 2);
    outerBox->setFixedWidth(65);
    outerBox->setFixedHeight(180);

    QVBoxLayout* outerLayout = new QVBoxLayout(outerBox);
    outerLayout->setContentsMargins(0, 0, 0, 0);
    outerLayout->setSpacing(4);

    QLabel* titleLabel = new QLabel("Scale");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setObjectName("titleLab");
    outerLayout->addWidget(titleLabel);

    QWidget* innerBox = new QWidget(outerBox);
    innerBox->setObjectName("innerBox");

    QVBoxLayout* innerLayout = new QVBoxLayout(innerBox);
    innerLayout->setSpacing(2);
    innerLayout->setContentsMargins(2, 2, 2, 2);
    innerLayout->setAlignment(Qt::AlignTop);

    autoBtn = new QPushButton("Auto");
    autoBtn->setFixedHeight(46);
    autoBtn->setCheckable(true);
    autoBtn->setChecked(true);
    autoBtn->setObjectName("autoBtn");
    innerLayout->addWidget(autoBtn);

    plusBtn = new QPushButton("+");
    plusBtn->setFixedHeight(46);
    plusBtn->setObjectName("pmBtn");
    innerLayout->addWidget(plusBtn);

    minusBtn = new QPushButton("-");
    minusBtn->setFixedHeight(46);
    minusBtn->setObjectName("pmBtn");
    innerLayout->addWidget(minusBtn);

    outerLayout->addWidget(innerBox);
    bottomLay->addWidget(outerBox);
    bottomLay->addSpacing(7);

    //-------- 고조파 그래프 -------------------

    // QWidget* harmGraphWidget = new QWidget();
    // harmGraphWidget->resize(300, 150);
    // harmGraphWidget->setStyleSheet("border: 1px solid #ccc;");

    QCustomPlot* customPlot = new QCustomPlot(this);

    QCPBars* bars = new QCPBars(customPlot->xAxis, customPlot->yAxis);
    bars->setName("전압크기");
    bars->setPen(Qt::NoPen);
    bars->setBrush(QColor(255,0,0));

    QVector<double> ticks = {1,2,3,4,5,6,7,8,9,10};
    QVector<double> values = {220,230,240};

    bars->setData(ticks, values);

    QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
    textTicker->addTick(1, "A상");
    textTicker->addTick(2, "B상");
    textTicker->addTick(3, "C상");
    customPlot->xAxis->setTicker(textTicker);

    customPlot->xAxis->setTickLabelRotation(0);
    customPlot->xAxis->setSubTicks(false);
    customPlot->xAxis->setTickLength(0, 10);
    customPlot->xAxis->setRange(0, 10);

    customPlot->yAxis->setRange(0, 300);
    customPlot->yAxis->setLabel("[V]"); // 기본 라벨 숨김

    customPlot->replot();

    bottomLay->addWidget(customPlot);

    mainLayout->addLayout(topbarLay);
    mainLayout->addLayout(bottomLay);

    setLayout(mainLayout);
}


void a3700n_harm::loadQSS(const QString& path)
{
    QFile file(path);
    if (file.open(QFile::ReadOnly)) {
        QString style = QString::fromUtf8(file.readAll());
        this->setStyleSheet(style); // 현재 파일에만 적용한다.
        file.close();
    } else {
        qWarning() << "QSS 파일을 찾을 수 없습니다:" << path;
    }
}
