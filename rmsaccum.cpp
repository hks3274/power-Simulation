#include "rmsaccum.h"

void RMSAccum::reset()
{
    sumAV=sumBV=sumCV=0;
    sumAI=sumBI=sumCI=0;

    fundSumAV=fundSumBV=fundSumCV=0;
    fundSumAC=fundSumBC=fundSumCC=0;

    harmSumAV=harmSumBV=harmSumCV=0;
    harmSumAC=harmSumBC=harmSumCC=0;

    fundSumAVLL=fundSumBVLL=fundSumCVLL=0;
    sumAVLL=sumBVLL=sumCVLL=0;
    harmSumAVLL=harmSumBVLL=harmSumCVLL=0;

    sumP_A=sumP_B=sumP_C=0;

    sumResidualV=sumResidualI=0;

    sampleCnt = 0;
}

void RMSAccum::accumulate(const RMSAccumInput& in)
{
    // LN 합성
    sumAV += in.vAVal * in.vAVal;
    sumBV += in.vBVal * in.vBVal;
    sumCV += in.vCVal * in.vCVal;

    sumAI += in.cAVal * in.cAVal;
    sumBI += in.cBVal * in.cBVal;
    sumCI += in.cCVal * in.cCVal;

    // LN 기본파
    fundSumAV += in.fundAV * in.fundAV;
    fundSumBV += in.fundBV * in.fundBV;
    fundSumCV += in.fundCV * in.fundCV;

    fundSumAC += in.fundAC * in.fundAC;
    fundSumBC += in.fundBC * in.fundBC;
    fundSumCC += in.fundCC * in.fundCC;

    // LN 고조파
    harmSumAV += in.harmAV * in.harmAV;
    harmSumBV += in.harmBV * in.harmBV;
    harmSumCV += in.harmCV * in.harmCV;

    harmSumAC += in.harmAC * in.harmAC;
    harmSumBC += in.harmBC * in.harmBC;
    harmSumCC += in.harmCC * in.harmCC;

    // LL 기본파, 합성, 고조파
    fundSumAVLL += in.fundAVLL * in.fundAVLL;
    fundSumBVLL += in.fundBVLL * in.fundBVLL;
    fundSumCVLL += in.fundCVLL * in.fundCVLL;

    sumAVLL += in.vAValLL * in.vAValLL;
    sumBVLL += in.vBValLL * in.vBValLL;
    sumCVLL += in.vCValLL * in.vCValLL;

    harmSumAVLL += in.harmAVLL * in.harmAVLL;
    harmSumBVLL += in.harmBVLL * in.harmBVLL;
    harmSumCVLL += in.harmCVLL * in.harmCVLL;

    // Power + Residual
    sumP_A += in.pA;
    sumP_B += in.pB;
    sumP_C += in.pC;

    sumResidualV += in.residualV * in.residualV;
    sumResidualI += in.residualI * in.residualI;

    sampleCnt++;
}

measure_data RMSAccum::compute(std::chrono::nanoseconds t, int N)
{
    const double inv = 1.0 / N;

    measure_data m{};
    m.t = t;

    // LN RMS
    m.vRmsA = std::sqrt(sumAV * inv);
    m.vRmsB = std::sqrt(sumBV * inv);
    m.vRmsC = std::sqrt(sumCV * inv);

    m.iRmsA = std::sqrt(sumAI * inv);
    m.iRmsB = std::sqrt(sumBI * inv);
    m.iRmsC = std::sqrt(sumCI * inv);

    // LN 기본파
    m.vFundA = std::sqrt(fundSumAV * inv);
    m.vFundB = std::sqrt(fundSumBV * inv);
    m.vFundC = std::sqrt(fundSumCV * inv);

    m.iFundA = std::sqrt(fundSumAC * inv);
    m.iFundB = std::sqrt(fundSumBC * inv);
    m.iFundC = std::sqrt(fundSumCC * inv);

    // LN 고조파
    m.vHarmA = std::sqrt(harmSumAV * inv);
    m.vHarmB = std::sqrt(harmSumBV * inv);
    m.vHarmC = std::sqrt(harmSumCV * inv);

    m.iHarmA = std::sqrt(harmSumAC * inv);
    m.iHarmB = std::sqrt(harmSumBC * inv);
    m.iHarmC = std::sqrt(harmSumCC * inv);

    // 선간전압
    m.vFundALL = std::sqrt(fundSumAVLL * inv);
    m.vFundBLL = std::sqrt(fundSumBVLL * inv);
    m.vFundCLL = std::sqrt(fundSumCVLL * inv);

    m.vRmsALL = std::sqrt(sumAVLL * inv);
    m.vRmsBLL = std::sqrt(sumBVLL * inv);
    m.vRmsCLL = std::sqrt(sumCVLL * inv);

    m.vHarmALL = std::sqrt(harmSumAVLL * inv);
    m.vHarmBLL = std::sqrt(harmSumBVLL * inv);
    m.vHarmCLL = std::sqrt(harmSumCVLL * inv);

    // Power
    m.pA = sumP_A * inv;
    m.pB = sumP_B * inv;
    m.pC = sumP_C * inv;
    m.pTotal = m.pA + m.pB + m.pC;

    // Residual
    m.residualSumV = sumResidualV;
    m.residualSumI = sumResidualI;

    reset();
    return m;
}
