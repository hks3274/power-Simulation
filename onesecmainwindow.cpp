#include "onesecmainwindow.h"
#include <QVBoxLayout>
#include <QHeaderView>
#include <QFont>
#include <QLabel>
#include <QDebug>
#include <cmath>
#include <QScrollArea>

#include "mainwindow.h"
#include "inputmainwindow.h"
#include "a3700n_gui.h"
#include "oneseccalcdata.h"


oneSecMainWindow::oneSecMainWindow(dataManagement& dataMng, QWidget* parent)
    : QMainWindow(parent)
    , dataMng(dataMng)
{
    setWindowTitle("1초 데이터 창");

    // 너비 고정, 높이는 기본값 1000으로 설정하되 최대 제한 없음
    resize(450, 1000);
    setMinimumSize(450, 600);   // 최소 크기 (줄일 때)
    setMaximumHeight(QWIDGETSIZE_MAX); // 높이는 무제한 늘릴 수 있음

    QMenuBar* menubar = menuBar();

    QMenu* windowMenu = menubar->addMenu("창열기");
    // 액션 생성
    QAction* openMainWin   = new QAction("그래프창 열기", this);
    QAction* openInputWin  = new QAction("입력창 열기", this);
    QAction* openA3700Win  = new QAction("A3700창 열기", this);

    // 메뉴에 추가
    windowMenu->addAction(openMainWin);
    windowMenu->addAction(openInputWin);
    windowMenu->addAction(openA3700Win);


    connect(openInputWin, &QAction::triggered, [this]() {
        if (inputWindow) {
            inputWindow->show();
            inputWindow->raise();
            inputWindow->activateWindow();
        }
    });

    connect(openMainWin, &QAction::triggered, [this]() {
        if (mainWindow) {
            mainWindow->show();
            mainWindow->raise();
            mainWindow->activateWindow();
        }
    });

    connect(openA3700Win, &QAction::triggered, [this]() {
        if (a3700Window) {
            a3700Window->show();
            a3700Window->raise();
            a3700Window->activateWindow();
        }
    });



    // 스크롤 가능 영역 추가
    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true); // 내용에 맞게 자동 리사이즈
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    QWidget* central = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(central);

    auto makeTitle = [&](const QString& text) {
        QLabel* l = new QLabel(text, this);
        l->setFont(QFont("맑은 고딕", 11, QFont::Bold));
        l->setStyleSheet("background-color:#e6e6e6; padding:3px; border:1px solid #ccc;");
        return l;
    };

    auto makeTable = [&](int cols, const QStringList& headers) {
        auto* t = new QTableWidget(this);
        t->setColumnCount(cols);
        t->setHorizontalHeaderLabels(headers);

        // 0번째 열은 고정 너비, 나머지는 Stretch
        t->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
        t->setColumnWidth(0, 120);  // 모든 테이블의 0번째 열 너비 통일

        for (int c = 1; c < cols; ++c) {
            t->horizontalHeader()->setSectionResizeMode(c, QHeaderView::Stretch);
        }

        t->verticalHeader()->setVisible(false);
        t->setEditTriggers(QAbstractItemView::NoEditTriggers);
        t->setSelectionMode(QAbstractItemView::NoSelection);
        t->setStyleSheet(
            "QTableWidget { gridline-color:#ccc; font-family:'맑은 고딕'; font-size:10.5pt; font-weight:500; }"
            "QHeaderView::section { background-color:#f7f7f7; padding:4px; border:1px solid #ccc; font-size:10.5pt; font-weight:500; }"
            );

        //각 테이블의 높이를 column 크기에 맞춘다.
        QTimer::singleShot(0, this, [=]() {
            QHeaderView* vheader = t->verticalHeader();
            int totalHeight = 0;
            for (int r = 0; r < t->rowCount(); ++r)
                totalHeight += vheader->sectionSizeHint(r);
            int headerH = t->horizontalHeader()->height();
            t->setFixedHeight(totalHeight + headerH + 6);
        });
        t->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        return t;
    };

    // ───── 기본파 ─────
    layout->addWidget(makeTitle("기본파"));
    tableFund = makeTable(4, {"항목", "A상", "B상", "C상"});
    tableFund->setRowCount(4);
    layout->addWidget(tableFund);

    // ───── 고조파 ─────
    layout->addWidget(makeTitle("고조파"));
    tableHarm = makeTable(3, {"항목", "전압(V)", "전류(A)"});
    tableHarm->setRowCount(2);
    layout->addWidget(tableHarm);

    // ───── 합성파 ─────
    layout->addWidget(makeTitle("합성파"));
    tableTotal = makeTable(4, {"항목", "A상", "B상", "C상"});
    tableTotal->setRowCount(2);
    layout->addWidget(tableTotal);

    // ───── 전력 정보 ─────
    layout->addWidget(makeTitle("전력 정보"));
    tablePower = makeTable(5, {"항목", "A상", "B상", "C상", "합계"});
    tablePower->setRowCount(5);
    layout->addWidget(tablePower);

    // ───── THD ─────
    layout->addWidget(makeTitle("THD(고조파 왜곡률)"));
    tableThd = makeTable(4, {"항목", "A상", "B상", "C상"});
    tableThd->setRowCount(2);
    layout->addWidget(tableThd);

    // ───── Residual ─────
    layout->addWidget(makeTitle("Residual(잔류)"));
    tableResidual = makeTable(3, {"항목", "전압(V)", "전류(A)"});
    tableResidual->setRowCount(1);
    layout->addWidget(tableResidual);

    // ───── symmentic Component ─────
    layout->addWidget(makeTitle("Symmetric Component"));
    tableSymmentic = makeTable(4, {"항목", "Positive\nSequence", "Negative\nSequence", "Zero\nSequence"});
    tableSymmentic->setRowCount(2);
    layout->addWidget(tableSymmentic);

    // ───── symmentic Component ─────
    layout->addWidget(makeTitle("NEMA Unbalance"));
    tableNemaUnbal = makeTable(4, {"항목", "3상 RMS의\n평균값(A)", "3상 RMS중\n최대값(B)", "Unbal\n(B–A)/A"});
    tableNemaUnbal->setRowCount(2);
    layout->addWidget(tableNemaUnbal);

    // ───── symmentic Component ─────
    layout->addWidget(makeTitle("U0, U2 Unbalance"));
    tableUo_U2 = makeTable(3, {"항목", "U0\n(Zero/Positive)", "U2\n(Negative / Positive)"});
    tableUo_U2->setRowCount(2);
    layout->addWidget(tableUo_U2);


    layout->addStretch();
    central->setLayout(layout);

    //central 위젯을 scrollArea 안에 넣기
    scrollArea->setWidget(central);

    oneSecCalcData* osCalcData = new oneSecCalcData(dataMng);

    //scrollArea를 메인윈도우 중앙 위젯으로 설정
    setCentralWidget(scrollArea);
    connect(osCalcData, &oneSecCalcData::oneSecTableChanged, this, &oneSecMainWindow::onRmsData);

}

