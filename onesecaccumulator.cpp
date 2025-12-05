#include "onesecaccumulator.h"

void oneSecAccumulator::reset()
{
    sumFundV2_A = sumFundV2_B = sumFundV2_C = 0.0;
    sumFundI2_A = sumFundI2_B = sumFundI2_C = 0.0;

    sumTotalV2_A = sumTotalV2_B = sumTotalV2_C = 0.0;
    sumTotalI2_A = sumTotalI2_B = sumTotalI2_C = 0.0;

    sumHarmV2 = 0.0;
    sumHarmI2 = 0.0;

    sumFundV2_ALL = sumFundV2_BLL = sumFundV2_CLL = 0.0;
    sumTotalV2_ALL = sumTotalV2_BLL = sumTotalV2_CLL = 0.0;

    sumHarmAV2_LL = sumHarmBV2_LL = sumHarmCV2_LL = 0.0;

    sumPowerA = sumPowerB = sumPowerC = 0.0;

    sumResidualV = sumResidualI = 0.0;

    oneSecSampleCnt = 0;
}
