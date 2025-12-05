#ifndef ONESECCALCDATA_H
#define ONESECCALCDATA_H

#include "oneSecMinMax.h"
#include "dataManagement.h"
#include "onesecaccumulator.h"
#include "phasorcalculator.h"
#include <QObject>

class oneSecCalcData : public QObject
{
    Q_OBJECT
public:
    oneSecCalcData(dataManagement& dataMng);

signals:
    void oneSecTableChanged(oneSecTableData* tableData, oneSecList* osl);

private slots:
    void onRmsData(measure_data m);

private:
    dataManagement& dataMng;
    phasorCalculator* phasorCalc;

    oneSecList* oneSecData;
    oneSecTableData* tableData;

    oneSecAccumulator acc;
    oneSecMinMax mm;

    double sumEnergy = 0.0;

    phasor_data lastPhasor;  // 마지막 페이저 정보 저장용


};

#endif // ONESECCALCDATA_H
