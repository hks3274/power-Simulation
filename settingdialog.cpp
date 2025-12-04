#include "settingdialog.h"
#include <QBoxLayout>
#include <QMouseEvent>   // 추가

settingDialog::settingDialog (settingStorage& storage, dataManagement& dataMng, QWidget* parent)
    : QDialog(parent), settings(storage), dataMng(dataMng)
{
    setWindowTitle("설정 저장 관리");
    resize(500, 400);
    setFixedSize(500, 500);

    QHBoxLayout* mainLay = new QHBoxLayout(this);
    mainLay->setContentsMargins(18,18,18,18);
    mainLay->setSpacing(10);

    listWidget = new QListWidget(this);
    nameEdit = new QLineEdit(this);
    nameEdit->setPlaceholderText("저장할 이름 입력");
    saveButton = new QPushButton("저장하기", this);

    // 왼쪽 영역
    QVBoxLayout* leftLay = new QVBoxLayout();
    leftLay->addWidget(listWidget);

    QHBoxLayout* hbuttonlay = new QHBoxLayout();
    loadButton = new QPushButton("불러오기", this);
    delButton = new QPushButton("삭제", this);
    hbuttonlay->addWidget(loadButton);
    hbuttonlay->addWidget(delButton);
    leftLay->addLayout(hbuttonlay);

    // 오른쪽 영역
    QVBoxLayout* rightLay = new QVBoxLayout();
    settingView = new QLabel("Setting View", this);
    rightLay->addWidget(settingView);

    QHBoxLayout* hlay = new QHBoxLayout();
    hlay->addWidget(nameEdit);
    hlay->addWidget(saveButton);
    rightLay->addLayout(hlay);

    // 레이아웃 합침
    QHBoxLayout* hMain = new QHBoxLayout();
    hMain->addLayout(leftLay, 1);
    hMain->addLayout(rightLay, 1);
    mainLay->addLayout(hMain);
    setLayout(mainLay);

    // --- 저장된 이름 목록 불러오기 ---
    auto names = settings.getAllSaveNames();
    for (const auto& name : names)
        listWidget->addItem(QString::fromStdString(name));

    if (listWidget->count() == 0) {
        listWidget->addItem("저장된 설정이 없습니다.");
        listWidget->item(0)->setFlags(Qt::NoItemFlags);
    }

    // --- 초기 라벨 표시 ---
    if (listWidget->currentRow() == -1) {
        currentValueView();
    }

    // --- 리스트 클릭 시 ---
    connect(listWidget, &QListWidget::itemClicked, [this](QListWidgetItem* item) {
        if (!item) return;
        QString info = settings.previewLoadText(item->text().toStdString());

        settingView->setText(info);
        settingView->setAlignment(Qt::AlignTop | Qt::AlignLeft);
        settingView->setStyleSheet(
            "font-family: Consolas; font-size: 10pt; "
            "background-color: #f9f9f9; border: 1px solid #ccc; padding: 6px;"
            );
        settingView->setWordWrap(true);

        // 버튼 상태 전환 (리스트 항목 선택됨)
        saveButton->setEnabled(false);
        nameEdit->setEnabled(false);
        loadButton->setEnabled(true);
        delButton->setEnabled(true);
    });


    // --- 저장하기 ---
    connect(saveButton, &QPushButton::clicked, [this, &dataMng]() {
        QString name = nameEdit->text().trimmed();
        if (name.isEmpty()) {
            QMessageBox::warning(this, "이름 없음", "저장할 이름을 입력하세요.");
            return;
        }

        // --- 이미 존재하는 이름인지 확인 ---
        auto existingNames = settings.getAllSaveNames();
        bool duplicate = std::any_of(existingNames.begin(), existingNames.end(),
                                     [&](const std::string& n) {
                                         return QString::fromStdString(n).compare(name, Qt::CaseSensitive) == 0;
                                     });

        if (duplicate) {
            QMessageBox::warning(this, "중복 이름",
                                 QString("'%1' 이름이 이미 존재합니다.\n"
                                         "다른 이름을 입력해주세요.").arg(name));
            return;
        }

        // --- 저장 실행 ---
        settings.save(name.toStdString(), dataMng);
        QMessageBox::information(this, "저장 완료",
                                 QString("'%1' 설정이 저장되었습니다.").arg(name));

        // --- 리스트 새로고침 ---
        listWidget->clear();
        auto names = settings.getAllSaveNames();
        for (const auto& n : names)
            listWidget->addItem(QString::fromStdString(n));

        settings.saveLastLoadedName(name.toStdString());

        // --- 입력칸 초기화 ---
        nameEdit->clear();
    });

    // --- 불러오기 ---
    connect(loadButton, &QPushButton::clicked, [this, &dataMng]() {
        auto selectedItem = listWidget->currentItem();
        if (!selectedItem) return;

        QString selectedName = selectedItem->text();
        if (selectedName == "저장된 설정이 없습니다.") return;

        settings.loadByName(selectedName.toStdString(), dataMng);
        settings.saveLastLoadedName(selectedName.toStdString());

        accept();  // 다이얼로그 닫기
    });


    //삭제하기
    connect(delButton, &QPushButton::clicked, [this]() {
        auto selectedItem = listWidget->currentItem();
        if (!selectedItem) return;

        QString selectedName = selectedItem->text();
        if (selectedName == "저장된 설정이 없습니다.") return;

        // --- 기본값은 삭제 금지 ---
        if (selectedName == "기본값") {
            QMessageBox::warning(this, "삭제 불가",
                                 "기본값은 삭제할 수 없습니다.");
            return;
        }

        // --- 삭제 확인 ---
        QMessageBox::StandardButton reply = QMessageBox::question(
            this,
            "삭제 확인",
            QString("'%1' 설정을 삭제하시겠습니까?").arg(selectedName),
            QMessageBox::Yes | QMessageBox::No
            );

        if (reply == QMessageBox::Yes) {
            // --- DB에서 삭제 ---
            settings.deleteByName(selectedName.toStdString());

            // --- 만약 마지막 불러온 이름과 동일하면 기본값으로 복원 ---
            std::string lastName = settings.getLastLoadedName();
            if (QString::fromStdString(lastName) == selectedName) {
                settings.saveLastLoadedName("기본값");
                qDebug() << "[삭제된 이름이 마지막 설정이므로 기본값으로 복원]";
            }

            // --- 리스트 갱신 ---
            listWidget->clear();
            auto names = settings.getAllSaveNames();
            for (const auto& n : names)
                listWidget->addItem(QString::fromStdString(n));

            if (listWidget->count() == 0) {
                listWidget->addItem("저장된 설정이 없습니다.");
                listWidget->item(0)->setFlags(Qt::NoItemFlags);
            }

            // --- 오른쪽 영역 리셋 ---
            settingView->clear();
            currentValueView();

            // --- 버튼 상태 복귀 ---
            saveButton->setEnabled(true);
            nameEdit->setEnabled(true);
            loadButton->setEnabled(false);
            delButton->setEnabled(false);

            QMessageBox::information(this, "삭제 완료",
                                     QString("'%1' 설정이 삭제되었습니다.").arg(selectedName));
        }
    });





    // --- 초기 상태 (현재값 보기) ---
    saveButton->setEnabled(true);
    nameEdit->setEnabled(true);
    loadButton->setEnabled(false);
    delButton->setEnabled(false);
}


