#include "a3700n_gui.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QToolBar>
#include <QActionGroup>
#include <QStackedWidget>
#include <QListWidget>
#include <QLabel>
#include <QFrame>
#include <QFile>
#include <QStyleFactory>
#include <QDebug>
#include <Qmenubar>

A3700N_GUI::A3700N_GUI(dataManagement& dataMng)
    : dataMng(dataMng)
{
    setWindowTitle("A3700N TEST");
    setFixedSize(800, 480);
    setStyle(QStyleFactory::create("Fusion")); // 전반 적인 시각 스타일을 fusion 테마로 변경한다 -> 일정한 스타일로 통일한다.
    loadQSS(":/styles/a3700n_style.qss");


    // === 상단 툴바 ===
    QToolBar* toolbar = new QToolBar(this);
    toolbar->setMovable(false);
    toolbar->setFloatable(false);
    toolbar->setFixedHeight(40);
    addToolBar(Qt::TopToolBarArea, toolbar);

    QAction* actVoltage = new QAction("VOLTAGE", this);
    QAction* actCurrent = new QAction("CURRENT", this);
    actVoltage->setCheckable(true);
    actCurrent->setCheckable(true);
    actVoltage->setChecked(true);

    QActionGroup* group = new QActionGroup(this);
    group->addAction(actVoltage);
    group->addAction(actCurrent);
    group->setExclusive(true);

    toolbar->addAction(actVoltage);
    toolbar->addAction(actCurrent);

    // === 메인 스택 (전체 전환) ===
    mainStack = new QStackedWidget(this);
    setCentralWidget(mainStack);

    // === VOLTAGE 페이지 생성 ===
    QWidget* voltagePage = createPage(
        {"voltage", "RMS", "Fundamental", "THD %", "Frequency", "Residual"}
        );

    // === CURRENT 페이지 생성 ===
    QWidget* currentPage = createPage(
        {"current", "RMS", "Fundamental", "THD %", "Residual"}
        );

    mainStack->addWidget(voltagePage); // index 0
    mainStack->addWidget(currentPage); // index 1

    // 각 페이지 마다 리스트 생성
    connect(actVoltage, &QAction::triggered, this, [=, this]() {
        mainStack->setCurrentWidget(voltagePage);
    });
    connect(actCurrent, &QAction::triggered, this, [=, this]() {
        mainStack->setCurrentWidget(currentPage);
    });

}

