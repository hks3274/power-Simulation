#include "a3700n_harm.h"
#include "qcustomplot.h"

#include <QHBoxLayout>
#include <QComboBox>
#include <QCheckBox>
#include <QButtonGroup>
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


    valueCombo = new QComboBox();
    valueCombo->addItem("Voltage");
    valueCombo->addItem("[%]RMS");
    valueCombo->addItem("[%]Fund");
    valueCombo->setFixedWidth(120);
    valueCombo->setObjectName("harmCombo");

    viewCombo = new QComboBox();
    viewCombo->addItem("Graph");
    viewCombo->addItem("Text");
    viewCombo->setFixedWidth(120);
    viewCombo->setObjectName("harmCombo");

    topbarLay->addWidget(valueCombo);
    topbarLay->addWidget(viewCombo);

    fundCheck = new QCheckBox("Fund.");
    fundCheck->setFixedWidth(80);
    fundCheck->setChecked(true);
    fundCheck->setObjectName("customCheckBox");

    topbarLay->addStretch(2);
    topbarLay->addWidget(fundCheck);
    topbarLay->addStretch(1);

    abcGroup = new QButtonGroup();
    abcGroup->setExclusive(false);

    ACheck = new QCheckBox("A");
    ACheck->setFixedWidth(50);
    ACheck->setChecked(true);
    ACheck->setObjectName("ACheck");
    abcGroup->addButton(ACheck);

    BCheck = new QCheckBox("B");
    BCheck->setFixedWidth(50);
    BCheck->setChecked(true);
    BCheck->setObjectName("BCheck");
    abcGroup->addButton(BCheck);

    CCheck = new QCheckBox("C");
    CCheck->setFixedWidth(50);
    CCheck->setChecked(true);
    CCheck->setObjectName("CCheck");
    abcGroup->addButton(CCheck);



    topbarLay->addWidget(ACheck);
    topbarLay->addWidget(BCheck);
    topbarLay->addWidget(CCheck);

    connect(ACheck, &QCheckBox::toggled, this, [this](bool checked){
        if(checked){
            bars1->setVisible(true);
        }else {
            bars1->setVisible(false);
        }
        if (lastOsl)
            updateHarmData(lastOsl);
    });

    connect(BCheck, &QCheckBox::toggled, this, [this](bool checked){
        if(checked){
            bars2->setVisible(true);
        }else {
            bars2->setVisible(false);
        }
        if (lastOsl)
            updateHarmData(lastOsl);
    });


    connect(CCheck, &QCheckBox::toggled, this, [this](bool checked){
        if(checked){
            bars3->setVisible(true);
        }else {
            bars3->setVisible(false);
        }
        if (lastOsl)
            updateHarmData(lastOsl);
    });

    connect(valueCombo, &QComboBox::currentIndexChanged, this, [this](){
        if (lastOsl)
            updateHarmData(lastOsl);
    });

    connect(fundCheck, &QCheckBox::toggled, this, [this](bool checked){
        if (lastOsl)
            updateHarmData(lastOsl);
    });



    //-------아래부분----------------------

    QStackedWidget* stack = new QStackedWidget();

    QWidget* graphStrak = new QWidget();

    QHBoxLayout* bottomLay = new QHBoxLayout(graphStrak);

    QWidget* outerBox = new QWidget();
    outerBox->setObjectName("outerBox");
    outerBox->setContentsMargins(2, 2, 2, 2);
    outerBox->setFixedWidth(65);
    outerBox->setFixedHeight(190);

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
    autoBtn->setFixedHeight(50);
    autoBtn->setCheckable(true);
    autoBtn->setChecked(true);
    autoBtn->setObjectName("autoBtn");
    innerLayout->addWidget(autoBtn);

    plusBtn = new QPushButton("+");
    plusBtn->setFixedHeight(50);
    plusBtn->setObjectName("pmBtn");
    innerLayout->addWidget(plusBtn);

    minusBtn = new QPushButton("-");
    minusBtn->setFixedHeight(50);
    minusBtn->setObjectName("pmBtn");
    innerLayout->addWidget(minusBtn);

    outerLayout->addWidget(innerBox);
    bottomLay->addWidget(outerBox);
    bottomLay->addSpacing(7);


    connect(autoBtn, &QPushButton::clicked, this, [this](){
        if (lastOsl)
            updateHarmData(lastOsl);
    });

    connect(plusBtn, &QPushButton::clicked, this, [this]() {
        autoBtn->setChecked(false);
        plusCheck = true;
        if (lastOsl)
            updateHarmData(lastOsl);
        plusCheck = false;

    });

    connect(minusBtn, &QPushButton::clicked, this, [this]() {
        autoBtn->setChecked(false);
        minusCheck = true;
        if (lastOsl)
            updateHarmData(lastOsl);
        minusCheck = false;
    });


    //-------- 고조파 그래프 -------------------

    QVBoxLayout* harmGraphLay = new QVBoxLayout();

    QHBoxLayout* infoLay = new QHBoxLayout();

    QWidget* infoWidget = new QWidget(this);
    infoWidget->setFixedHeight(40);
    QGridLayout* grid = new QGridLayout(infoWidget);
    grid->setContentsMargins(0,0,0,0);

    QLabel* THD = new QLabel("THD");
    THD->setObjectName("title");
    grid->addWidget(THD, 0,0, Qt::AlignLeft | Qt::AlignVCenter);

    QLabel* A = new QLabel("A");
    A->setObjectName("customA");
    grid->addWidget(A, 0,1, Qt::AlignCenter);

    QLabel* B = new QLabel("B");
    B->setObjectName("customB");
    grid->addWidget(B, 0,4, Qt::AlignCenter);

    QLabel* C = new QLabel("C");
    C->setObjectName("customC");
    grid->addWidget(C, 0,7, Qt::AlignCenter);

    thdA = new QLabel("4.8");
    thdA->setObjectName("values");
    grid->addWidget(thdA, 0,2, Qt::AlignRight | Qt::AlignVCenter);

    thdB = new QLabel("4.8");
    thdB->setObjectName("values");
    grid->addWidget(thdB, 0,5, Qt::AlignRight | Qt::AlignVCenter);

    thdC = new QLabel("4.8");
    thdC->setObjectName("values");
    grid->addWidget(thdC, 0,8, Qt::AlignRight | Qt::AlignVCenter);

    QLabel* percentA = new QLabel("%");
    percentA->setObjectName("unit");
    grid->addWidget(percentA,0,3,Qt::AlignCenter);
    QLabel* percentB = new QLabel("%");
    percentB->setObjectName("unit");
    grid->addWidget(percentB,0,6,Qt::AlignCenter);
    QLabel* percentC = new QLabel("%");
    percentC->setObjectName("unit");
    grid->addWidget(percentC,0,9,Qt::AlignCenter);

    QLabel* Fund = new QLabel("Fund.");
    Fund->setObjectName("title");
    grid->addWidget(Fund, 1,0, Qt::AlignLeft | Qt::AlignVCenter);

    QLabel* fA = new QLabel("A");
    fA->setObjectName("customA");
    grid->addWidget(fA, 1,1, Qt::AlignCenter);

    QLabel* fB = new QLabel("B");
    fB->setObjectName("customB");
    grid->addWidget(fB, 1,4, Qt::AlignCenter);

    QLabel* fC = new QLabel("C");
    fC->setObjectName("customC");
    grid->addWidget(fC, 1,7, Qt::AlignCenter);

    fundA = new QLabel("4.8");
    fundA->setObjectName("values");
    grid->addWidget(fundA, 1,2, Qt::AlignRight | Qt::AlignVCenter);

    fundB = new QLabel("4.8");
    fundB->setObjectName("values");
    grid->addWidget(fundB, 1,5, Qt::AlignRight | Qt::AlignVCenter);

    fundC = new QLabel("4.8");
    fundC->setObjectName("values");
    grid->addWidget(fundC, 1,8, Qt::AlignRight | Qt::AlignVCenter);

    unitA = new QLabel("V");
    unitA->setObjectName("unit");
    grid->addWidget(unitA,1,3,Qt::AlignCenter);
    unitB = new QLabel("V");
    unitB->setObjectName("unit");
    grid->addWidget(unitB,1,6,Qt::AlignCenter);
    unitC = new QLabel("V");
    unitC->setObjectName("unit");
    grid->addWidget(unitC,1,9,Qt::AlignCenter);

    grid->setColumnMinimumWidth(0, 60);  // THD / Fund
    grid->setColumnMinimumWidth(1, 40);  // A
    grid->setColumnMinimumWidth(2, 45);  // A 값
    grid->setColumnMinimumWidth(3, 20);  // %
    grid->setColumnMinimumWidth(4, 40);  // B
    grid->setColumnMinimumWidth(5, 45);  // B 값
    grid->setColumnMinimumWidth(6, 20);  // %
    grid->setColumnMinimumWidth(7, 40);  // C
    grid->setColumnMinimumWidth(8, 45);  // C 값
    grid->setColumnMinimumWidth(9, 20);  // %
    grid->setHorizontalSpacing(2);       // 셀 간 간격 줄이기

    infoLay->addStretch();
    infoLay->addWidget(infoWidget);
    harmGraphLay->addLayout(infoLay);

    customPlot = new QCustomPlot(this);

    QCPBarsGroup *group1 = new QCPBarsGroup(customPlot);
    group1->setSpacing(0);

    bars1 = new QCPBars(customPlot->xAxis, customPlot->yAxis);
    bars1->setBrush(QColor(255, 0, 0));
    bars1->setWidth(0.1);
    bars1->setBarsGroup(group1);
    bars2 = new QCPBars(customPlot->xAxis, customPlot->yAxis);
    bars2->setBrush(QColor(0, 0, 255));
    bars2->setWidth(0.1);
    bars2->setBarsGroup(group1);
    bars3 = new QCPBars(customPlot->xAxis, customPlot->yAxis);
    bars3->setBrush(QColor(60, 200, 60));
    bars3->setWidth(0.1);
    bars3->setBarsGroup(group1);

    bars1->setPen(Qt::NoPen); //바깥 테두리선 없애기
    bars2->setPen(Qt::NoPen);
    bars3->setPen(Qt::NoPen);

    customPlot->xAxis->setRange(0, 10);
    customPlot->yAxis->setRange(0, 0.7);
    customPlot->xAxis->grid()->setVisible(false); //세로 격자선 없애기
    customPlot->yAxis->grid()->setPen(QPen(QColor(200, 200, 200), 1, Qt::SolidLine)); //가로 격자선을 실선으로


    customPlot->replot();

    harmGraphLay->addWidget(customPlot);
    bottomLay->addLayout(harmGraphLay);


    infoTable = new QTableWidget(stack);
    infoTable->setContentsMargins(2,10,2,2);
    infoTable->setSelectionMode(QAbstractItemView::NoSelection);

    QVector<double> items = {0,0,0,0,0,0,0,0,0,0};

    updateTable(10, items); //임의의 값을 넣는다.

    stack->addWidget(graphStrak);
    stack->addWidget(infoTable);

    mainLayout->addLayout(topbarLay);
    mainLayout->addWidget(stack);

    connect(viewCombo, &QComboBox::currentIndexChanged, this, [this, stack](int index) {
        stack->setCurrentIndex(index);
        if(index == 1) {
            fundCheck->hide();
            ACheck->setChecked(false);
            BCheck->setChecked(false);
            CCheck->setChecked(false);
            abcGroup->setExclusive(true);
            ACheck->setChecked(true);
        }
        else {
            fundCheck->show();
            abcGroup->setExclusive(false);
        }
    });


    setLayout(mainLayout);

}


