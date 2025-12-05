#include "oneseccalcdata.h"
#include <complex>
#include <QDateTime>

oneSecCalcData::oneSecCalcData(dataManagement& dataMng)
    : dataMng(dataMng)
{
    phasorCalc = new phasorCalculator(dataMng);
    connect(&dataMng, &dataManagement::rmsDataChanged, this, &oneSecCalcData::onRmsData);
}

void oneSecCalcData::onRmsData(measure_data m)
{
    auto elapsed = m.t - acc.prevTime;
    acc.oneSecSampleCnt++;

    oneSecData = new oneSecList();
    tableData = new oneSecTableData();

    // 누적
    acc.sumFundV2_A += m.vFundA * m.vFundA;
    acc.sumFundV2_B += m.vFundB * m.vFundB;
    acc.sumFundV2_C += m.vFundC * m.vFundC;
    acc.sumFundI2_A += m.iFundA * m.iFundA;
    acc.sumFundI2_B += m.iFundB * m.iFundB;
    acc.sumFundI2_C += m.iFundC * m.iFundC;

    acc.sumHarmV2 += m.vHarmA * m.vHarmA;
    acc.sumHarmI2 += m.iHarmA * m.iHarmA;

    acc.sumTotalV2_A += m.vRmsA * m.vRmsA;
    acc.sumTotalV2_B += m.vRmsB * m.vRmsB;
    acc.sumTotalV2_C += m.vRmsC * m.vRmsC;
    acc.sumTotalI2_A += m.iRmsA * m.iRmsA;
    acc.sumTotalI2_B += m.iRmsB * m.iRmsB;
    acc.sumTotalI2_C += m.iRmsC * m.iRmsC;

    acc.sumPowerA += m.pA;
    acc.sumPowerB += m.pB;
    acc.sumPowerC += m.pC;

    //선간전압
    acc.sumFundV2_ALL += m.vFundALL * m.vFundALL;
    acc.sumFundV2_BLL += m.vFundBLL * m.vFundBLL;
    acc.sumFundV2_CLL += m.vFundCLL * m.vFundCLL;

    acc.sumTotalV2_ALL += m.vRmsALL * m.vRmsALL;
    acc.sumTotalV2_BLL += m.vRmsBLL * m.vRmsBLL;
    acc.sumTotalV2_CLL += m.vRmsCLL * m.vRmsCLL;

    acc.sumHarmAV2_LL += m.vHarmALL * m.vHarmALL;
    acc.sumHarmBV2_LL += m.vHarmBLL * m.vHarmBLL;
    acc.sumHarmCV2_LL += m.vHarmCLL * m.vHarmCLL;


    if (elapsed >= std::chrono::milliseconds(995)) {
        // === RMS 계산 ===
        double vFundA = std::sqrt(acc.sumFundV2_A / acc.oneSecSampleCnt);
        double vFundB = std::sqrt(acc.sumFundV2_B / acc.oneSecSampleCnt);
        double vFundC = std::sqrt(acc.sumFundV2_C / acc.oneSecSampleCnt);
        double iFundA = std::sqrt(acc.sumFundI2_A / acc.oneSecSampleCnt);
        double iFundB = std::sqrt(acc.sumFundI2_B / acc.oneSecSampleCnt);
        double iFundC = std::sqrt(acc.sumFundI2_C / acc.oneSecSampleCnt);

        double vHarm = std::sqrt(acc.sumHarmV2 / acc.oneSecSampleCnt);
        double iHarm = std::sqrt(acc.sumHarmI2 / acc.oneSecSampleCnt);

        double vTotalA = std::sqrt(acc.sumTotalV2_A / acc.oneSecSampleCnt);
        double vTotalB = std::sqrt(acc.sumTotalV2_B / acc.oneSecSampleCnt);
        double vTotalC = std::sqrt(acc.sumTotalV2_C / acc.oneSecSampleCnt);
        double iTotalA = std::sqrt(acc.sumTotalI2_A / acc.oneSecSampleCnt);
        double iTotalB = std::sqrt(acc.sumTotalI2_B / acc.oneSecSampleCnt);
        double iTotalC = std::sqrt(acc.sumTotalI2_C / acc.oneSecSampleCnt);

        //============= 선간 전압 ==========================
        double vFundALL = std::sqrt(acc.sumFundV2_ALL/ acc.oneSecSampleCnt);
        double vFundBLL = std::sqrt(acc.sumFundV2_BLL/ acc.oneSecSampleCnt);
        double vFundCLL = std::sqrt(acc.sumFundV2_CLL/ acc.oneSecSampleCnt);

        double vTotalALL = std::sqrt(acc.sumTotalV2_ALL / acc.oneSecSampleCnt);
        double vTotalBLL = std::sqrt(acc.sumTotalV2_BLL / acc.oneSecSampleCnt);
        double vTotalCLL = std::sqrt(acc.sumTotalV2_CLL / acc.oneSecSampleCnt);

        double vHarmALL = std::sqrt(acc.sumHarmAV2_LL / acc.oneSecSampleCnt);
        double vHarmBLL = std::sqrt(acc.sumHarmBV2_LL / acc.oneSecSampleCnt);
        double vHarmCLL = std::sqrt(acc.sumHarmCV2_LL / acc.oneSecSampleCnt);



        // === 위상 ===
        lastPhasor = phasorCalc->calcPhasor();
        double vPhA = lastPhasor.vAPhaseDeg;
        double vPhB = lastPhasor.vBPhaseDeg;
        double vPhC = lastPhasor.vCPhaseDeg;
        double iPhA = lastPhasor.cAPhaseDeg;
        double iPhB = lastPhasor.cBPhaseDeg;
        double iPhC = lastPhasor.cCPhaseDeg;
        double vHph = lastPhasor.hvPhaseDeg;
        double iHph = lastPhasor.hcPhaseDeg;

        double dt_sec = std::chrono::duration_cast<std::chrono::duration<double>>(elapsed).count();
        double pA = acc.sumPowerA / acc.oneSecSampleCnt;
        double pB = acc.sumPowerB / acc.oneSecSampleCnt;
        double pC = acc.sumPowerC / acc.oneSecSampleCnt;
        double pTotal = pA + pB + pC;
        sumEnergy += pTotal * dt_sec;
        double energyWh = sumEnergy / 3600.0;


        tableData->vFundA = vFundA;
        tableData->vFundB = vFundB;
        tableData->vFundC = vFundC;
        tableData->iFundA = iFundA;
        tableData->iFundB = iFundB;
        tableData->iFundC = iFundC;
        tableData->vPhA = vPhA;
        tableData->vPhB = vPhB;
        tableData->vPhC = vPhC;
        tableData->iPhA = iPhA;
        tableData->iPhB = iPhB;
        tableData->iPhC = iPhC;
        tableData->vHarm = vHarm;
        tableData->iHarm = iHarm;
        tableData->vHph = vHph;
        tableData->iHph = iHph;
        tableData->vTotalA = vTotalA;
        tableData->vTotalB = vTotalB;
        tableData->vTotalC = vTotalC;
        tableData->iTotalA = iTotalA;
        tableData->iTotalB = iTotalB;
        tableData->iTotalC = iTotalC;


        // === 전력 정보 ===
        double sA = vTotalA * iTotalA;
        double sB = vTotalB * iTotalB;
        double sC = vTotalC * iTotalC;
        double sSum = sA + sB + sC;

        double qA = std::sqrt(std::max(0.0, sA*sA - pA*pA));
        double qB = std::sqrt(std::max(0.0, sB*sB - pB*pB));
        double qC = std::sqrt(std::max(0.0, sC*sC - pC*pC));
        double qSum = qA + qB + qC;

        double pfA = (sA > 1e-9)? pA/sA : 0.0;
        double pfB = (sB > 1e-9)? pB/sB : 0.0;
        double pfC = (sC > 1e-9)? pC/sC : 0.0;
        double pfSum = (sSum > 1e-9)? pTotal/sSum : 0.0;

        tableData->pA = pA;
        tableData->pB = pB;
        tableData->pC = pC;
        tableData->pTotal = pTotal;
        tableData->sA = sA;
        tableData->sB = sB;
        tableData->sC = sC;
        tableData->sSum = sSum;
        tableData->qA = qA;
        tableData->qB = qB;
        tableData->qC = qC;
        tableData->qSum = qSum;
        tableData->pfA = pfA;
        tableData->pfB = pfB;
        tableData->pfC = pfC;
        tableData->pfSum = pfSum;
        tableData->energyWh = energyWh;

        // === THD 계산 ===
        auto safeTHD = [](double harm, double fund) {
            if (fund < 1e-9)        // 기본파가 사실상 0이면
                return std::numeric_limits<double>::infinity();  // ∞ (무한대)
            else
                return (harm / fund * 100.0);
        };

        double thdVA = safeTHD(vHarm, vFundA);
        double thdVB = safeTHD(vHarm, vFundB);
        double thdVC = safeTHD(vHarm, vFundC);
        double thdIA = safeTHD(iHarm, iFundA);
        double thdIB = safeTHD(iHarm, iFundB);
        double thdIC = safeTHD(iHarm, iFundC);

        //선간전압
        double thdVALL = safeTHD(vHarmALL, vFundALL);
        double thdVBLL = safeTHD(vHarmBLL, vFundBLL);
        double thdVCLL = safeTHD(vHarmCLL, vFundCLL);


        tableData->thdVA = thdVA;
        tableData->thdVB = thdVB;
        tableData->thdVC = thdVC;
        tableData->thdIA = thdIA;
        tableData->thdIB = thdIB;
        tableData->thdIC = thdIC;

        //==========Reaidual==================

        // === Residual RMS 계산 ===
        double vResidualRMS = std::sqrt(m.residualSumV / dataMng.getSamplePerCycle());
        double iResidualRMS = std::sqrt(m.residualSumI / dataMng.getSamplePerCycle());

        tableData->vResidualRMS = vResidualRMS;
        tableData->iResidualRMS = iResidualRMS;


        //==========simmentic component==================
        std::complex<double> a(-0.5, std::sqrt(3)/2.0); //120
        std::complex<double> a2 = std::pow(a, 2); //-120

        auto deg2rad = [](double d){ return d * M_PI / 180.0; }; //라디안으로 변환
        lastPhasor = phasorCalc->calcPhasor();
        // --- 전압 phasor ---
        std::complex<double> VA = std::polar(lastPhasor.vAMag, deg2rad(lastPhasor.vAPhaseDeg));
        std::complex<double> VB = std::polar(lastPhasor.vBMag, deg2rad(lastPhasor.vBPhaseDeg));
        std::complex<double> VC = std::polar(lastPhasor.vCMag, deg2rad(lastPhasor.vCPhaseDeg));

        std::complex<double> VALL = std::polar(lastPhasor.vALLMag, deg2rad(lastPhasor.vALLPhaseDeg));
        std::complex<double> VBLL = std::polar(lastPhasor.vBLLMag, deg2rad(lastPhasor.vBLLPhaseDeg));
        std::complex<double> VCLL = std::polar(lastPhasor.vCLLMag, deg2rad(lastPhasor.vCLLPhaseDeg));

        // --- 전류 phasor ---
        std::complex<double> IA = std::polar(lastPhasor.cAMag, deg2rad(lastPhasor.cAPhaseDeg));
        std::complex<double> IB = std::polar(lastPhasor.cBMag, deg2rad(lastPhasor.cBPhaseDeg));
        std::complex<double> IC = std::polar(lastPhasor.cCMag, deg2rad(lastPhasor.cCPhaseDeg));

        // --- 대칭분 계산 (전압) ---
        std::complex<double> Vpos  = (VA + a * VB + a2 * VC) / 3.0;
        std::complex<double> Vneg  = (VA + a2 * VB + a * VC) / 3.0;
        std::complex<double> Vzero = (VA + VB + VC) / 3.0;

        std::complex<double> VLLpos  = (VALL + a * VBLL + a2 * VCLL) / 3.0;
        std::complex<double> VLLneg  = (VALL + a2 * VBLL + a * VCLL) / 3.0;
        std::complex<double> VLLzero = (VALL + VBLL + VCLL) / 3.0;


        // --- 대칭분 계산 (전류) ---
        std::complex<double> Ipos  = (IA + a * IB + a2 * IC) / 3.0;
        std::complex<double> Ineg  = (IA + a2 * IB + a * IC) / 3.0;
        std::complex<double> Izero = (IA + IB + IC) / 3.0;

        // --- 크기 계산 ---
        double Vpos_mag = std::abs(Vpos);
        double Vneg_mag = std::abs(Vneg);
        double Vzero_mag = std::abs(Vzero);

        //선간전압
        double Vpos_mag_LL = std::abs(VLLpos);
        double Vneg_mag_LL = std::abs(VLLneg);
        double Vzero_mag_LL = std::abs(VLLzero);


        double Ipos_mag = std::abs(Ipos);
        double Ineg_mag = std::abs(Ineg);
        double Izero_mag = std::abs(Izero);


        tableData->Vpos_mag = Vpos_mag;
        tableData->Vneg_mag = Vneg_mag;
        tableData->Vzero_mag = Vzero_mag;
        tableData->Ipos_mag = Ipos_mag;
        tableData->Ineg_mag = Ineg_mag;
        tableData->Izero_mag = Izero_mag;


        //============ NEMA Unbalance =============
        //A = 3상 RMS의 평균값
        //B = 3상 RMS 중 최대값
        //unbalance = ((B - A) / A) * 100.0
        //기본파 기준

        std::vector<double> v = { vFundA, vFundB, vFundC };
        std::vector<double> i = { iFundA, iFundB, iFundC };

        std::vector<double> vLL = {vFundALL, vFundBLL, vFundCLL};

        double vA = std::accumulate(v.begin(), v.end(), 0.0) / v.size(); // 평균값
        double vB = *std::max_element(v.begin(), v.end());               // 최대값

        double vUnbalance = ((vB - vA) / vA) * 100.0; // %

        double vALL = std::accumulate(vLL.begin(), vLL.end(), 0.0) / vLL.size(); //평균값
        double vBLL = *std::max_element(vLL.begin(), vLL.end());

        double vLLUnbalance = ((vBLL - vALL)/ vALL) * 100.0;

        double iA = std::accumulate(i.begin(), i.end(), 0.0) / i.size(); // 평균값
        double iB = *std::max_element(i.begin(), i.end());               // 최대값

        double iUnbalance = ((iB - iA) / iA) * 100.0; // %

        tableData->vA = vA;
        tableData->vB = vB;
        tableData->vUnbalance = vUnbalance;
        tableData->iA =  iA;
        tableData->iB =  iB;

        //=========== U0 , U2 ====================
        //U0 = Zero / Positive
        //U2 = negative / Positive

        double vU0 = 0.0;
        double vU2 = 0.0;


        if (Vpos_mag > 1e-4) {
            // 정상 계산
            vU0 = Vzero_mag / Vpos_mag;
            vU2 = Vneg_mag / Vpos_mag;
        } else {
            // 분모가 0 → 무한대로 설정
            vU0 = std::numeric_limits<double>::infinity();
            vU2 = std::numeric_limits<double>::infinity();
        }

        double iU0 = 0.0;
        double iU2 = 0.0;

        if (Ipos_mag > 1e-4) {
            // 정상 계산
            iU0 = Izero_mag / Ipos_mag;
            iU2 = Ineg_mag / Ipos_mag;
        } else {
            // 분모가 0 → 무한대로 설정
            iU0 = std::numeric_limits<double>::infinity();
            iU2 = std::numeric_limits<double>::infinity();
        }

        double vTotalAvg = (vTotalA + vTotalB + vTotalC) / 3.0 ;
        double vTotalAvgLL = (vTotalALL + vTotalBLL + vTotalCLL) /3.0;
        double vFundAvg = (vFundA + vFundB + vFundC) / 3.0 ;
        double vFundAvgLL = (vFundALL + vFundBLL + vFundCLL) /3.0;
        double iTotalAvg = (iTotalA + iTotalB + iTotalC)/ 3.0;
        double iFundAvg = (iFundA + iFundB + iFundC) / 3.0 ;

        tableData->vU0 = vU0;
        tableData->vU2 = vU2;
        tableData->iU0 = iU0;
        tableData->iU2 = iU2;




        //================ MIN/MAX 값 =========================================================

        QString now = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");


        //---------상전압 (LN) RMS Max 값-------------
        mm.vTotalAMM.update(vTotalA, now);
        mm.vTotalBMM.update(vTotalB, now);
        mm.vTotalCMM.update(vTotalC, now);
        mm.vTotalAvgMM.update(vTotalAvg, now);

        mm.vTotalALLMM.update(vTotalALL, now);
        mm.vTotalBLLMM.update(vTotalBLL, now);
        mm.vTotalCLLMM.update(vTotalCLL, now);
        mm.vTotalAvgLLMM.update(vTotalAvgLL, now);


        mm.vFundAMM.update(vFundA, now);
        mm.vFundBMM.update(vFundB, now);
        mm.vFundCMM.update(vFundC, now);
        mm.vFundAvgMM.update(vFundAvg, now);

        mm.vFundALLMM.update(vFundALL, now);
        mm.vFundBLLMM.update(vFundBLL, now);
        mm.vFundCLLMM.update(vFundCLL, now);
        mm.vFundAvgLLMM.update(vFundAvgLL, now);


        mm.thdVAMM.update(thdVA, now);
        mm.thdVBMM.update(thdVB, now);
        mm.thdVCMM.update(thdVC, now);

        mm.thdVALLMM.update(thdVALL, now);
        mm.thdVBLLMM.update(thdVBLL, now);
        mm.thdVCLLMM.update(thdVCLL, now);

        mm.freqMM.update(dataMng.getCyclePerSecond(), now);

        mm.vResidualRMSMM.update(vResidualRMS, now);

        mm.iTotalAMM.update(iTotalA, now);
        mm.iTotalBMM.update(iTotalB, now);
        mm.iTotalCMM.update(iTotalC, now);
        mm.iTotalAvgMM.update(iTotalAvg, now);

        mm.iFundAMM.update(iFundA, now);
        mm.iFundBMM.update(iFundB, now);
        mm.iFundCMM.update(iFundC, now);
        mm.iFundAvgMM.update(iFundAvg, now);

        mm.thdIAMM.update(thdIA, now);
        mm.thdIBMM.update(thdIB, now);
        mm.thdICMM.update(thdIC, now);

        mm.iResidualRMSMM.update(iResidualRMS, now);

        mm.pAMM.update(pA, now);
        mm.pBMM.update(pB, now);
        mm.pCMM.update(pC, now);
        mm.pTotalMM.update(pTotal, now);

        mm.qAMM.update(qA, now);
        mm.qBMM.update(qB, now);
        mm.qCMM.update(qC, now);
        mm.qSumMM.update(qSum, now);

        mm.sAMM.update(sA, now);
        mm.sBMM.update(sB, now);
        mm.sCMM.update(sC, now);
        mm.sSumMM.update(sSum, now);

        mm.pfAMM.update(pfA, now);
        mm.pfBMM.update(pfB, now);
        mm.pfCMM.update(pfC, now);
        mm.pfSumMM.update(pfSum, now);

        mm.Vpos_magMM.update(Vpos_mag, now);
        mm.Vneg_magMM.update(Vneg_mag, now);
        mm.Vzero_magMM.update(Vzero_mag, now);

        mm.Vpos_mag_LLMM.update(Vpos_mag_LL, now);
        mm.Vneg_mag_LLMM.update(Vneg_mag_LL, now);
        mm.Vzero_mag_LLMM.update(Vzero_mag_LL, now);

        mm.Ipos_magMM.update(Ipos_mag, now);
        mm.Ineg_magMM.update(Ineg_mag, now);
        mm.Izero_magMM.update(Izero_mag, now);

        mm.vLLUnbalanceMM.update(vLLUnbalance, now);
        mm.vUnbalanceMM.update(vUnbalance, now);
        mm.vU2MM.update(vU2, now);
        mm.vU0MM.update(vU0, now);

        mm.iUnbalanceMM.update(iUnbalance, now);
        mm.iU2MM.update(iU2, now);
        mm.iU0MM.update(iU0, now);


        //==================================================================================================
        //A3700 gui에 보내는 값.
        //-------- voltage -------------
        //RMS
        oneSecData->voltage.RMS = { vTotalA, vTotalB, vTotalC, vTotalAvg};
        oneSecData->voltage.RMS_MAX = {mm.vTotalAMM.max, mm.vTotalBMM.max, mm.vTotalCMM.max, mm.vTotalAvgMM.max};
        oneSecData->voltage.RMS_MAX_Time = {mm.vTotalAMM.maxTime, mm.vTotalBMM.maxTime, mm.vTotalCMM.maxTime, mm.vTotalAvgMM.maxTime};
        oneSecData->voltage.RMS_MIN = {mm.vTotalAMM.min, mm.vTotalBMM.min, mm.vTotalCMM.min, mm.vTotalAvgMM.min};
        oneSecData->voltage.RMS_MIN_Time = {mm.vTotalAMM.minTime, mm.vTotalBMM.minTime, mm.vTotalCMM.minTime, mm.vTotalAvgMM.minTime};

        // Fundamental
        oneSecData->voltage.Fundamental = { vFundA, vFundB, vFundC, vFundAvg};
        oneSecData->voltage.Fundamental_MAX = { mm.vFundAMM.max, mm.vFundBMM.max, mm.vFundCMM.max, mm.vFundAvgMM.max};
        oneSecData->voltage.Fundamental_MAX_Time = { mm.vFundAMM.maxTime, mm.vFundBMM.maxTime, mm.vFundCMM.maxTime, mm.vFundAvgMM.maxTime};
        oneSecData->voltage.Fundamental_MIN = { mm.vFundAMM.min, mm.vFundBMM.min, mm.vFundCMM.min, mm.vFundAvgMM.min};
        oneSecData->voltage.Fundamental_MIN_Time = { mm.vFundAMM.minTime, mm.vFundBMM.minTime, mm.vFundCMM.minTime, mm.vFundAvgMM.minTime};

        // THD (%)
        oneSecData->voltage.THD = { thdVA, thdVB, thdVC };
        oneSecData->voltage.THD_MAX = { mm.thdVAMM.max, mm.thdVBMM.max, mm.thdVCMM.max };
        oneSecData->voltage.THD_MAX_Time = { mm.thdVAMM.maxTime,mm. thdVBMM.maxTime, mm.thdVCMM.maxTime };

        //frequency
        oneSecData->voltage.Frequency = dataMng.getCyclePerSecond();
        oneSecData->voltage.Frequency_MAX = mm.freqMM.max;
        oneSecData->voltage.Frequency_MAX_Time = mm.freqMM.maxTime;
        oneSecData->voltage.Frequency_MIN = mm.freqMM.min;
        oneSecData->voltage.Frequency_MIN_Time = mm.freqMM.minTime;

        //Residual
        oneSecData->voltage.Residual = vResidualRMS;
        oneSecData->voltage.Residual_MAX = mm.vResidualRMSMM.max;
        oneSecData->voltage.Residual_MAX_Time = mm.vResidualRMSMM.maxTime;
        oneSecData->voltage.Residual_MIN = mm.vResidualRMSMM.min;
        oneSecData->voltage.Residual_MIN_Time = mm.vResidualRMSMM.minTime;

        //-------- Current -------------
        //RMS
        oneSecData->current.RMS = {iTotalA, iTotalB, iTotalC, iTotalAvg};
        oneSecData->current.RMS_MAX = {mm.iTotalAMM.max, mm.iTotalBMM.max, mm.iTotalCMM.max, mm.iTotalAvgMM.max};
        oneSecData->current.RMS_MAX_Time = {mm.iTotalAMM.maxTime, mm.iTotalBMM.maxTime, mm.iTotalCMM.maxTime, mm.iTotalAvgMM.maxTime};
        oneSecData->current.RMS_MIN = {mm.iTotalAMM.min, mm.iTotalBMM.min, mm.iTotalCMM.min, mm.iTotalAvgMM.min};
        oneSecData->current.RMS_MIN_Time = {mm.iTotalAMM.minTime, mm.iTotalBMM.minTime, mm.iTotalCMM.minTime, mm.iTotalAvgMM.minTime};

        // Fundamental
        oneSecData->current.Fundamental = { iFundA, iFundB, iFundC, iFundAvg};
        oneSecData->current.Fundamental_MAX = { mm.iFundAMM.max, mm.iFundBMM.max, mm.iFundCMM.max, mm.iFundAvgMM.max};
        oneSecData->current.Fundamental_MAX_Time = { mm.iFundAMM.maxTime, mm.iFundBMM.maxTime, mm.iFundCMM.maxTime, mm.iFundAvgMM.maxTime};
        oneSecData->current.Fundamental_MIN = { mm.iFundAMM.min, mm.iFundBMM.min, mm.iFundCMM.min, mm.iFundAvgMM.min};
        oneSecData->current.Fundamental_MIN_Time = { mm.iFundAMM.minTime, mm.iFundBMM.minTime, mm.iFundCMM.minTime, mm.iFundAvgMM.minTime};

        // THD (%)
        oneSecData->current.THD = { thdIA, thdIB, thdIC };
        oneSecData->current.THD_MAX = { mm.thdIAMM.max, mm.thdIBMM.max, mm.thdICMM.max };
        oneSecData->current.THD_MAX_Time = { mm.thdIAMM.maxTime, mm.thdIBMM.maxTime, mm.thdICMM.maxTime };

        //Residual
        oneSecData->current.Residual = iResidualRMS;
        oneSecData->current.Residual_MAX = mm.iResidualRMSMM.max;
        oneSecData->current.Residual_MAX_Time = mm.iResidualRMSMM.maxTime;
        oneSecData->current.Residual_MIN = mm.iResidualRMSMM.min;
        oneSecData->current.Residual_MIN_Time = mm.iResidualRMSMM.minTime;

        //--------   power   -------------
        //Active
        oneSecData->power.active = {pA/1000.0, pB/1000.0, pC/1000.0, pTotal/1000.0};
        oneSecData->power.active_MAX = {mm.pAMM.max/1000.0, mm.pBMM.max/1000.0, mm.pCMM.max/1000.0, mm.pTotalMM.max/1000.0};
        oneSecData->power.active_MAX_Time = {mm.pAMM.maxTime, mm.pBMM.maxTime, mm.pCMM.maxTime, mm.pTotalMM.maxTime};
        oneSecData->power.active_MIN = {mm.pAMM.min/1000.0, mm.pBMM.min/1000.0, mm.pCMM.min/1000.0, mm.pTotalMM.min/1000.0};
        oneSecData->power.active_MIN_Time = {mm.pAMM.minTime, mm.pBMM.minTime, mm.pCMM.minTime, mm.pTotalMM.minTime};

        //Reactive
        oneSecData->power.reactive = {qA/1000.0, qB/1000.0,qC/1000.0, qSum/1000.0};
        oneSecData->power.reactive_MAX = {mm.qAMM.max/1000.0, mm.qBMM.max/1000.0,mm.qCMM.max/1000.0, mm.qSumMM.max/1000.0};
        oneSecData->power.reactive_MAX_Time = {mm.qAMM.maxTime, mm.qBMM.maxTime, mm.qCMM.maxTime, mm.qSumMM.maxTime};
        oneSecData->power.reactive_MIN = {mm.qAMM.min/1000.0, mm.qBMM.min/1000.0,mm.qCMM.min/1000.0, mm.qSumMM.min/1000.0};
        oneSecData->power.reactive_MIN_Time = {mm.qAMM.minTime, mm.qBMM.minTime, mm.qCMM.minTime, mm.qSumMM.minTime};

        //Apparent
        oneSecData->power.apparent = {sA/1000.0, sB/1000.0,sC/1000.0, sSum/1000.0};
        oneSecData->power.apparent_MAX = {mm.sAMM.max/1000.0, mm.sBMM.max/1000.0,mm.sCMM.max/1000.0, mm.sSumMM.max/1000.0};
        oneSecData->power.apparent_MAX_Time = {mm.sAMM.maxTime, mm.sBMM.maxTime,mm.sCMM.maxTime, mm.sSumMM.maxTime};
        oneSecData->power.apparent_MIN = {mm.sAMM.min/1000.0, mm.sBMM.min/1000.0,mm.sCMM.min/1000.0, mm.sSumMM.min/1000.0};
        oneSecData->power.apparent_MIN_Time = {mm.sAMM.minTime, mm.sBMM.minTime,mm.sCMM.minTime, mm.sSumMM.minTime};
        //PF
        oneSecData->power.pf = {pfA, pfB, pfC, pfSum};
        oneSecData->power.pf_MAX = {mm.pfAMM.max, mm.pfBMM.max, mm.pfCMM.max, mm.pfSumMM.max};
        oneSecData->power.pf_MAX_Time = {mm.pfAMM.maxTime, mm.pfBMM.maxTime, mm.pfCMM.maxTime, mm.pfSumMM.maxTime};
        oneSecData->power.pf_MIN = {mm.pfAMM.min, mm.pfBMM.min, mm.pfCMM.min, mm.pfSumMM.min};
        oneSecData->power.pf_MIN_Time = {mm.pfAMM.minTime, mm.pfBMM.minTime,mm. pfCMM.minTime, mm.pfSumMM.minTime};

        //Energy
        oneSecData->power.energy = energyWh/1000.0;

        //-------- symm Unbal -------------
        //volt. symm.
        oneSecData->symmUnbal.voltSymm = { Vpos_mag, Vneg_mag, Vzero_mag };
        oneSecData->symmUnbal.voltSymm_MAX = { mm.Vpos_magMM.max, mm.Vneg_magMM.max, mm.Vzero_magMM.max };
        oneSecData->symmUnbal.voltSymm_MAX_Time = { mm.Vpos_magMM.maxTime, mm.Vneg_magMM.maxTime, mm.Vzero_magMM.maxTime };

        //curr. symm.
        oneSecData->symmUnbal.currsymm = { Ipos_mag, Ineg_mag, Izero_mag };
        oneSecData->symmUnbal.currsymm_MAX = { mm.Ipos_magMM.max, mm.Ineg_magMM.max, mm.Izero_magMM.max };
        oneSecData->symmUnbal.currsymm_MAX_Time = { mm.Ipos_magMM.maxTime, mm.Ineg_magMM.maxTime, mm.Izero_magMM.maxTime };
        //volt. unbal.
        oneSecData->symmUnbal.voltUnbal = {vLLUnbalance, vUnbalance, vU2, vU0};
        oneSecData->symmUnbal.voltUnbal_MAX = {mm.vLLUnbalanceMM.max, mm.vUnbalanceMM.max, mm.vU2MM.max, mm.vU0MM.max};
        oneSecData->symmUnbal.voltUnbal_MAX_Time = {mm.vLLUnbalanceMM.maxTime, mm.vUnbalanceMM.maxTime, mm.vU2MM.maxTime, mm.vU0MM.maxTime};
        //curr. unbal.
        oneSecData->symmUnbal.currUnbal = {iUnbalance, iU2, iU0};
        oneSecData->symmUnbal.currUnbal_MAX = {mm.iUnbalanceMM.max, mm.iU2MM.max, mm.iU0MM.max};
        oneSecData->symmUnbal.currUnbal_MAX_Time = {mm.iUnbalanceMM.maxTime, mm.iU2MM.maxTime, mm.iU0MM.maxTime};

        //-------harmonic------------------
        oneSecData->harmonic.harmV = vHarm;
        oneSecData->harmonic.harmA = iHarm;

        //------선간 전압(line to line)------------
        oneSecData->voltage.RMS_LL = {vTotalALL, vTotalBLL, vTotalCLL, vTotalAvgLL};
        oneSecData->voltage.RMS_LL_MAX = {mm.vTotalALLMM.max, mm.vTotalBLLMM.max, mm.vTotalCLLMM.max, mm.vTotalAvgLLMM.max};
        oneSecData->voltage.RMS_LL_MAX_Time = {mm.vTotalALLMM.maxTime, mm.vTotalBLLMM.maxTime, mm.vTotalCLLMM.maxTime, mm.vTotalAvgLLMM.maxTime};
        oneSecData->voltage.RMS_LL_MIN = {mm.vTotalALLMM.min, mm.vTotalBLLMM.min, mm.vTotalCLLMM.min, mm.vTotalAvgLLMM.min};
        oneSecData->voltage.RMS_LL_MIN_Time = {mm.vTotalALLMM.minTime, mm.vTotalBLLMM.minTime, mm.vTotalCLLMM.minTime, mm.vTotalAvgLLMM.minTime};

        oneSecData->voltage.Fundamental_LL = {vFundALL, vFundBLL, vFundCLL, vFundAvgLL};
        oneSecData->voltage.Fundamental_LL_MAX = { mm.vFundALLMM.max, mm.vFundBLLMM.max, mm.vFundCLLMM.max, mm.vFundAvgLLMM.max};
        oneSecData->voltage.Fundamental_LL_MAX_Time = { mm.vFundALLMM.maxTime, mm.vFundBLLMM.maxTime, mm.vFundCLLMM.maxTime, mm.vFundAvgLLMM.maxTime};
        oneSecData->voltage.Fundamental_LL_MIN = { mm.vFundALLMM.min, mm.vFundBLLMM.min, mm.vFundCLLMM.min, mm.vFundAvgLLMM.min};
        oneSecData->voltage.Fundamental_LL_MIN_Time = { mm.vFundALLMM.minTime, mm.vFundBLLMM.minTime, mm.vFundCLLMM.minTime, mm.vFundAvgLLMM.minTime};

        oneSecData->voltage.THDLL = {thdVALL, thdVBLL, thdVCLL};
        oneSecData->voltage.THDLL_MAX = {mm.thdVALLMM.max, mm.thdVBLLMM.max, mm.thdVCLLMM.max};
        oneSecData->voltage.THDLL_MAX_Time = {mm.thdVALLMM.maxTime, mm.thdVBLLMM.maxTime, mm.thdVCLLMM.maxTime};

        oneSecData->symmUnbal.voltSymm_LL = {Vpos_mag_LL, Vneg_mag_LL, Vzero_mag_LL};
        oneSecData->symmUnbal.voltSymm_LL_MAX = {mm.Vpos_mag_LLMM.max, mm.Vneg_mag_LLMM.max, mm.Vzero_mag_LLMM.max};
        oneSecData->symmUnbal.voltSymm_LL_MAX_Time = {mm.Vpos_mag_LLMM.maxTime, mm.Vneg_mag_LLMM.maxTime, mm.Vzero_mag_LLMM.maxTime};

        // === 리셋 ===
        acc.prevTime = m.t;
        acc.oneSecSampleCnt = 0;
        acc.sumFundV2_A = acc.sumFundV2_B = acc.sumFundV2_C = 0.0;
        acc.sumFundI2_A = acc.sumFundI2_B = acc.sumFundI2_C = 0.0;
        acc.sumHarmV2 = acc.sumHarmI2 = 0.0;
        acc.sumTotalV2_A = acc.sumTotalV2_B = acc.sumTotalV2_C = 0.0;
        acc.sumTotalI2_A = acc.sumTotalI2_B = acc.sumTotalI2_C = 0.0;
        acc.sumPowerA = acc.sumPowerB = acc.sumPowerC = 0.0;
        acc.sumResidualV = acc.sumResidualI = 0.0;
        acc.sumFundV2_ALL = acc.sumFundV2_BLL = acc.sumFundV2_CLL = 0.0;
        acc.sumTotalV2_ALL = acc.sumTotalV2_BLL = acc.sumTotalV2_CLL = 0.0;
        acc.sumHarmAV2_LL = acc.sumHarmBV2_LL = acc.sumHarmCV2_LL = 0.0;

        emit oneSecTableChanged(tableData, oneSecData);
    }
}