QWidget* A3700N_GUI::createPage(const QStringList& listNames)
{
    QWidget* page = new QWidget(this);
    page->setStyleSheet("border: none;");
    QHBoxLayout* mainLay = new QHBoxLayout(page);
    mainLay->setContentsMargins(0, 0, 0, 0);
    mainLay->setSpacing(0);

    // === 왼쪽 리스트 ===
    QListWidget* listWidget = new QListWidget(page);

    QFont listFont("맑은 고딕", 11, 600);
    listWidget->setFont(listFont);

    // listNames[0]는 리스트에 추가하지 않음
    for (int i = 1; i < listNames.size(); ++i) {
        listWidget->addItem(listNames[i]);

    }

    listWidget->setCurrentRow(0);
    mainLay->addWidget(listWidget);

    // === 오른쪽 스택 ===
    QStackedWidget* rightStack = new QStackedWidget(page);

    // 각 항목마다 페이지 생성
    QString baseKey = listNames[0];  // "voltage" or "current"

    for (int i = 1; i < listNames.size(); ++i) {
        QWidget* rightArea = new QWidget(page);
        rightArea->setObjectName("content");
        rightArea->setProperty("itemKey", listNames[i]);  // 예: "RMS", "THD %", ...

        QVBoxLayout* vLay = new QVBoxLayout(rightArea);
        vLay->setContentsMargins(20, 30, 20, 30);
        vLay->setSpacing(20);  // 항목 간 간격 (기본 spacing)
        vLay->setAlignment(Qt::AlignTop); // 위쪽 정렬


        // === 제목 ===
        QLabel* title = new QLabel(listNames[i], rightArea);
        title->setObjectName("title");
        vLay->addWidget(title);

        // 제목 아래 첫 구분선
        QFrame* titleLine = new QFrame(rightArea);
        titleLine->setFrameShape(QFrame::HLine);
        titleLine->setFrameShadow(QFrame::Plain);
        titleLine->setStyleSheet("background-color:#cccccc; height:1px;");
        vLay->addWidget(titleLine);

        QVector<QLabel*> valueLabels;

        QStringList fields;
        QString unit;

        // === 전압/전류 구분 ===
        if (baseKey == "voltage") {
            if (listNames[i] == "RMS" || listNames[i] == "Fundamental") {
                fields = {"A", "B", "C", "Average"};
                unit = "V";
            }
            else if (listNames[i] == "THD %") {
                fields = {"A", "B", "C"};
                unit = "%";
            }
            else if (listNames[i] == "Frequency") {
                fields = {"Frequency"};
                unit = "Hz";
            }
            else if (listNames[i] == "Residual") {
                fields = {"RMS"};
                unit = "V";
            }
        }
        else if (baseKey == "current") {
            if (listNames[i] == "RMS" || listNames[i] == "Fundamental") {
                fields = {"A", "B", "C", "Average"};
                unit = "A";
            }
            else if (listNames[i] == "THD %") {
                fields = {"A", "B", "C"};
                unit = "%";
            }
            else if (listNames[i] == "Frequency") {
                fields = {"Frequency"};
                unit = "Hz";
            }
            else if (listNames[i] == "Residual") {
                fields = {"Residual"};
                unit = "A";
            }
        }

        // === 각 항목 생성 ===
        for (int j = 0; j < fields.size(); ++j) {
            QHBoxLayout* row = new QHBoxLayout();

            QLabel* name = new QLabel(fields[j], rightArea);
            QLabel* val = new QLabel("--", rightArea);
            QLabel* u = new QLabel(unit, rightArea);

            name->setObjectName("name");
            val->setObjectName("val");
            u->setObjectName("u");

            name->setFixedWidth(120);
            val->setFixedWidth(120);
            u->setFixedWidth(50);

            row->addStretch();
            row->addWidget(name);
            row->addStretch();
            row->addStretch();
            row->addWidget(val);
            row->addWidget(u);
            row->addStretch();
            vLay->addLayout(row);

            valueLabels.append(val);

            // 항목 아래 구분선 추가 (마지막 제외)
            if (j != fields.size() - 1) {
                QFrame* line = new QFrame(rightArea);
                line->setFrameShape(QFrame::HLine);
                line->setFrameShadow(QFrame::Plain);
                line->setStyleSheet("background-color:#e0e0e0; height:1px;");
                vLay->addWidget(line);
            }
        }

        // === 라벨 리스트 저장 ===
        rightArea->setProperty("valueLabels", QVariant::fromValue(valueLabels));
        rightStack->addWidget(rightArea);
    }


    // === 리스트 클릭 시 스택 전환 ===
    connect(listWidget, &QListWidget::currentRowChanged,
            rightStack, &QStackedWidget::setCurrentIndex);

    mainLay->addWidget(rightStack);
    mainLay->setStretch(0, 1);
    mainLay->setStretch(1, 4);

    page->setProperty("stack", QVariant::fromValue((QObject*)rightStack));
    page->setProperty("dataKey", listNames[0]);
    return page;
}

void A3700N_GUI::updateOneSec(dataManagement::oneSecList* osl)
{
    if (!osl) return;

    // 현재 메인 페이지 (voltage / current)
    QWidget* currentPage = mainStack->currentWidget();
    QString dataKey = currentPage->property("dataKey").toString(); // "voltage" or "current"
    QStackedWidget* stack = qobject_cast<QStackedWidget*>(currentPage->property("stack").value<QObject*>());
    if (!stack) return;

    // 모든 스택 페이지 순회
    for (int i = 0; i < stack->count(); ++i) {
        QWidget* w = stack->widget(i);
        QString itemKey = w->property("itemKey").toString(); // "RMS", "THD %", "Residual" 등

        QVector<double> vals;

        // === 데이터 가져오기 ===
        if (dataKey == "voltage") {
            if (itemKey == "RMS") vals = osl->voltage.RMS;
            else if (itemKey == "Fundamental") vals = osl->voltage.Fundamental;
            else if (itemKey == "THD %") vals = osl->voltage.THD;
            else if (itemKey == "Frequency") vals = {osl->voltage.Frequency};
            else if (itemKey == "Residual") vals = {osl->voltage.Residual};
        }
        else if (dataKey == "current") {
            // if (itemKey == "RMS") vals = osl->current.RMS;
            // else if (itemKey == "Fundamental") vals = osl->current.Fundamental;
            // else if (itemKey == "THD %") vals = osl->current.THD;
            // else if (itemKey == "Frequency") vals = {osl->current.Frequency};
            // else if (itemKey == "Residual") vals = {osl->current.Residual};
        }

        // === 값 갱신 ===
        QVector<QLabel*> labels = w->property("valueLabels").value<QVector<QLabel*>>();
        if (labels.isEmpty() || vals.isEmpty()) continue;

        int n = std::min(labels.size(), vals.size());
        for (int j = 0; j < n; ++j) {
            labels[j]->setText(QString::number(vals[j], 'f', 3));
        }
    }
}





void A3700N_GUI::loadQSS(const QString& path)
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