void a3700n_harm::updateTable(int totalItems, QVector<double> itemValues ){


    if(itemValues.empty()) return;

    infoTable->clearContents();
    infoTable->setRowCount(0);
    infoTable->setColumnCount(0);


    int itemsPerColumn = 10;
    int numCols = (totalItems + itemsPerColumn - 1) / itemsPerColumn;
    int numRows = itemsPerColumn;

    infoTable->setRowCount(numRows);
    infoTable->setColumnCount(numCols * 2);

    infoTable->horizontalHeader()->setVisible(false);
    infoTable->verticalHeader()->setVisible(false);
    infoTable->setEditTriggers(QAbstractItemView::NoEditTriggers);


    for (int i = 0; i < totalItems; ++i) {
        int colGroup = i / itemsPerColumn; // 몇 번째 그룹(열)
        int row = i % itemsPerColumn;
        int colNum = colGroup * 2;         // 번호 열
        int colData = colGroup * 2 + 1;    // 데이터 열

        // 번호 셀
        infoTable->setItem(row, colNum, new QTableWidgetItem(QString::number(i)));
        infoTable->item(row, colNum)->setTextAlignment(Qt::AlignCenter);

        // 데이터 셀
        infoTable->setItem(row, colData, new QTableWidgetItem(QString::number(itemValues[i], 'f', 2)));
        infoTable->item(row, colData)->setTextAlignment(Qt::AlignCenter);
    }

    for (int i = 0; i < infoTable->rowCount() ; ++i) {
        infoTable->setRowHeight(i, 15);
    }


    // 번호 열만 폭 좁게
    for (int i = 0; i < infoTable->columnCount(); i += 2){
        infoTable->setColumnWidth(i, 15);  // 번호 열 폭 (좁게)
        for (int j = 0; j < infoTable->rowCount(); ++j) {
            QTableWidgetItem* item = infoTable->item(j, i);
            if (item) {
                item->setBackground(QColor("#9c9c9c"));  // 회색 배경
                item->setForeground(QColor("#555"));     // 어두운 글씨
                item->setFont(QFont("맑은 고딕", 10, QFont::ExtraBold));
            }
        }
    }
    // 데이터 열은 자동 늘리기
    for (int i = 1; i < infoTable->columnCount(); i += 2)
        infoTable->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Stretch);
}




