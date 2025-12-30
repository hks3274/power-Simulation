#include "show_hide_check_widget.h"
#include <QLabel>

show_hide_check_widget::show_hide_check_widget(dataManagement& dataMng, QWidget* parent)
    : QWidget(parent), dataMng(dataMng)
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // 각 섹션 생성
    QWidget* secThree = createThreePhasePage();
    QWidget* secRms   = createRmsPage();
    QWidget* secPh    = createPhasorPage();

    // 섹션별로 아코디언 버튼 만들기
    auto makeAccordion = [&](const QString& title, QWidget* content) -> QWidget* {
        QWidget* wrapper = new QWidget;
        QVBoxLayout* lay = new QVBoxLayout(wrapper);
        lay->setContentsMargins(0, 0, 0, 0);

        QToolButton* header = new QToolButton;
        header->setText(title);
        header->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        header->setArrowType(Qt::RightArrow);
        header->setCheckable(true);
        header->setChecked(false);
        header->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

        header->setStyleSheet(
            "QToolButton {"
            "   background-color: white;"
            "   border: none;"
            "   border-radius: 6px;"
            "   text-align: left;"
            "   padding: 4px;"
            "}"
            "QToolButton:hover {"
            "   background-color: #f2f2f2;"   // 선택사항: 살짝 회색 강조
            "}"
            "QToolButton:checked {"
            "   background-color: white;"     // 토글돼도 흰색 유지
            "   color: black;"
            "}"
            );

        // 내부 내용 숨기기
        content->setVisible(false);

        // 클릭 시 펼침/닫힘
        connect(header, &QToolButton::toggled, this, [header, content](bool open){
            header->setArrowType(open ? Qt::DownArrow : Qt::RightArrow);
            content->setVisible(open);
        });

        lay->addWidget(header);
        lay->addWidget(content);
        return wrapper;
    };

    // 각 섹션 아코디언화
    mainLayout->addWidget(makeAccordion("3상 그래프", secThree));
    mainLayout->addWidget(makeAccordion("RMS 그래프", secRms));
    mainLayout->addWidget(makeAccordion("Phasor", secPh));
    mainLayout->addStretch();

    // 스크롤 영역
    QScrollArea* scroll = new QScrollArea(this);
    QWidget* container = new QWidget;
    container->setLayout(mainLayout);
    scroll->setWidget(container);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);

    QVBoxLayout* outer = new QVBoxLayout(this);
    outer->addWidget(scroll);
    setLayout(outer);
}

QWidget* show_hide_check_widget::createThreePhasePage()
{
    QWidget* page = new QWidget;
    QGridLayout* grid = new QGridLayout(page);

    QStringList phases = {"A", "B", "C"};
    QStringList types = {"전압", "전류"};

    grid->addWidget(new QLabel(""), 0, 0);
    for (int i = 0; i < phases.size(); ++i)
        grid->addWidget(new QLabel(phases[i]), 0, i + 1, Qt::AlignCenter);

    int row = 1;
    for (const QString& t : types) {
        grid->addWidget(new QLabel(t), row, 0, Qt::AlignRight);
        for (int i = 0; i < phases.size(); ++i) {
            QString key = QString("3P_%1_%2").arg(t).arg(phases[i]);
            chkMap[key] = new QCheckBox;
            chkMap[key]->setChecked(true);
            grid->addWidget(chkMap[key], row, i + 1, Qt::AlignCenter);
        }
        row++;
    }

    page->setLayout(grid);
    return page;
}

QWidget* show_hide_check_widget::createRmsPage()
{
    QWidget* page = new QWidget;
    QGridLayout* grid = new QGridLayout(page);

    // ==== [1] RMS (A/B/C) ====
    QStringList columns = {"A", "B", "C"};
    QStringList rmsRows = {"전체RMS 전압", "전체RMS 전류", "기본파 전압", "기본파 전류"};

    int row = 0;

    // 열 헤더
    grid->addWidget(new QLabel(""), row, 0);
    for (int c = 0; c < columns.size(); ++c)
        grid->addWidget(new QLabel(columns[c]), row, c + 1, Qt::AlignCenter);
    row++;

    // A/B/C 체크박스 테이블
    for (int r = 0; r < rmsRows.size(); ++r) {
        grid->addWidget(new QLabel(rmsRows[r]), row, 0, Qt::AlignRight);
        for (int c = 0; c < columns.size(); ++c) {
            QString key = QString("RMS_%1_%2").arg(rmsRows[r]).arg(columns[c]);
            chkMap[key] = new QCheckBox;
            chkMap[key]->setChecked(true);
            grid->addWidget(chkMap[key], row, c + 1, Qt::AlignCenter);
        }
        row++;
    }

    // ==== [2] 평균 전력 / 고조파 (단일) ====
    QStringList singleRows = {"평균전력(Pavg)", "고조파 전압", "고조파 전류"};

    // 살짝 시각적 간격 주기
    row++;

    for (const QString& name : singleRows) {
        QLabel* label = new QLabel(name);
        label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        grid->addWidget(label, row, 0);

        QString key = QString("RMS_%1_Value").arg(name);
        chkMap[key] = new QCheckBox;
        chkMap[key]->setChecked(true);
        grid->addWidget(chkMap[key], row, 1, Qt::AlignLeft);

        row++;
    }

    grid->setVerticalSpacing(6);
    grid->setHorizontalSpacing(10);
    grid->setContentsMargins(10, 10, 10, 10);

    page->setLayout(grid);
    return page;
}

QWidget* show_hide_check_widget::createPhasorPage()
{
    QWidget* page = new QWidget;
    QGridLayout* grid = new QGridLayout(page);

    QStringList columns = {"A", "B", "C", "고조파"};
    QStringList rows = {"전압", "전류"};

    grid->addWidget(new QLabel(""), 0, 0);
    for (int i = 0; i < columns.size(); ++i)
        grid->addWidget(new QLabel(columns[i]), 0, i + 1, Qt::AlignCenter);

    for (int r = 0; r < rows.size(); ++r) {
        grid->addWidget(new QLabel(rows[r]), r + 1, 0, Qt::AlignRight);
        for (int c = 0; c < columns.size(); ++c) {
            QString key = QString("PH_%1_%2").arg(rows[r]).arg(columns[c]);
            chkMap[key] = new QCheckBox;
            chkMap[key]->setChecked(true);
            grid->addWidget(chkMap[key], r + 1, c + 1, Qt::AlignCenter);
        }
    }

    page->setLayout(grid);
    return page;
}

void show_hide_check_widget::connectTargets(graphWidget* g, rmsGraphWidget* r, phasorWidget* p)
{
    gWidget = g;
    rWidget = r;
    pWidget = p;

    for (auto it = chkMap.begin(); it != chkMap.end(); ++it) {
        const QString& key = it.key();
        QCheckBox* box = it.value();

        if (key.startsWith("3P_") && gWidget) {
            connect(box, &QCheckBox::toggled, gWidget, [g, key](bool on){ g->setWaveVisible(key, on); });
        }
        else if (key.startsWith("RMS_") && rWidget) {
            connect(box, &QCheckBox::toggled, rWidget, [r, key](bool on){ r->setRmsVisible(key, on); });
        }
        else if (key.startsWith("PH_") && pWidget) {
            connect(box, &QCheckBox::toggled, pWidget, [p, key](bool on){ p->setPhasorVisible(key, on); });
        }
    }
}
