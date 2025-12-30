#include "onesecmainwindow.h"
#include <QVBoxLayout>
#include <QHeaderView>
#include <QFont>
#include <QLabel>
#include <QDebug>
#include <cmath>
#include <QScrollArea>
#include <complex>

#include "mainwindow.h"
#include "inputmainwindow.h"

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

    // 메뉴에 추가
    windowMenu->addAction(openMainWin);
    windowMenu->addAction(openInputWin);


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

    //scrollArea를 메인윈도우 중앙 위젯으로 설정
    setCentralWidget(scrollArea);
    connect(&dataMng, &dataManagement::rmsDataChanged, this, &oneSecMainWindow::onRmsData);

}

void oneSecMainWindow::onRmsData(dataManagement::measure_data m)
{
    auto elapsed = m.t - prev_sumTime;
    oneSecSampleCnt++;

    oneSecData = new dataManagement::oneSecList();

    // 누적
    sumFundV2_A += m.vFundA * m.vFundA;
    sumFundV2_B += m.vFundB * m.vFundB;
    sumFundV2_C += m.vFundC * m.vFundC;
    sumFundI2_A += m.iFundA * m.iFundA;
    sumFundI2_B += m.iFundB * m.iFundB;
    sumFundI2_C += m.iFundC * m.iFundC;

    sumHarmV2 += m.vHarmA * m.vHarmA;
    sumHarmI2 += m.iHarmA * m.iHarmA;

    sumTotalV2_A += m.vRmsA * m.vRmsA;
    sumTotalV2_B += m.vRmsB * m.vRmsB;
    sumTotalV2_C += m.vRmsC * m.vRmsC;
    sumTotalI2_A += m.iRmsA * m.iRmsA;
    sumTotalI2_B += m.iRmsB * m.iRmsB;
    sumTotalI2_C += m.iRmsC * m.iRmsC;

    sumPowerA += m.pA;
    sumPowerB += m.pB;
    sumPowerC += m.pC;


    if (elapsed >= std::chrono::milliseconds(995)) {
        // === RMS 계산 ===
        double vFundA = std::sqrt(sumFundV2_A / oneSecSampleCnt);
        double vFundB = std::sqrt(sumFundV2_B / oneSecSampleCnt);
        double vFundC = std::sqrt(sumFundV2_C / oneSecSampleCnt);
        double iFundA = std::sqrt(sumFundI2_A / oneSecSampleCnt);
        double iFundB = std::sqrt(sumFundI2_B / oneSecSampleCnt);
        double iFundC = std::sqrt(sumFundI2_C / oneSecSampleCnt);

        double vHarm = std::sqrt(sumHarmV2 / oneSecSampleCnt);
        double iHarm = std::sqrt(sumHarmI2 / oneSecSampleCnt);

        double vTotalA = std::sqrt(sumTotalV2_A / oneSecSampleCnt);
        double vTotalB = std::sqrt(sumTotalV2_B / oneSecSampleCnt);
        double vTotalC = std::sqrt(sumTotalV2_C / oneSecSampleCnt);
        double iTotalA = std::sqrt(sumTotalI2_A / oneSecSampleCnt);
        double iTotalB = std::sqrt(sumTotalI2_B / oneSecSampleCnt);
        double iTotalC = std::sqrt(sumTotalI2_C / oneSecSampleCnt);

        // === 위상 ===
        lastPhasor = dataMng.calcPhasor();
        double vPhA = lastPhasor.vAPhaseDeg;
        double vPhB = lastPhasor.vBPhaseDeg;
        double vPhC = lastPhasor.vCPhaseDeg;
        double iPhA = lastPhasor.cAPhaseDeg;
        double iPhB = lastPhasor.cBPhaseDeg;
        double iPhC = lastPhasor.cCPhaseDeg;
        double vHph = lastPhasor.hvPhaseDeg;
        double iHph = lastPhasor.hcPhaseDeg;

        double dt_sec = std::chrono::duration_cast<std::chrono::duration<double>>(elapsed).count();
        double pA = sumPowerA / oneSecSampleCnt;
        double pB = sumPowerB / oneSecSampleCnt;
        double pC = sumPowerC / oneSecSampleCnt;
        double pTotal = pA + pB + pC;
        sumEnergy += pTotal * dt_sec;
        double energyWh = sumEnergy / 3600.0;

        // === 기본파 ===
        QStringList fundNames = {"전압 RMS (V)", "전류 RMS (A)", "전압 위상 (°)", "전류 위상 (°)"};
        double fundVals[4][3] = {
            {vFundA, vFundB, vFundC},
            {iFundA, iFundB, iFundC},
            {vPhA,   vPhB,   vPhC},
            {iPhA,   iPhB,   iPhC}
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
        tableHarm->setItem(0, 1, new QTableWidgetItem(QString::number(vHarm, 'f', 2) + " V"));
        tableHarm->setItem(0, 2, new QTableWidgetItem(QString::number(iHarm, 'f', 2) + " A"));
        tableHarm->setItem(1, 0, new QTableWidgetItem("위상 (°)"));
        tableHarm->setItem(1, 1, new QTableWidgetItem(QString::number(vHph, 'f', 1) + "°"));
        tableHarm->setItem(1, 2, new QTableWidgetItem(QString::number(iHph, 'f', 1) + "°"));


        // === 합성파 ===
        QStringList totalNames = {"전압 RMS (V)", "전류 RMS (A)"};
        double totalVals[2][3] = {
            {vTotalA, vTotalB, vTotalC},
            {iTotalA, iTotalB, iTotalC}
        };
        for (int r = 0; r < 2; ++r) {
            tableTotal->setItem(r, 0, new QTableWidgetItem(totalNames[r]));
            for (int c = 0; c < 3; ++c) {
                QString unit = (r == 0 ? " V" : " A");
                tableTotal->setItem(r, c + 1, new QTableWidgetItem(QString::number(totalVals[r][c], 'f', 2) + unit));
            }
        }



        // === 전력 정보 ===
        double sA = vTotalA * iTotalA;
        double sB = vTotalB * iTotalB;
        double sC = vTotalC * iTotalC;
        double sSum = sA + sB + sC;

        double qA = std::sqrt(std::max(0.0, sA*sA - pA*pA));
        double qB = std::sqrt(std::max(0.0, sB*sB - pB*pB));
        double qC = std::sqrt(std::max(0.0, sC*sC - pC*pC));
        double qSum = qA + qB + qC;

        double pfA = (sA > 1e-9)? pA/sA : 0.0;
        double pfB = (sB > 1e-9)? pB/sB : 0.0;
        double pfC = (sC > 1e-9)? pC/sC : 0.0;
        double pfSum = (sSum > 1e-9)? pTotal/sSum : 0.0;

        QString powerNames[5] = {"유효전력 (W)", "피상전력 (VA)", "무효전력 (VAR)", "역률 (PF)", "전력량 (Wh)"};
        double powerVals[5][4] = {
            {pA, pB, pC, pTotal},
            {sA, sB, sC, sSum},
            {qA, qB, qC, qSum},
            {pfA, pfB, pfC, pfSum},
            {0, 0, 0, energyWh}
        };

        for (int r = 0; r < 5; ++r) {
            tablePower->setItem(r, 0, new QTableWidgetItem(powerNames[r]));
            for (int c = 0; c < 4; ++c) {
                QString val = QString::number(powerVals[r][c], 'f', (r == 3 ? 3 : 2));
                if (r == 4 && c < 3) val = "-";
                tablePower->setItem(r, c + 1, new QTableWidgetItem(val));
            }
        }

        // === THD 계산 ===
        auto safeTHD = [](double harm, double fund) {
            if (fund < 1e-9)        // 기본파가 사실상 0이면
                return std::numeric_limits<double>::infinity();  // ∞ (무한대)
            else
                return (harm / fund * 100.0);
        };

        double thdVA = safeTHD(vHarm, vFundA);
        double thdVB = safeTHD(vHarm, vFundB);
        double thdVC = safeTHD(vHarm, vFundC);
        double thdIA = safeTHD(iHarm, iFundA);
        double thdIB = safeTHD(iHarm, iFundB);
        double thdIC = safeTHD(iHarm, iFundC);

        QStringList thdNames = {"전압 THD (%)", "전류 THD (%)"};
        double thdVals[2][3] = {
            {thdVA, thdVB, thdVC},
            {thdIA, thdIB, thdIC}
        };

        for (int r = 0; r < 2; ++r) {
            tableThd->setItem(r, 0, new QTableWidgetItem(thdNames[r]));
            for (int c = 0; c < 3; ++c) {
                QString val = std::isinf(thdVals[r][c]) ? "∞" : QString::number(thdVals[r][c], 'f', 2);
                tableThd->setItem(r, c + 1, new QTableWidgetItem(val + " %"));
            }
        }


        //==========Reaidual==================

        // === Residual RMS 계산 ===
        double vResidualRMS = std::sqrt(m.residualSumV / oneSecSampleCnt);
        double iResidualRMS = std::sqrt(m.residualSumI / oneSecSampleCnt);

        // Residual 테이블 갱신
        tableResidual->setItem(0, 0, new QTableWidgetItem("Residual 전압 (V)"));
        tableResidual->setItem(0, 1, new QTableWidgetItem(QString::number(vResidualRMS, 'f', 3) + " V"));
        tableResidual->setItem(0, 2, new QTableWidgetItem(QString::number(iResidualRMS, 'f', 3) + " A"));


        //==========simmentic component==================
        std::complex<double> a(-0.5, std::sqrt(3)/2.0); //120
        std::complex<double> a2 = std::pow(a, 2); //-120

        auto deg2rad = [](double d){ return d * M_PI / 180.0; }; //라디안으로 변환
        lastPhasor = dataMng.calcPhasor();
        // --- 전압 phasor ---
        std::complex<double> VA = std::polar(lastPhasor.vAMag, deg2rad(lastPhasor.vAPhaseDeg));
        std::complex<double> VB = std::polar(lastPhasor.vBMag, deg2rad(lastPhasor.vBPhaseDeg));
        std::complex<double> VC = std::polar(lastPhasor.vCMag, deg2rad(lastPhasor.vCPhaseDeg));

        // --- 전류 phasor ---
        std::complex<double> IA = std::polar(lastPhasor.cAMag, deg2rad(lastPhasor.cAPhaseDeg));
        std::complex<double> IB = std::polar(lastPhasor.cBMag, deg2rad(lastPhasor.cBPhaseDeg));
        std::complex<double> IC = std::polar(lastPhasor.cCMag, deg2rad(lastPhasor.cCPhaseDeg));

        // --- 대칭분 계산 (전압) ---
        std::complex<double> Vpos  = (VA + a * VB + a2 * VC) / 3.0;
        std::complex<double> Vneg  = (VA + a2 * VB + a * VC) / 3.0;
        std::complex<double> Vzero = (VA + VB + VC) / 3.0;

        // --- 대칭분 계산 (전류) ---
        std::complex<double> Ipos  = (IA + a * IB + a2 * IC) / 3.0;
        std::complex<double> Ineg  = (IA + a2 * IB + a * IC) / 3.0;
        std::complex<double> Izero = (IA + IB + IC) / 3.0;

        // --- 크기 계산 ---
        double Vpos_mag = std::abs(Vpos);
        double Vneg_mag = std::abs(Vneg);
        double Vzero_mag = std::abs(Vzero);

        double Ipos_mag = std::abs(Ipos);
        double Ineg_mag = std::abs(Ineg);
        double Izero_mag = std::abs(Izero);


        // --- 전압 성분 ---
        tableSymmentic->setItem(0, 0, new QTableWidgetItem("전압 성분 (V)"));
        tableSymmentic->setItem(0, 1, new QTableWidgetItem(QString::number(Vpos_mag, 'f', 3) + " V"));
        tableSymmentic->setItem(0, 2, new QTableWidgetItem(QString::number(Vneg_mag, 'f', 3) + " V"));
        tableSymmentic->setItem(0, 3, new QTableWidgetItem(QString::number(Vzero_mag, 'f', 3) + " V"));

        tableSymmentic->setItem(1, 0, new QTableWidgetItem("전류 성분 (A)"));
        tableSymmentic->setItem(1, 1, new QTableWidgetItem(QString::number(Ipos_mag, 'f', 3) + " A"));
        tableSymmentic->setItem(1, 2, new QTableWidgetItem(QString::number(Ineg_mag, 'f', 3) + " A"));
        tableSymmentic->setItem(1, 3, new QTableWidgetItem(QString::number(Izero_mag, 'f', 3) + " A"));



        //============ NEMA Unbalance =============
        //A = 3상 RMS의 평균값
        //B = 3상 RMS 중 최대값
        //unbalance = ((B - A) / A) * 100.0
        //기본파 기준

        std::vector<double> v = { vFundA, vFundB, vFundC };
        std::vector<double> i = { iFundA, iFundB, iFundC };

        double vA = std::accumulate(v.begin(), v.end(), 0.0) / v.size(); // 평균값
        double vB = *std::max_element(v.begin(), v.end());               // 최대값

        double vUnbalance = ((vB - vA) / vA) * 100.0; // %

        double iA = std::accumulate(i.begin(), i.end(), 0.0) / i.size(); // 평균값
        double iB = *std::max_element(i.begin(), i.end());               // 최대값

        double iUnbalance = ((iB - iA) / iA) * 100.0; // %

        tableNemaUnbal->setItem(0, 0, new QTableWidgetItem("기본파 전압(V)"));
        tableNemaUnbal->setItem(0, 1, new QTableWidgetItem(QString::number(vA, 'f', 2)+ " V"));
        tableNemaUnbal->setItem(0, 2, new QTableWidgetItem(QString::number(vB, 'f', 2)+ " A"));
        tableNemaUnbal->setItem(0, 3, new QTableWidgetItem(QString::number(vUnbalance, 'f', 2) + " %"));

        tableNemaUnbal->setItem(1, 0, new QTableWidgetItem("기본파 전류(A)"));
        tableNemaUnbal->setItem(1, 1, new QTableWidgetItem(QString::number(iA, 'f', 2)+ " V"));
        tableNemaUnbal->setItem(1, 2, new QTableWidgetItem(QString::number(iB, 'f', 2)+ " A"));
        tableNemaUnbal->setItem(1, 3, new QTableWidgetItem(QString::number(iUnbalance, 'f', 2) + " %"));


        //=========== U0 , U2 ====================
        //U0 = Zero / Positive
        //U2 = negative / Positive

        double vU0 = 0.0;
        double vU2 = 0.0;


        if (Vpos_mag > 1e-4) {
            // 정상 계산
            vU0 = Vzero_mag / Vpos_mag;
            vU2 = Vneg_mag / Vpos_mag;
        } else {
            // 분모가 0 → 무한대로 설정
            vU0 = std::numeric_limits<double>::infinity();
            vU2 = std::numeric_limits<double>::infinity();
        }

        // 표시: 무한대면 "∞", 아니면 수치 출력
        auto fmtVal = [](double val) {
            if (std::isinf(val))
                return QString("∞");
            else
                return QString::number(val * 100.0, 'f', 2) + " %"; // % 단위로 표시
        };


        tableUo_U2->setItem(0, 0, new QTableWidgetItem("전압"));
        tableUo_U2->setItem(0, 1, new QTableWidgetItem(fmtVal(vU0)));
        tableUo_U2->setItem(0, 2, new QTableWidgetItem(fmtVal(vU2)));

        double iU0 = 0.0;
        double iU2 = 0.0;

        if (Ipos_mag > 1e-4) {
            // 정상 계산
            iU0 = Izero_mag / Ipos_mag;
            iU2 = Ineg_mag / Ipos_mag;
        } else {
            // 분모가 0 → 무한대로 설정
            iU0 = std::numeric_limits<double>::infinity();
            iU2 = std::numeric_limits<double>::infinity();
        }

        tableUo_U2->setItem(1, 0, new QTableWidgetItem("전류"));
        tableUo_U2->setItem(1, 1, new QTableWidgetItem(fmtVal(iU0)));
        tableUo_U2->setItem(1, 2, new QTableWidgetItem(fmtVal(iU2)));



        //Voltage 1초 값 넣기
        //RMS
        oneSecData->voltage.RMS = { vTotalA, vTotalB, vTotalC, (vTotalA + vTotalB + vTotalC) / 3.0 };
        // Fundamental
        oneSecData->voltage.Fundamental = { vFundA, vFundB, vFundC, (vFundA + vFundB + vFundC) / 3.0 };
        // THD (%)
        oneSecData->voltage.THD = { thdVA, thdVB, thdVC };
        //frequency
        oneSecData->voltage.Frequency = dataMng.getInputFreq();
        //Residual
        oneSecData->voltage.Residual = vResidualRMS;


        emit oneSecChanged(oneSecData);

        // === 리셋 ===
        prev_sumTime = m.t;
        oneSecSampleCnt = 0;
        sumFundV2_A = sumFundV2_B = sumFundV2_C = 0.0;
        sumFundI2_A = sumFundI2_B = sumFundI2_C = 0.0;
        sumHarmV2 = sumHarmI2 = 0.0;
        sumTotalV2_A = sumTotalV2_B = sumTotalV2_C = 0.0;
        sumTotalI2_A = sumTotalI2_B = sumTotalI2_C = 0.0;
        sumPowerA = sumPowerB = sumPowerC = 0.0;
        sumResidualV = sumResidualI = 0.0;
    }
}


void oneSecMainWindow::setMainWindow(MainWindow* mainWin){
    mainWindow = mainWin;
}

void oneSecMainWindow::setInputWindow(inputMainWindow* inputWin){
    inputWindow = inputWin;
}

oneSecMainWindow::~oneSecMainWindow() = default;
