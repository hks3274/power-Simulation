#include "frequencytracker.h"
#include <QDebug>

frequencytracker::frequencytracker(dataManagement& dataMng)
    : dataMng(dataMng)
{
    connect(&dataMng, &dataManagement::rmsDataChanged,
            this, &frequencytracker::onRms);
}

void frequencytracker::start() {
    running = true;
    first = true;
    lastZcTime = -1.0;
    zcCount = 0;
    qDebug() << "[Tracker] 시작 ";
}

void frequencytracker::stop() {
    running = false;
    qDebug() << "[Tracker] 중지";
}

void frequencytracker::onRms(dataManagement::measure_data m) {
    if(!running) return;

    // --- dt 계산 (RMS 창 타임스탬프 이용) ---
    static double last_t = -1.0;
    const double t_now = std::chrono::duration<double>(m.t).count();
    const double dt = (last_t >= 0.0) ? (t_now - last_t) : 0.0;// 빌드 최적화 테스트
    last_t = t_now;

    auto ph = dataMng.calcPhasor();
    double phaseNow = qDegreesToRadians(ph.vPhaseDeg);

    if(first) { //첫번째에서는 위상을 구할 수 없기에 위상값을 넣고 리턴
        prevPhaseRad = phaseNow;
        first = false;
        return;
    }

    // === 1순위: 주파수 추적 ===
    double dphi = phaseNow - prevPhaseRad; //두번째부터 위상 값 차이 확인 -> 추적
    // 위상을 [-π, π] 범위로 정규화
    while (dphi > M_PI) dphi -= 2*M_PI;
    while (dphi < -M_PI) dphi += 2*M_PI;
    prevPhaseRad = phaseNow;


    double T_win = 1.0 / dataMng.getCyclePerSecond();
    double w_drift = dphi / T_win; // w_drift = Δθ/t
    double df = w_drift / (2 * M_PI);

    double fs_current = dataMng.getCyclePerSecond();

    // 주파수 보정
    const double Kp_freq = 0.4;   // P 게인
    const double Ki_freq = 0.009;   // I 게인
    const double Kd_freq = 0.002;  // D 게인


    static double freqIntegral = 0.0;  //누적 적분항
    static double df_prev = 0.0; //이전 df

    // I제어 항 (적분 누적)
    if (dt > 0.0) {
        freqIntegral += df * dt;                  // ∫df dt
        freqIntegral = std::clamp(freqIntegral, -1.0, 1.0); //일정량(일정 수치)만큼 증가(감소)하면 더이상 증가(감소) 하지 않는다.
    }
    const double ddf = (dt > 0.0) ? (df -df_prev) / dt : 0.0; // D
    df_prev = df;


    // 최종 보정
    double fs_new = fs_current
                    + Kp_freq * df
                    + Ki_freq * freqIntegral
                    + Kd_freq * ddf;

    // === 2순위: ZC 기반 위상 정렬 ===
    double currentZcTime = dataMng.getLastZcSec(); //최근의 ZC(zero crossing) 시간을 가져옴

    if(currentZcTime > 0 && currentZcTime != lastZcTime) {        // 새로운 ZC 검출될 때
        lastZcTime = currentZcTime;
        zcCount++;

        // ZC에서의 전압 위상을 확인
        auto ph_at_zc = dataMng.calcPhasor();
        double phase_at_zc = qDegreesToRadians(ph_at_zc.vPhaseDeg);

        // 위상을 [-π, π] 범위로 정규화
        while (phase_at_zc > M_PI) phase_at_zc -= 2*M_PI;
        while (phase_at_zc <= -M_PI) phase_at_zc += 2*M_PI;

        // ZC에서 위상이 0도가 되도록 보정
        // positive zero-crossing(- → +)에서는 위상이 0도여야 함
        // 위상오차(rad) → 주파수 등가(Hz)
        const double df_eq = phase_at_zc / (2.0 * M_PI * T_win);

        // 위상 오차를 주파수 보정에 반영
        const double Kp_phase = 0.4;  // P게인
        const double Ki_phase = 0.009;  // I게인
        const double Kd_phase = 0.002; // D게인


        // 위상 적분 상태
        static double phaseIntegralHz = 0.0;  // 누적 적분 항
        static double df_eq_prev = 0.0; //이전의 df값

        if (dt > 0.0) {
            phaseIntegralHz += df_eq * dt;                 // ∫df_eq dt
            phaseIntegralHz = std::clamp(phaseIntegralHz, -1.0, 1.0);
        }
        const double ddf_eq = (dt > 0.0) ? (df_eq - df_eq_prev) / dt : 0.0; // D
        df_eq_prev = df_eq;

        // P + I를 주파수 보정에 반영
        fs_new += Kp_phase * df_eq
                  + Ki_phase * phaseIntegralHz
                  + Kd_phase * ddf_eq;

        fs_new = std::round(fs_new * 1000.0) / 1000.0;

        qDebug().noquote()
            << "[ZC] phase_err(deg)=" << qRadiansToDegrees(phase_at_zc)
            << " df_eq=" << df_eq;
    }

    emit cpsChanged(fs_new);
    dataMng.setCyclePerSecond(fs_new);

    qDebug() << "[Tracker] fs 보정 ->" << fs_new
             << ", 주파수보정:" << (Kp_freq * df)
             << ", ZC개수:" << zcCount;
}
