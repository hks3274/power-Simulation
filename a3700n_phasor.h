#ifndef A3700N_PHASOR_H
#define A3700N_PHASOR_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QPainter>
#include <QTimer>
#include <QVector>
#include <QPushButton>
#include "dataManagement.h"
#include "phasorcalculator.h"

class a3700n_phasor : public QWidget
{
    Q_OBJECT
public:
    explicit a3700n_phasor(dataManagement& dataMng, bool volCheck, bool curCheck);

public slots:
    void setVolCheck(bool enabled);
    void setCurCheck(bool enabled);
    void updatePhasor(measure_data m);

protected:
    void paintEvent(QPaintEvent*) override;

private:
    dataManagement& dataMng;
    bool volCheck, curCheck;

    phasorCalculator* phasorCalc;

    // 오른쪽 수치표 라벨 저장
    QVector<QLabel*> voltMagLabels, voltPhaseLabels;
    QVector<QLabel*> currMagLabels, currPhaseLabels;

    std::chrono::nanoseconds prev_sumTime{};

    // 멤버 변수 추가
    double vAX, vAY, vBX, vBY, vCX, vCY;
    double cAX, cAY, cBX, cBY, cCX, cCY;

    QWidget* voltGroup = nullptr;
    QWidget* currGroup = nullptr;


    QPushButton* vLLBtn;
    QPushButton* vLNBtn;

    phasor_data lastPh;

    void nowUpdate(phasor_data ph);
};

#endif