//title 옆에 있는 voltage 버튼을 눌렀을 경우.
void a3700n_harm::setVoltCheck(bool checked) {
    valueCombo->setItemText(0, "Voltage");

    voltageCheck = true;
    currentCheck = false;

    changeCheck = true;
    if (lastOsl)
        updateHarmData(lastOsl);
    changeCheck = false;
}

void a3700n_harm::setCurrCheck(bool checked) {
    valueCombo->setItemText(0, "Current");

    voltageCheck = false;
    currentCheck = true;

    changeCheck = true;
    if (lastOsl)
        updateHarmData(lastOsl);
    changeCheck = false;


}


void a3700n_harm::updateHarmData(oneSecList* osl) {
    if (!osl) return;

    //최신 데이터 저장
    lastOsl = osl;

    QVector<double> itemsValues;
    itemsValues.clear();

    if (voltageCheck) {
        thdA->setText(QString::number(osl->voltage.THD[0], 'f', 1));
        thdB->setText(QString::number(osl->voltage.THD[1], 'f', 1));
        thdC->setText(QString::number(osl->voltage.THD[2], 'f', 1));

        fundA->setText(QString::number(osl->voltage.Fundamental[0], 'f', 1));
        fundB->setText(QString::number(osl->voltage.Fundamental[1], 'f', 1));
        fundC->setText(QString::number(osl->voltage.Fundamental[2], 'f', 1));

        unitA->setText("V");
        unitB->setText("V");
        unitC->setText("V");

        int order = dataMng.getVHarmonicOrder();
        int maxOrder = std::max(order, 10);
        if(order >= 10) maxOrder += 1;
        double maxValue = std::numeric_limits<double>::lowest();

        datax.clear();
        datay1.clear();
        datay2.clear();
        datay3.clear();

        if(valueCombo->currentText() == "Voltage") {
            for (int i = 0; i <= maxOrder; i++) {
                datax.append(i);
                if (i == 1 && fundCheck->isChecked()) {
                    datay1.append(osl->voltage.RMS[0]);
                    datay2.append(osl->voltage.RMS[1]);
                    datay3.append(osl->voltage.RMS[2]);

                    maxValue= std::max({osl->voltage.RMS[0], osl->voltage.RMS[1], osl->voltage.RMS[2]});
                }
                else if (i == order) {
                    datay1.append(osl->harmonic.harmV);
                    datay2.append(osl->harmonic.harmV);
                    datay3.append(osl->harmonic.harmV);
                    maxValue = std::max(maxValue,osl->harmonic.harmV);
                }
                else {
                    // 빈 구간은 표시되지 않도록 NaN 사용 (QCustomPlot이 자동으로 skip)
                    datay1.append(qQNaN());
                    datay2.append(qQNaN());
                    datay3.append(qQNaN());
                }

                if (i == 1) {
                    // A, B, C 선택에 따라 값 넣기
                    if (ACheck->isChecked()) itemsValues.append(osl->voltage.RMS[0]);
                    else if (BCheck->isChecked()) itemsValues.append(osl->voltage.RMS[1]);
                    else if (CCheck->isChecked()) itemsValues.append(osl->voltage.RMS[2]);
                }
                else if (i == order) {
                    itemsValues.append(osl->harmonic.harmV);
                }
                else {
                    itemsValues.append(0);
                }

            }

        }
        else if(valueCombo->currentText() == "[%]RMS"){
            for (int i = 0; i <= maxOrder; i++) {
                datax.append(i);

                double ARms = osl->voltage.Fundamental[0] / osl->voltage.RMS[0] * 100;
                double BRms = osl->voltage.Fundamental[1] / osl->voltage.RMS[1] * 100;
                double CRms = osl->voltage.Fundamental[2] / osl->voltage.RMS[2] * 100;

                double AHarmRms = osl->harmonic.harmV / osl->voltage.RMS[0] * 100;
                double BHarmRms = osl->harmonic.harmV / osl->voltage.RMS[1] * 100;
                double CHarmRms = osl->harmonic.harmV / osl->voltage.RMS[2] * 100;

                if (i == 1 && fundCheck->isChecked()) {
                    datay1.append(ARms);
                    datay2.append(BRms);
                    datay3.append(CRms);

                    maxValue= std::max({ARms, BRms, CRms});
                }
                else if (i == order) {
                    datay1.append(AHarmRms);
                    datay2.append(BHarmRms);
                    datay3.append(CHarmRms);
                    maxValue = std::max({maxValue,AHarmRms, BHarmRms, CHarmRms});
                }
                else {
                    // 빈 구간은 표시되지 않도록 NaN 사용 (QCustomPlot이 자동으로 skip)
                    datay1.append(qQNaN());
                    datay2.append(qQNaN());
                    datay3.append(qQNaN());
                }

                if (i == 1) {
                    // A, B, C 선택에 따라 값 넣기
                    if (ACheck->isChecked()) itemsValues.append(ARms);
                    else if (BCheck->isChecked()) itemsValues.append(BRms);
                    else if (CCheck->isChecked()) itemsValues.append(CRms);
                }
                else if (i == order) {
                    if (ACheck->isChecked()) itemsValues.append(AHarmRms);
                    else if (BCheck->isChecked()) itemsValues.append(BHarmRms);
                    else if (CCheck->isChecked()) itemsValues.append(CHarmRms);
                }
                else {
                    itemsValues.append(0);
                }
            }
        }
        else if(valueCombo->currentText() == "[%]Fund"){
            for (int i = 0; i <= maxOrder; i++) {
                datax.append(i);

                double AFund = 100;
                double BFund = 100;
                double CFund = 100;

                double AHarmFund = osl->harmonic.harmV / osl->voltage.Fundamental[0] * 100;
                double BHarmFund = osl->harmonic.harmV / osl->voltage.Fundamental[1] * 100;
                double CHarmFund = osl->harmonic.harmV / osl->voltage.Fundamental[2] * 100;

                if (i == 1 && fundCheck->isChecked()) {
                    datay1.append(AFund);
                    datay2.append(BFund);
                    datay3.append(CFund);

                    maxValue= std::max({AFund, BFund, CFund});
                }
                else if (i == order) {
                    datay1.append(AHarmFund);
                    datay2.append(BHarmFund);
                    datay3.append(CHarmFund);
                    maxValue = std::max({maxValue,AHarmFund, BHarmFund, CHarmFund});
                }
                else {
                    // 빈 구간은 표시되지 않도록 NaN 사용 (QCustomPlot이 자동으로 skip)
                    datay1.append(qQNaN());
                    datay2.append(qQNaN());
                    datay3.append(qQNaN());
                }

                if (i == 1) {
                    // A, B, C 선택에 따라 값 넣기
                    if (ACheck->isChecked()) itemsValues.append(AFund);
                    else if (BCheck->isChecked()) itemsValues.append(BFund);
                    else if (CCheck->isChecked()) itemsValues.append(CFund);
                }
                else if (i == order) {
                    if (ACheck->isChecked()) itemsValues.append(AHarmFund);
                    else if (BCheck->isChecked()) itemsValues.append(BHarmFund);
                    else if (CCheck->isChecked()) itemsValues.append(CHarmFund);
                }
                else {
                    itemsValues.append(0);
                }

            }
        }

        updateTable(itemsValues.size(), itemsValues);

        bars1->setData(datax, datay1);
        bars2->setData(datax, datay2);
        bars3->setData(datax, datay3);

        customPlot->xAxis->setRange(0, maxOrder);

        if(autoBtn->isChecked() || changeCheck){
            if      (maxValue < 4)   newRangeV = 4;
            else if (maxValue < 8)   newRangeV = 8;
            else if (maxValue < 20)  newRangeV = 20;
            else if (maxValue < 40)  newRangeV = 40;
            else if (maxValue < 80)  newRangeV = 80;
            else if (maxValue < 200) newRangeV = 200;
            else if (maxValue < 400) newRangeV = 400;
            else                     newRangeV = 800;

        } else {
            if(plusCheck) {
                if(newRangeV < 800){
                    if(newRangeV == 8) newRangeV = 20;
                    else if(newRangeV == 80) newRangeV = 200;
                    else newRangeV *= 2;
                } else newRangeV = 800;
            }
            if(minusCheck) {
                if(newRangeV > 1){
                    if(newRangeV == 20) newRangeV = 8;
                    else if(newRangeV == 200) newRangeV = 80;
                    else newRangeV /= 2;
                } else newRangeV = 1;
            }
        }

        customPlot->yAxis->setRange(0, newRangeV);

        maxValue = std::numeric_limits<double>::lowest();

        customPlot->replot();
    }
    else if (currentCheck) {
        thdA->setText(QString::number(osl->current.THD[0], 'f', 1));
        thdB->setText(QString::number(osl->current.THD[1], 'f', 1));
        thdC->setText(QString::number(osl->current.THD[2], 'f', 1));

        fundA->setText(QString::number(osl->current.Fundamental[0], 'f', 3));
        fundB->setText(QString::number(osl->current.Fundamental[1], 'f', 3));
        fundC->setText(QString::number(osl->current.Fundamental[2], 'f', 3));

        unitA->setText("A");
        unitB->setText("A");
        unitC->setText("A");

        int order = dataMng.getCHarmonicOrder();
        int maxOrder = std::max(order, 10);
        if(order >= 10) maxOrder += 1;
        double maxValue = std::numeric_limits<double>::lowest();

        datax.clear();
        datay1.clear();
        datay2.clear();
        datay3.clear();

        if(valueCombo->currentText() == "Current") {
            for (int i = 0; i <= maxOrder; i++) {
                datax.append(i);
                if (i == 1 && fundCheck->isChecked()) {
                    datay1.append(osl->current.RMS[0]);
                    datay2.append(osl->current.RMS[1]);
                    datay3.append(osl->current.RMS[2]);

                    maxValue= std::max({osl->current.RMS[0], osl->current.RMS[1], osl->current.RMS[2]});
                }
                else if (i == order) {
                    datay1.append(osl->harmonic.harmA);
                    datay2.append(osl->harmonic.harmA);
                    datay3.append(osl->harmonic.harmA);
                    maxValue = std::max(maxValue,osl->harmonic.harmA);
                }
                else {
                    // 빈 구간은 표시되지 않도록 NaN 사용 (QCustomPlot이 자동으로 skip)
                    datay1.append(qQNaN());
                    datay2.append(qQNaN());
                    datay3.append(qQNaN());
                }

                if (i == 1) {
                    // A, B, C 선택에 따라 값 넣기
                    if (ACheck->isChecked()) itemsValues.append(osl->current.RMS[0]);
                    else if (BCheck->isChecked()) itemsValues.append(osl->current.RMS[1]);
                    else if (CCheck->isChecked()) itemsValues.append(osl->current.RMS[2]);
                }
                else if (i == order) {
                    itemsValues.append(osl->harmonic.harmA);
                }
                else {
                    itemsValues.append(0);
                }
            }

        }
        else if(valueCombo->currentText() == "[%]RMS"){
            for (int i = 0; i <= maxOrder; i++) {
                datax.append(i);

                double ARms = osl->current.Fundamental[0] / osl->current.RMS[0] * 100;
                double BRms = osl->current.Fundamental[1] / osl->current.RMS[1] * 100;
                double CRms = osl->current.Fundamental[2] / osl->current.RMS[2] * 100;

                double AHarmRms = osl->harmonic.harmA / osl->current.RMS[0] * 100;
                double BHarmRms = osl->harmonic.harmA / osl->current.RMS[1] * 100;
                double CHarmRms = osl->harmonic.harmA / osl->current.RMS[2] * 100;

                if (i == 1 && fundCheck->isChecked()) {
                    datay1.append(ARms);
                    datay2.append(BRms);
                    datay3.append(CRms);

                    maxValue= std::max({ARms, BRms, CRms});
                }
                else if (i == order) {
                    datay1.append(AHarmRms);
                    datay2.append(BHarmRms);
                    datay3.append(CHarmRms);
                    maxValue = std::max({maxValue,AHarmRms, BHarmRms, CHarmRms});
                }
                else {
                    // 빈 구간은 표시되지 않도록 NaN 사용 (QCustomPlot이 자동으로 skip)
                    datay1.append(qQNaN());
                    datay2.append(qQNaN());
                    datay3.append(qQNaN());
                }


                if (i == 1) {
                    // A, B, C 선택에 따라 값 넣기
                    if (ACheck->isChecked()) itemsValues.append(ARms);
                    else if (BCheck->isChecked()) itemsValues.append(BRms);
                    else if (CCheck->isChecked()) itemsValues.append(CRms);
                }
                else if (i == order) {
                    if (ACheck->isChecked()) itemsValues.append(AHarmRms);
                    else if (BCheck->isChecked()) itemsValues.append(BHarmRms);
                    else if (CCheck->isChecked()) itemsValues.append(CHarmRms);
                }
                else {
                    itemsValues.append(0);
                }
            }
        }
        else if(valueCombo->currentText() == "[%]Fund"){
            for (int i = 0; i <= maxOrder; i++) {
                datax.append(i);

                double AFund = 100;
                double BFund = 100;
                double CFund = 100;

                double AHarmFund = osl->harmonic.harmA / osl->current.Fundamental[0] * 100;
                double BHarmFund = osl->harmonic.harmA / osl->current.Fundamental[1] * 100;
                double CHarmFund = osl->harmonic.harmA / osl->current.Fundamental[2] * 100;

                if (i == 1 && fundCheck->isChecked()) {
                    datay1.append(AFund);
                    datay2.append(BFund);
                    datay3.append(CFund);

                    maxValue= std::max({AFund, BFund, CFund});
                }
                else if (i == order) {
                    datay1.append(AHarmFund);
                    datay2.append(BHarmFund);
                    datay3.append(CHarmFund);
                    maxValue = std::max({maxValue,AHarmFund, BHarmFund, CHarmFund});
                }
                else {
                    // 빈 구간은 표시되지 않도록 NaN 사용 (QCustomPlot이 자동으로 skip)
                    datay1.append(qQNaN());
                    datay2.append(qQNaN());
                    datay3.append(qQNaN());
                }

                if (i == 1) {
                    // A, B, C 선택에 따라 값 넣기
                    if (ACheck->isChecked()) itemsValues.append(AFund);
                    else if (BCheck->isChecked()) itemsValues.append(BFund);
                    else if (CCheck->isChecked()) itemsValues.append(CFund);
                }
                else if (i == order) {
                    if (ACheck->isChecked()) itemsValues.append(AHarmFund);
                    else if (BCheck->isChecked()) itemsValues.append(BHarmFund);
                    else if (CCheck->isChecked()) itemsValues.append(CHarmFund);
                }
                else {
                    itemsValues.append(0);
                }
            }
        }

        bars1->setData(datax, datay1);
        bars2->setData(datax, datay2);
        bars3->setData(datax, datay3);

        customPlot->xAxis->setRange(0, maxOrder);

        updateTable(itemsValues.size(), itemsValues);


        if(autoBtn->isChecked() || changeCheck){
            if      (maxValue < 4)   newRangeC = 4;
            else if (maxValue < 8)   newRangeC = 8;
            else if (maxValue < 20)  newRangeC = 20;
            else if (maxValue < 40)  newRangeC = 40;
            else if (maxValue < 80)  newRangeC = 80;
            else if (maxValue < 200) newRangeC = 200;
            else if (maxValue < 400) newRangeC = 400;
            else                     newRangeC = 800;

        } else {
            if(plusCheck) {
                if(newRangeC < 800){
                    if(newRangeC == 8) newRangeC = 20;
                    else if(newRangeC == 80) newRangeC = 200;
                    else newRangeC *= 2;
                } else newRangeC = 800;
            }
            if(minusCheck) {
                if(newRangeC > 1){
                    if(newRangeC == 20) newRangeC = 8;
                    else if(newRangeC == 200) newRangeC = 80;
                    else newRangeC /= 2;
                } else newRangeC = 1;
            }
        }

        customPlot->yAxis->setRange(0, newRangeC);

        maxValue = std::numeric_limits<double>::lowest();

        customPlot->replot();
    }
    else {
        qWarning() << "오류 : 고조파 데이터 수집 오류";
    }


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
