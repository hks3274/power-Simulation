#ifndef RMSACCUM_H
#define RMSACCUM_H

#include "dataStructures.h"
class RMSAccum
{
public:
    RMSAccum() = default;

    void reset();
    void accumulate(const RMSAccumInput& in);
    bool isFull(int N) const { return sampleCnt >= N; }

    measure_data compute(std::chrono::nanoseconds t, int N);

private:
    // === LN 누적 ===
    double sumAV=0, sumBV=0, sumCV=0;
    double sumAI=0, sumBI=0, sumCI=0;

    double fundSumAV=0, fundSumBV=0, fundSumCV=0;
    double fundSumAC=0, fundSumBC=0, fundSumCC=0;

    double harmSumAV=0, harmSumBV=0, harmSumCV=0;
    double harmSumAC=0, harmSumBC=0, harmSumCC=0;

    // === LL 누적 ===
    double fundSumAVLL=0, fundSumBVLL=0, fundSumCVLL=0;
    double sumAVLL=0, sumBVLL=0, sumCVLL=0;
    double harmSumAVLL=0, harmSumBVLL=0, harmSumCVLL=0;

    // === power, residual ===
    double sumP_A=0, sumP_B=0, sumP_C=0;
    double sumResidualV=0, sumResidualI=0;

    int sampleCnt = 0;
};

#endif // RMSACCUM_H