void settingDialog::currentValueView()
{
    QString info;
    info += QString("[ 현재값 ] \n\n");
    info += QString("주파수 (Hz):      %1\n").arg(dataMng.getInputFreq());
    info += QString("초당 Cycle 수:    %1\n").arg(dataMng.getCyclePerSecond());
    info += QString("Cycle당 샘플 수:  %1\n").arg(dataMng.getSamplePerCycle());
    info += QString("TimeScale:       %1\n").arg(dataMng.getTimeScale());
    info += QString("그래프 폭:        %1\n").arg(dataMng.getGraphWidth());
    info += "\n--- 3상 전압 ---\n";
    info += QString("A상:  V=%1\n").arg(dataMng.getVAValue());
    info += QString("B상:  V=%1,  φ=%2°\n").arg(dataMng.getVBValue()).arg(dataMng.getVBPhaseDeg());
    info += QString("C상:  V=%1,  φ=%2°\n").arg(dataMng.getVCValue()).arg(dataMng.getVCPhaseDeg());
    info += "\n--- 3상 전류 ---\n";
    info += QString("A상:  I=%1,  φ=%2°\n").arg(dataMng.getIAValue()).arg(dataMng.getIAPhaseDeg());
    info += QString("B상:  I=%1,  φ=%2°\n").arg(dataMng.getIBValue()).arg(dataMng.getIBPhaseDeg());
    info += QString("C상:  I=%1,  φ=%2°\n").arg(dataMng.getICValue()).arg(dataMng.getICPhaseDeg());
    info += "\n--- 고조파 ---\n";
    info += QString("전압: %1차, 크기=%2, 위상=%3°\n")
                .arg(dataMng.getVHarmonicOrder())
                .arg(dataMng.getVHarmonicMag())
                .arg(dataMng.getVHarmonicPhase());
    info += QString("전류: %1차, 크기=%2, 위상=%3°")
                .arg(dataMng.getCHarmonicOrder())
                .arg(dataMng.getCHarmonicMag())
                .arg(dataMng.getCHarmonicPhase());

    settingView->setText(info);
    settingView->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    settingView->setStyleSheet(
        "font-family: Consolas; font-size: 10pt; "
        "background-color: #f9f9f9; border: 1px solid #ccc; padding: 6px;"
        );
    settingView->setWordWrap(true);

    // 현재값 보기로 돌아오면 버튼 상태 복귀
    saveButton->setEnabled(true);
    nameEdit->setEnabled(true);
    loadButton->setEnabled(false);
    delButton->setEnabled(false);
}


void settingDialog::mousePressEvent(QMouseEvent* event)
{
    if (!listWidget->geometry().contains(event->pos())) {
        listWidget->clearSelection();  // 선택 해제
        settingView->clear();
        currentValueView();            // 현재값 보기로 복귀
    }
    QDialog::mousePressEvent(event);
}