void oneSecMainWindow::onRmsData(oneSecTableData* tableData, oneSecList* osl)
{


    // === 기본파 ===
    QStringList fundNames = {"전압 RMS (V)", "전류 RMS (A)", "전압 위상 (°)", "전류 위상 (°)"};
    double fundVals[4][3] = {
        {tableData->vFundA, tableData->vFundB, tableData->vFundC},
        {tableData->iFundA, tableData->iFundB, tableData->iFundC},
        {tableData->vPhA,   tableData->vPhB,   tableData->vPhC},
        {tableData->iPhA,   tableData->iPhB,   tableData->iPhC}
    };
    for (int r = 0; r < 4; ++r) {
        tableFund->setItem(r, 0, new QTableWidgetItem(fundNames[r]));
        for (int c = 0; c < 3; ++c) {
            QString unit = (r < 2 ? (r == 0 ? " V" : " A") : "°");
            tableFund->setItem(r, c + 1,
                               new QTableWidgetItem(QString::number(fundVals[r][c], 'f', (r < 2 ? 2 : 1 ))+ unit));
        }
    }

    // === 고조파 ===
    tableHarm->setItem(0, 0, new QTableWidgetItem("RMS"));
    tableHarm->setItem(0, 1, new QTableWidgetItem(QString::number(tableData->vHarm, 'f', 2) + " V"));
    tableHarm->setItem(0, 2, new QTableWidgetItem(QString::number(tableData->iHarm, 'f', 2) + " A"));
    tableHarm->setItem(1, 0, new QTableWidgetItem("위상 (°)"));
    tableHarm->setItem(1, 1, new QTableWidgetItem(QString::number(tableData->vHph, 'f', 1) + "°"));
    tableHarm->setItem(1, 2, new QTableWidgetItem(QString::number(tableData->iHph, 'f', 1) + "°"));


    // === 합성파 ===
    QStringList totalNames = {"전압 RMS (V)", "전류 RMS (A)"};
    double totalVals[2][3] = {
        {tableData->vTotalA, tableData->vTotalB, tableData->vTotalC},
        {tableData->iTotalA, tableData->iTotalB, tableData->iTotalC}
    };
    for (int r = 0; r < 2; ++r) {
        tableTotal->setItem(r, 0, new QTableWidgetItem(totalNames[r]));
        for (int c = 0; c < 3; ++c) {
            QString unit = (r == 0 ? " V" : " A");
            tableTotal->setItem(r, c + 1, new QTableWidgetItem(QString::number(totalVals[r][c], 'f', 2) + unit));
        }
    }


    QString powerNames[5] = {"유효전력 (W)", "피상전력 (VA)", "무효전력 (VAR)", "역률 (PF)", "전력량 (Wh)"};
    double powerVals[5][4] = {
        {tableData->pA, tableData->pB, tableData->pC, tableData->pTotal},
        {tableData->sA, tableData->sB, tableData->sC, tableData->sSum},
        {tableData->qA, tableData->qB, tableData->qC, tableData->qSum},
        {tableData->pfA, tableData->pfB, tableData->pfC, tableData-> pfSum},
        {0, 0, 0, tableData->energyWh}
    };


    for (int r = 0; r < 5; ++r) {
        tablePower->setItem(r, 0, new QTableWidgetItem(powerNames[r]));
        for (int c = 0; c < 4; ++c) {
            QString val = QString::number(powerVals[r][c], 'f', (r == 3 ? 3 : 2));
            if (r == 4 && c < 3) val = "-";
            tablePower->setItem(r, c + 1, new QTableWidgetItem(val));
        }
    }


    QStringList thdNames = {"전압 THD (%)", "전류 THD (%)"};
    double thdVals[2][3] = {
        {tableData->thdVA, tableData->thdVB, tableData->thdVC},
        {tableData->thdIA, tableData->thdIB, tableData->thdIC}
    };

    for (int r = 0; r < 2; ++r) {
        tableThd->setItem(r, 0, new QTableWidgetItem(thdNames[r]));
        for (int c = 0; c < 3; ++c) {
            QString val = std::isinf(thdVals[r][c]) ? "∞" : QString::number(thdVals[r][c], 'f', 2);
            tableThd->setItem(r, c + 1, new QTableWidgetItem(val + " %"));
        }
    }


    //==========Reaidual==================

    // Residual 테이블 갱신
    tableResidual->setItem(0, 0, new QTableWidgetItem("Residual 전압 (V)"));
    tableResidual->setItem(0, 1, new QTableWidgetItem(QString::number(tableData->vResidualRMS, 'f', 3) + " V"));
    tableResidual->setItem(0, 2, new QTableWidgetItem(QString::number(tableData->iResidualRMS, 'f', 3) + " A"));


    //==========simmentic component==================


    // --- 전압 성분 ---
    tableSymmentic->setItem(0, 0, new QTableWidgetItem("전압 성분 (V)"));
    tableSymmentic->setItem(0, 1, new QTableWidgetItem(QString::number(tableData->Vpos_mag, 'f', 3) + " V"));
    tableSymmentic->setItem(0, 2, new QTableWidgetItem(QString::number(tableData->Vneg_mag, 'f', 3) + " V"));
    tableSymmentic->setItem(0, 3, new QTableWidgetItem(QString::number(tableData->Vzero_mag, 'f', 3) + " V"));

    tableSymmentic->setItem(1, 0, new QTableWidgetItem("전류 성분 (A)"));
    tableSymmentic->setItem(1, 1, new QTableWidgetItem(QString::number(tableData->Ipos_mag, 'f', 3) + " A"));
    tableSymmentic->setItem(1, 2, new QTableWidgetItem(QString::number(tableData->Ineg_mag, 'f', 3) + " A"));
    tableSymmentic->setItem(1, 3, new QTableWidgetItem(QString::number(tableData->Izero_mag, 'f', 3) + " A"));

    //============ NEMA Unbalance ============

    tableNemaUnbal->setItem(0, 0, new QTableWidgetItem("기본파 전압(V)"));
    tableNemaUnbal->setItem(0, 1, new QTableWidgetItem(QString::number(tableData->vA, 'f', 2)+ " V"));
    tableNemaUnbal->setItem(0, 2, new QTableWidgetItem(QString::number(tableData->vB, 'f', 2)+ " A"));
    tableNemaUnbal->setItem(0, 3, new QTableWidgetItem(QString::number(tableData->vUnbalance, 'f', 2) + " %"));

    tableNemaUnbal->setItem(1, 0, new QTableWidgetItem("기본파 전류(A)"));
    tableNemaUnbal->setItem(1, 1, new QTableWidgetItem(QString::number(tableData->iA, 'f', 2)+ " V"));
    tableNemaUnbal->setItem(1, 2, new QTableWidgetItem(QString::number(tableData->iB, 'f', 2)+ " A"));
    tableNemaUnbal->setItem(1, 3, new QTableWidgetItem(QString::number(tableData->iUnbalance, 'f', 2) + " %"));



    //=========== U0 , U2 ====================

    // 표시: 무한대면 "∞", 아니면 수치 출력
    auto fmtVal = [](double val) {
        if (std::isinf(val))
            return QString("∞");
        else
            return QString::number(val * 100.0, 'f', 2) + " %"; // % 단위로 표시
    };

    tableUo_U2->setItem(0, 0, new QTableWidgetItem("전압"));
    tableUo_U2->setItem(0, 1, new QTableWidgetItem(fmtVal(tableData->vU0)));
    tableUo_U2->setItem(0, 2, new QTableWidgetItem(fmtVal(tableData->vU2)));

    tableUo_U2->setItem(1, 0, new QTableWidgetItem("전류"));
    tableUo_U2->setItem(1, 1, new QTableWidgetItem(fmtVal(tableData->iU0)));
    tableUo_U2->setItem(1, 2, new QTableWidgetItem(fmtVal(tableData->iU2)));


    emit oneSecChanged(osl);
}


void oneSecMainWindow::setMainWindow(MainWindow* mainWin){
    mainWindow = mainWin;
}

void oneSecMainWindow::setInputWindow(inputMainWindow* inputWin){
    inputWindow = inputWin;
}

void oneSecMainWindow::setA3700Window(A3700N_GUI* a3700Win){
    a3700Window = a3700Win;
}

oneSecMainWindow::~oneSecMainWindow() = default;
