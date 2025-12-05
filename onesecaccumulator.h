#ifndef ONESECACCUMULATOR_H
#define ONESECACCUMULATOR_H

#include <chrono>
#include <QObject>

class oneSecAccumulator
{
public:
    // 누적 변수들
    double sumFundV2_A = 0.0, sumFundV2_B = 0.0, sumFundV2_C = 0.0;
    double sumFundI2_A = 0.0, sumFundI2_B = 0.0, sumFundI2_C = 0.0;

    double sumTotalV2_A = 0.0, sumTotalV2_B = 0.0, sumTotalV2_C = 0.0;
    double sumTotalI2_A = 0.0, sumTotalI2_B = 0.0, sumTotalI2_C = 0.0;

    double sumHarmV2 = 0.0, sumHarmI2 = 0.0;

    // 선간 전압
    double sumFundV2_ALL = 0.0, sumFundV2_BLL = 0.0, sumFundV2_CLL = 0.0;
    double sumTotalV2_ALL = 0.0, sumTotalV2_BLL = 0.0, sumTotalV2_CLL = 0.0;
    double sumHarmAV2_LL = 0.0, sumHarmBV2_LL = 0.0, sumHarmCV2_LL = 0.0;

    double sumPowerA = 0.0, sumPowerB = 0.0, sumPowerC = 0.0;

    double sumResidualV = 0.0;
    double sumResidualI = 0.0;

    int oneSecSampleCnt = 0;
    std::chrono::nanoseconds prevTime{};

    void reset();

};

#endif // ONESECACCUMULATOR_H
