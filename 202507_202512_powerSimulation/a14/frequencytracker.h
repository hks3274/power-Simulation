#ifndef FREQUENCYTRACKER_H
#define FREQUENCYTRACKER_H

#include <QObject>
#include "dataManagement.h"

class DFTKalmanTracker; // 전방 선언


class frequencytracker : public QObject {
    Q_OBJECT
public:
    explicit frequencytracker(dataManagement& dataMng);
    void start();
    void stop();


signals:
    void cpsChanged(double new_fs);

private slots:
    void onRms(dataManagement::measure_data m);


private:
    dataManagement& dataMng;
    bool running = false;
    double prevPhaseRad = 0.0;
    bool first = true;

    double lastZcTime = -1.0;     // 마지막 처리된 ZC 시간
    int zcCount = 0;              // ZC 카운터 (디버깅용)

};

#endif // FREQUENCYTRACKER_H
