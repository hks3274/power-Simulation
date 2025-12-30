#include "mainwindow.h"

#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include "dataManagement.h"
#include "onesecmainwindow.h"
#include "inputmainwindow.h"
#include "a3700n_gui.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    dataManagement dataMng;

    MainWindow w(dataMng);         // 그래프 메인윈도우
    oneSecMainWindow ow(dataMng);  // 1초 데이터 메인윈도우
    A3700N_GUI aw(dataMng);
    inputMainWindow iw(dataMng, &w, &ow, &aw); // 입력창 메인윈도우1

    w.setInputWindow(&iw);
    w.setOneSecWindow(&ow);
    w.setA3700Window(&aw);

    ow.setInputWindow(&iw);
    ow.setMainWindow(&w);
    ow.setA3700Window(&aw);

    aw.setInputWindow(&iw);
    aw.setMainWindow(&w);
    aw.setOneSecWindow(&ow);

    // 화면 크기와 중앙 좌표
    QRect screenRect = QApplication::primaryScreen()->availableGeometry();
    int screenW = screenRect.width();
    int screenH = screenRect.height();
    int centerX = screenRect.left() + screenW / 2;
    int centerY = screenRect.top()  + screenH / 2;

    // 각 창 크기와 간격
    int gap = 30;
    int iwW = iw.width();
    int wW  =  w.width();
    int owW = ow.width();
    int maxH = std::max({iw.height(), w.height(), ow.height()});

    // 전체 폭
    int totalW = iwW + wW + owW + gap * 2;

    // 좌상단 시작점 (모니터 중앙 기준)
    int startX = centerX - totalW / 2;
    int startY = centerY - maxH / 2;

    // 좌→우 배치
    iw.move(startX, startY);
    w.move(startX + iwW + gap, startY);
    ow.move(startX + iwW + wW + gap * 2, startY);


    iw.show();
    w.hide();
    ow.hide();
    aw.show();

    // 세 창을 화면 밖으로 넘지 않게 "위치만" 조정 (리사이즈 X)
    QTimer::singleShot(0, [&](){
        const QRect screen = QApplication::primaryScreen()->availableGeometry();
        const int marginW = 6;
        const int marginH = 6;
        int gap = 10;

        // 현재 프레임 크기
        int iwW = iw.frameGeometry().width();
        int iwH = iw.frameGeometry().height();
        int wW  =  w.frameGeometry().width();
        int wH  =  w.frameGeometry().height();
        int owW = ow.frameGeometry().width();
        int owH = ow.frameGeometry().height();

        // 가용 영역
        const int availW = screen.width()  - marginW * 2;
        const int availH = screen.height() - marginH * 2;

        const int widthsNoGap = iwW + wW + owW;
        // 먼저 간격을 가능한 한 줄여본다
        if (widthsNoGap + 2 * gap > availW) {
            gap = std::max(0, (availW - widthsNoGap) / 2);
        }

        // 남은 초과치가 있으면 "창 크기" 자체를 비율로 줄인다
        const int maxH = std::max({iwH, wH, owH});
        const double scaleW = std::min(1.0, (availW - 2.0 * gap) / std::max(1, widthsNoGap));
        const double scaleH = std::min(1.0, (double)availH / std::max(1, maxH));
        double s = std::min(scaleW, scaleH);
        s = std::clamp(s, 0.5, 1.0); // 너무 작아지면 보기 힘드니 50%로 하한

        auto scaleResize = [&](QWidget& win){
            // 콘텐츠 기준 resize (프레임 제외)
            QSize sz = win.size();
            int nw = std::max(win.minimumWidth(),  int(std::round(sz.width()  * s)));
            int nh = std::max(win.minimumHeight(), int(std::round(sz.height() * s)));
            win.resize(nw, nh);
        };

        if (s < 1.0) {
            scaleResize(iw);
            scaleResize(w);
            scaleResize(ow);

            // 축소 후 프레임 크기 다시 취득
            iwW = iw.frameGeometry().width(); iwH = iw.frameGeometry().height();
            wW  =  w.frameGeometry().width();  wH  =  w.frameGeometry().height();
            owW = ow.frameGeometry().width(); owH = ow.frameGeometry().height();
        }

        // 중앙 기준 시작점 계산
        const int totalW = iwW + wW + owW + gap * 2;
        const int startYCenter = screen.top()  + marginH + (availH - std::min(std::max({iwH,wH,owH}), availH)) / 2;
        const int startXCenter = screen.left() + marginW + (availW - std::min(totalW, availW)) / 2;

        // 좌→우 배치
        int x = startXCenter;

        // 좌: 세로 중앙
        iw.move(x, startYCenter + (std::max({iwH,wH,owH}) - iwH) / 2);
        x += iwW + gap;

        // 가운데: 세로 중앙
        w.move(x, startYCenter + (std::max({iwH,wH,owH}) - wH) / 2);
        x += wW + gap;

        // 오른쪽: w의 상단 y에 정렬
        ow.move(x, w.frameGeometry().y());

        // 좌우 경계 클램프
        auto clampX = [&](QWidget& win){
            QRect fg = win.frameGeometry();
            int nx = std::clamp(fg.x(), screen.left() + marginW, screen.right() - marginW - fg.width() + 1);
            if (nx != fg.x()) win.move(nx, fg.y());
        };
        clampX(iw); clampX(w); clampX(ow);
    });

    // 모든 창 닫히면 앱 종료 (세 창 모두 반영)
    auto maybeQuit = [&](){
        if (!w.isVisible() && !ow.isVisible() && !iw.isVisible() && aw.isVisible()) {
            qApp->quit();
        }
    };
    QObject::connect(&w,  &QWidget::destroyed, maybeQuit);
    QObject::connect(&ow, &QWidget::destroyed, maybeQuit);
    QObject::connect(&iw, &QWidget::destroyed, maybeQuit);
    QObject::connect(&aw, &QWidget::destroyed, maybeQuit);

    QObject::connect(&iw, &inputMainWindow::loadActionPushed,
                     &w,  &MainWindow::doLoadAction);

    QObject::connect(&ow, &oneSecMainWindow::oneSecChanged,
                    &aw, &A3700N_GUI::updateOneSec);

    return app.exec();
}

