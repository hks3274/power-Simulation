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
#include "dataManagement.h"

class a3700n_phasor : public QWidget
{
    Q_OBJECT
public:
    explicit a3700n_phasor(dataManagement& dataMng, bool volCheck, bool curCheck);

public slots:
    void setVolCheck(bool enabled);
    void setCurCheck(bool enabled);

protected:
    void paintEvent(QPaintEvent*) override;

private slots:
    void updatePhasor(dataManagement::measure_data m);

private:
    dataManagement& dataMng;
    bool volCheck, curCheck;

    dataManagement::phasor_data phasor;

    // 오른쪽 수치표 라벨 저장
    QVector<QLabel*> voltMagLabels, voltPhaseLabels;
    QVector<QLabel*> currMagLabels, currPhaseLabels;

    std::chrono::nanoseconds prev_sumTime{};

    // 멤버 변수 추가
    double vAX, vAY, vBX, vBY, vCX, vCY;
    double cAX, cAY, cBX, cBY, cCX, cCY;

    QWidget* voltGroup = nullptr;
    QWidget* currGroup = nullptr;

};

#endif
