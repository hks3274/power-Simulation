#ifndef A37__N_WAVE_H
#define A37__N_WAVE_H

#include "dataManagement.h"
#include "qcustomplot.h"
#include <QWidget>

class RichTextButton : public QPushButton {
public:
    RichTextButton(const QString &htmlText, QWidget *parent = nullptr)
        : QPushButton(parent), m_html(htmlText) {
    }

    void setHtml(const QString &html) {
        m_html = html;
        update(); // 다시 그리기 요청
    }


protected:
    void paintEvent(QPaintEvent *event) override {
        QPushButton::paintEvent(event);

        QTextDocument doc;
        doc.setHtml(m_html);

        QPainter painter(this);
        painter.translate(rect().center() - QPointF(doc.size().width()/2,
                                                    doc.size().height()/2));
        doc.drawContents(&painter);
    }


private:
    QString m_html;
};



class a3700n_wave: public QWidget
{
    Q_OBJECT
public:
    a3700n_wave(dataManagement& dataMng);

public slots:
    void setVolCheck(bool checked);
    void setCorCheck(bool checked);


private slots:
    void updateGraph();

private:
    dataManagement& dataMng;

    void loadQSS(const QString& listName);

    QCustomPlot* customPlot;
    QCPGraph *voltageGraphA{}, *voltageGraphB{}, *voltageGraphC{};
    QCPGraph *currentGraphA{}, *currentGraphB{}, *currentGraphC{};


    QCheckBox* VACheck;
    QCheckBox* VBCheck;
    QCheckBox* VCCheck;
    QCheckBox* CACheck;
    QCheckBox* CBCheck;
    QCheckBox* CCCheck;

    bool stopWave = false;

    QVector<double> stop_vA, stop_vB, stop_vC;
    QVector<double> stop_cA, stop_cB, stop_cC;


    QPushButton* autoBtn = nullptr;
    RichTextButton* VABtn = nullptr;
    QPushButton* plusBtn = nullptr;
    QPushButton* minusBtn = nullptr;

    double newRange = 4.0;
    double newRangeC = 2.0;


    QVector<double> vABuffer, vBBuffer, vCBuffer;
    QVector<double> cABuffer, cBBuffer, cCBuffer;

    qint64 sampleIndex_ = 0;         // 전체 샘플 인덱스
    int emitCounter_ = 0;            // 마지막 그리기 이후 샘플 누계


    double VMax = std::numeric_limits<double>::lowest(); // 가장 작은 값으로 초기화
    double VMin = std::numeric_limits<double>::max(); // 가장 큰 값으로 초기화
    double CMax = std::numeric_limits<double>::lowest();
    double CMin = std::numeric_limits<double>::max();


    void adjustScale(int direction);
    void autoScale();

};

#endif // A37__N_WAVE_H
