#include "phasorcalculator.h"
#include "dataStructures.h"
#include <complex>

phasorCalculator::phasorCalculator(dataManagement& dataMng)
    : dataMng(dataMng)
{

}

static inline double normDeg(double d){
    while (d > 180.0)  d -= 360.0;
    while (d <= -180.0) d += 360.0;
    return d;
}
static inline std::complex<double> rot90(const std::complex<double>& X){
    return std::complex<double>(-X.imag(), X.real());
}

// 위상/크기 계산
phasor_data phasorCalculator::calcPhasor() const {
    phasor_data ph{};

    const auto& data_queue = dataMng.data_queue;
    auto samplePerCycle = dataMng.getSamplePerCycle();

    const int Nset = samplePerCycle;
    if (Nset <= 0 || data_queue.empty())
        return ph;

    const size_t Sz = data_queue.size();
    const int N = std::min<int>({ Nset, (int)Sz });
    const size_t start = Sz - N;

    // --- 기본파(ABC) 누적기 ---
    std::complex<double> vAX(0,0), vBX(0,0), vCX(0,0);
    std::complex<double> cAX(0,0), cBX(0,0), cCX(0,0);
    std::complex<double> vALLX(0,0), vBLLX(0,0), vCLLX(0,0);//선간전압

    // --- 고조파(‘A상만’) 누적기 ---
    std::complex<double> hvX(0,0), hcX(0,0);

    for (int n = 0; n < N; n++) { // 한주기만큼 값을 누적
        const double th1 = -2.0 * M_PI * n / N;
        const std::complex<double> W1(std::cos(th1), std::sin(th1));

        const auto& s = data_queue[start + n];

        // 기본파: 3상 동시 누적 → 상간 위상 일관성 유지
        vAX += s.vAVal * W1;  vBX += s.vBVal * W1;  vCX += s.vCVal * W1;
        cAX += s.cAVal * W1;  cBX += s.cBVal * W1;  cCX += s.cCVal * W1;
        vALLX +=s.vAValLL * W1; vBLLX += s.vBValLL * W1; vCLLX +=s.vCValLL * W1;//선간전압

        // 고조파: ‘한 상만(A상)’ 누적
        const double thV = -2.0 * M_PI * dataMng.getVHarmonicOrder() * n / N;
        const double thC = -2.0 * M_PI * dataMng.getCHarmonicOrder() * n / N;
        if (dataMng.getVHarmonicMag() > 1e-9){
            hvX += s.vAVal * std::complex<double>(std::cos(thV), std::sin(thV));
        }
        if ((dataMng.getCHarmonicMag()) > 1e-9){
            hcX += s.cAVal * std::complex<double>(std::cos(thC), std::sin(thC));
        }
    }

    const double iN = 1.0 / (double)N;
    vAX *= iN; vBX *= iN; vCX *= iN;
    cAX *= iN; cBX *= iN; cCX *= iN;
    vALLX *= iN; vBLLX *= iN; vCLLX *= iN;//선간전압
    hvX *= iN; hcX *= iN;

    auto rz  = [&](const std::complex<double>& z){ return rot90(z); };
    auto mag = [](double x, double y){ return std::hypot(x,y) * std::sqrt(2.0); };
    auto ang = [&](double y, double x){ return normDeg(std::atan2(y, x) * 180.0 / M_PI); };

    // rot90 보정
    const auto vAZ = rz(vAX), vBZ = rz(vBX), vCZ = rz(vCX);
    const auto cAZ = rz(cAX), cBZ = rz(cBX), cCZ = rz(cCX);
    const auto hvZ = rz(hvX), hcZ = rz(hcX);
    const auto vALLZ = rz(vALLX), vBLLZ = rz(vBLLX),vCLLZ = rz(vCLLX); //선간전압

    // ===== 기본파 Voltage =====
    ph.vAReal = vAZ.real(); ph.vAImag = vAZ.imag();
    ph.vBReal = vBZ.real(); ph.vBImag = vBZ.imag();
    ph.vCReal = vCZ.real(); ph.vCImag = vCZ.imag();
    ph.vAMag  = mag(ph.vAReal, ph.vAImag); ph.vAPhaseDeg = ang(ph.vAImag, ph.vAReal);
    ph.vBMag  = mag(ph.vBReal, ph.vBImag); ph.vBPhaseDeg = ang(ph.vBImag, ph.vBReal);
    ph.vCMag  = mag(ph.vCReal, ph.vCImag); ph.vCPhaseDeg = ang(ph.vCImag, ph.vCReal);

    //===== 선간전압 Voltage =======
    ph.vALLReal = vALLZ.real(); ph.vALLImag = vALLZ.imag();
    ph.vBLLReal = vBLLZ.real(); ph.vBLLImag = vBLLZ.imag();
    ph.vCLLReal = vCLLZ.real(); ph.vCLLImag = vCLLZ.imag();
    ph.vALLMag =  mag(ph.vALLReal, ph.vALLImag); ph.vALLPhaseDeg = ang(ph.vALLImag, ph.vALLReal);
    ph.vBLLMag =  mag(ph.vBLLReal, ph.vBLLImag); ph.vBLLPhaseDeg = ang(ph.vBLLImag, ph.vBLLReal);
    ph.vCLLMag =  mag(ph.vCLLReal, ph.vCLLImag); ph.vCLLPhaseDeg = ang(ph.vCLLImag, ph.vCLLReal);

    // ===== 기본파 Current =====
    ph.cAReal = cAZ.real(); ph.cAImag = cAZ.imag();
    ph.cBReal = cBZ.real(); ph.cBImag = cBZ.imag();
    ph.cCReal = cCZ.real(); ph.cCImag = cCZ.imag();
    ph.cAMag  = mag(ph.cAReal, ph.cAImag); ph.cAPhaseDeg = ang(ph.cAImag, ph.cAReal);
    ph.cBMag  = mag(ph.cBReal, ph.cBImag); ph.cBPhaseDeg = ang(ph.cBImag, ph.cBReal);
    ph.cCMag  = mag(ph.cCReal, ph.cCImag); ph.cCPhaseDeg = ang(ph.cCImag, ph.cCReal);

    // ===== 고조파: ‘A상’만 =====
    ph.hvReal = hvZ.real(); ph.hvImag = hvZ.imag();
    ph.hcReal = hcZ.real(); ph.hcImag = hcZ.imag();
    ph.hvMag  = mag(ph.hvReal, ph.hvImag);
    ph.hvPhaseDeg = ang(ph.hvImag, ph.hvReal);
    ph.hcMag  = mag(ph.hcReal, ph.hcImag); ph.hcPhaseDeg = ang(ph.hcImag, ph.hcReal);
    ph.vOrderUsed = dataMng.getVHarmonicOrder();
    ph.cOrderUsed = dataMng.getCHarmonicOrder();

    return ph;
}
