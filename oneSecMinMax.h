#ifndef ONESECMINMAX_H
#define ONESECMINMAX_H

#include <QString>
#include <limits>

class oneSecMinMax
{
public:
    struct MinMax {
        double min = std::numeric_limits<double>::max();
        double max = std::numeric_limits<double>::lowest();
        QString minTime;
        QString maxTime;

        void update(double value, const QString& now) {
            if (value > max) { max = value; maxTime = now; }
            if (value < min) { min = value; minTime = now; }
        }
    };

    // ==== Min/Max 목록 ====
    MinMax vTotalAMM, vTotalBMM, vTotalCMM, vTotalAvgMM;
    MinMax vTotalALLMM, vTotalBLLMM, vTotalCLLMM, vTotalAvgLLMM;
    MinMax vFundAMM, vFundBMM, vFundCMM, vFundAvgMM;
    MinMax vFundALLMM, vFundBLLMM, vFundCLLMM, vFundAvgLLMM;
    MinMax thdVAMM, thdVBMM, thdVCMM;
    MinMax thdVALLMM, thdVBLLMM, thdVCLLMM;
    MinMax freqMM;
    MinMax vResidualRMSMM;


    MinMax iTotalAMM, iTotalBMM, iTotalCMM, iTotalAvgMM;
    MinMax iFundAMM, iFundBMM, iFundCMM, iFundAvgMM;
    MinMax thdIAMM, thdIBMM, thdICMM;
    MinMax iResidualRMSMM;


    MinMax pAMM, pBMM, pCMM, pTotalMM;
    MinMax qAMM, qBMM, qCMM, qSumMM;
    MinMax sAMM, sBMM, sCMM, sSumMM;
    MinMax pfAMM, pfBMM, pfCMM, pfSumMM;


    MinMax Vpos_magMM, Vneg_magMM, Vzero_magMM;
    MinMax Vpos_mag_LLMM, Vneg_mag_LLMM, Vzero_mag_LLMM;
    MinMax Ipos_magMM, Ineg_magMM, Izero_magMM;
    MinMax vLLUnbalanceMM, vUnbalanceMM, vU2MM, vU0MM;
    MinMax iUnbalanceMM, iU2MM, iU0MM;
};

#endif // ONESECMINMAX_H
