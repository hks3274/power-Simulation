#include "slideedit.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFrame>
#include <QFontMetrics>

slideEdit::slideEdit(QString cText, double cValue, double Max, double Min)
    : cValue(cValue), cMin(Min), cMax(Max)
{
    // 라벨
    cLabel = new QLabel(cText, this);
    QFont boldFont("나눔고딕", 9, QFont::Bold);
    cLabel->setFont(boldFont);

    // 라인에딧 (숫자 가독성 위해 최소 폭만 제안)
    cLineEdit = new QLineEdit(QString::number(cValue), this);
    cLineEdit->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    {
        QFontMetrics fm(cLineEdit->font());
        cLineEdit->setMinimumWidth(fm.horizontalAdvance(QStringLiteral("−0000.00")) + 10);
    }

    // 슬라이더 (내부 스케일 10배)
    cSlider = new QSlider(Qt::Horizontal, this);
    cSlider->setRange(static_cast<int>(Min * 10), static_cast<int>(Max * 10));
    cSlider->setValue(static_cast<int>(cValue * 10.0));
    cSlider->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    cSlider->setSingleStep(1);   // ←/→ 키, 휠(한 칸)
    cSlider->setPageStep(10);    // PgUp/PgDn
    cSlider->setTracking(true);
    cSlider->setFocusPolicy(Qt::StrongFocus);
    cSlider->setToolTip("←/→: 미세 조절, PgUp/PgDn: 크게 조절, 마우스 휠 지원");

    // min/max 라벨
    minLabel = new QLabel(QString::number(Min), this);
    maxLabel = new QLabel(QString::number(Max), this);
    minLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    maxLabel->setAlignment(Qt::AlignLeft  | Qt::AlignVCenter);
    minLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    maxLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

    // ---- 레이아웃 (여백/간격 슬림) ----
    auto sliderLayout = new QHBoxLayout();
    sliderLayout->setContentsMargins(4, 2, 4, 2);
    sliderLayout->setSpacing(4);
    sliderLayout->addWidget(minLabel);
    sliderLayout->addWidget(cSlider, /*stretch*/1);
    sliderLayout->addWidget(maxLabel);

    auto labelLayout = new QHBoxLayout();
    labelLayout->setContentsMargins(4, 2, 4, 2);
    labelLayout->setSpacing(4);
    labelLayout->addWidget(cLineEdit);

    // 프레임 (연한 톤)
    QFrame* outerFrame = new QFrame(this);
    outerFrame->setFrameStyle(QFrame::Box | QFrame::Plain);
    outerFrame->setLineWidth(1);
    outerFrame->setObjectName("myOuterFrame");
    outerFrame->setStyleSheet(
        "#myOuterFrame { border: 1px solid #d8d8d8; border-radius: 4px; background-color: #f7f7f7; }"
        );

    auto frameLayout_inner = new QVBoxLayout();
    frameLayout_inner->setContentsMargins(6, 4, 6, 4);
    frameLayout_inner->setSpacing(4);
    frameLayout_inner->addLayout(labelLayout);
    frameLayout_inner->addLayout(sliderLayout);

    auto frameLayout = new QHBoxLayout();
    frameLayout->setContentsMargins(6, 4, 6, 4);
    frameLayout->setSpacing(8);
    frameLayout->addWidget(cLabel);
    frameLayout->addLayout(frameLayout_inner);

    outerFrame->setLayout(frameLayout);

    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    mainLayout->addWidget(outerFrame);

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    // 시그널 연결
    connect(cSlider,   &QSlider::valueChanged,    this, &slideEdit::sliderChanged);
    connect(cLineEdit, &QLineEdit::returnPressed, this, &slideEdit::lineEditChanged);
}

void slideEdit::lineEditChanged()
{
    bool ok = false;
    double newVal = cLineEdit->text().toDouble(&ok);
    if (ok) {
        cValue = newVal * 10.0;
        cSlider->setValue(static_cast<int>(cValue));
    } else {
        cLineEdit->setText(QString::number(cValue / 10.0));
    }
}

void slideEdit::sliderChanged()
{
    cValue = cSlider->value();
    cLineEdit->setText(QString::number(cValue / 10.0));
    emit valueChanged(cValue / 10.0);
}

void slideEdit::setValue(double newVal)
{
    cValue = newVal * 10.0; // 내부는 10배 스케일
    cSlider->setValue(static_cast<int>(cValue));
    cLineEdit->setText(QString::number(newVal));
}
