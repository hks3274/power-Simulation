#include "mainwindow.h"

#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include "dataManagement.h"
#include "onesecmainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    dataManagement dataMng;

    MainWindow w(dataMng);         // 그래프 메인윈도우
    oneSecMainWindow ow(dataMng);  // 1초 데이터 메인윈도우

    // 화면 크기 구하기
    QRect screenRect = QApplication::primaryScreen()->availableGeometry();
    int screenW = screenRect.width();
    int screenH = screenRect.height();

    // 화면 중앙 좌표
    int centerX = screenRect.left() + screenW / 2;
    int centerY = screenRect.top()  + screenH / 2;

    // 두 창 전체 폭
    int totalWidth = w.width() + ow.width();
    int totalHeight = std::max(w.height(), ow.height());

    // 전체 블록의 좌상단 기준점 (모니터 중앙에 맞추기)
    int startX = centerX - totalWidth / 2;
    int startY = centerY - totalHeight / 2;

    // 첫 번째 창 (왼쪽)
    w.move(startX - 60, startY);

    // 두 번째 창 (바로 오른쪽)
    ow.move(startX + w.width() + 100, startY);

    w.show();
    ow.show();

    // 창 닫힘 상태 추적용
    QObject::connect(&w, &QWidget::destroyed, [&]() {
        if (!w.isVisible() && !ow.isVisible()) {
            qApp->quit();
        }
    });

    QObject::connect(&ow, &QWidget::destroyed, [&]() {
        if (!w.isVisible() && !ow.isVisible()) {
            qApp->quit();
        }
    });

    return app.exec();
}

