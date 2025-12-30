#include "painter_chart.h"
#include <QPainter>

painter_chart::painter_chart(QWidget* parent)
    : QWidget(parent)
{

}


void painter_chart::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.fillRect(rect(), Qt::white);

    //마진 및 영역 계산
    const int leftMargin = 40;
    const int rightMargin = 10;
    const int topMargin = 10;
    const int bottomMargin = 30;
    int plotX = leftMargin;
    int plotY = topMargin;
    int plotW = width() - leftMargin - rightMargin;
    int plotH = height() - topMargin - bottomMargin;
    int zeroY = plotY + plotH / 2;

    // 배경 및 축 그리기 (동일)
    painter.setPen(Qt::black);
    painter.drawRect(plotX, plotY, plotW, plotH);
    painter.setPen(QPen(Qt::black, 1));
    painter.drawLine(plotX, zeroY, plotX + plotW, zeroY);
    painter.setPen(QPen(Qt::black, 1));
    painter.drawLine(plotX, plotY, plotX, plotY + plotH);


    // ===== Y축 라벨 & 눈금 (-500 ~ 500, 100 단위) ======
    painter.setPen(QPen(Qt::gray, 0.3, Qt::DashDotDotLine));
    QFontMetrics fm(painter.font());

    for (int v = -500; v <= 500; v += 100) {
        double r = (double)(v + 500) / 1000.0; // [-500~500] -> [0~1]
        int y = plotY + plotH - (int)(plotH * r);


        // 수평 보조선
        painter.setPen(QPen(Qt::gray, 0.3, Qt::DashDotDotLine));
        painter.drawLine(plotX, y, plotX + plotW, y);

        // 라벨 표시
        QString label = QString::number(v);
        int textW = fm.horizontalAdvance(label);
        painter.setPen(Qt::black);
        painter.drawText(plotX - textW - 5, y + fm.height()/2, label);
        painter.setPen(Qt::gray);
    }

    //===== X축 라벨 & 눈금 (최근 10초 기준) =============
    if (!m_plotPoints.isEmpty())
    {
        painter.setPen(Qt::black);

        // updateData에서 계산한 windowStart / windowSec을 그대로 쓴다
        double latest_time = m_latestTime;   // updateData에서 저장해두기!
        double windowStart = latest_time - windowSec;

        for (int i = 0; i <= windowSec; i += 1) {    // 0~10초 tick
            double t = windowStart + i;   // 실제 시간 값
            double d = i;                 // 0~10초
            double x = plotX + (plotW * (d / windowSec));

            // 보조선
            painter.setPen(QPen(Qt::gray, 0.3, Qt::DashDotDotLine));
            painter.drawLine(x, plotY, x, plotY + plotH);

            // 라벨 (실제 시간 넣어줌)
            painter.setPen(Qt::black);
            QString label = QString::number(t, 'f', 1);  // "123.4"
            int tw = fm.horizontalAdvance(label);
            painter.drawText(x - tw/2, plotY + plotH + fm.height() + 3, label);
        }
    }

    // === 데이터 그리기 ===
    if (!m_plotPoints.isEmpty()) {

        //선 그리기
        QPen linePen(Qt::blue, 1, Qt::SolidLine);
        painter.setPen(linePen);
        // m_plotPoints는 이미 화면 좌표이므로 바로 그립니다.
        painter.drawPolyline(m_plotPoints.constData(), m_plotPoints.size());

        //점그리기
        QPen pointPen(Qt::red, 3, Qt::SolidLine, Qt::RoundCap);
        painter.setPen(pointPen);
        painter.drawPoints(m_plotPoints.constData(), m_plotPoints.size());


    }
}


void painter_chart::updateData(const QVector<QPointF> &pts)
{
    m_plotPoints.clear(); // 기존 화면 좌표 초기화

    if (pts.isEmpty()) {
        update();
        return;
    }

    // ----- 항상 '최근 10초'만 화면에 보이게 -----
    double latest_time = pts.back().x();       // 가장 최신 시각
    m_latestTime = latest_time;
    double windowStart = latest_time - windowSec; // 윈도우 시작 = 최근 10초 전
    double duration    = windowSec;            // x축은 항상 10초 기준

    // ----- plot 영역 크기 계산 -----
    const int leftMargin   = 40;
    const int rightMargin  = 10;
    const int topMargin    = 10;
    const int bottomMargin = 30;

    int plotW = width()  - leftMargin - rightMargin;
    int plotH = height() - topMargin - bottomMargin;
    int zeroY = topMargin + plotH / 2;   // 중앙 Y 좌표 (0 기준)

    if (plotW <= 0 || plotH <= 0) {
        update();
        return;
    }

    // ----- 좌표 변환 -----
    for (const QPointF &point : pts) {
        double t   = point.x();
        double val = point.y();

        double d = t - windowStart;      // 윈도우 기준 시간
        if (d < 0) d = 0;                // 윈도우보다 이전 → 화면 가장 왼쪽
        if (d > duration) d = duration;  // 윈도우보다 이후 → 화면 가장 오른쪽

        // X 좌표 변환: 0~10초를 plotW에 매핑
        double x_pixel = leftMargin
                         + (static_cast<double>(plotW) * d / duration);

        // Y 좌표 변환 (예: 값 범위를 -500 ~ 500 으로 가정)
        double y_pixel = zeroY
                         - (static_cast<double>(plotH) / 2.0) * (val / 500.0);

        m_plotPoints << QPointF(x_pixel, y_pixel);
    }

    // 화면 갱신
    update();
}
