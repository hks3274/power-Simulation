#include "settingstorage.h"
#include "sqlite_modern_cpp.h"
#include <qdebug.h>

//db 초기화
settingStorage::settingStorage(const std::string& dbPath)
    :databasePath(dbPath)
{
    try {
        sqlite::database db(databasePath);

        int ver = 0;
        db << "PRAGMA user_version;" >> ver;

        if (ver < 2) {
            db << "BEGIN IMMEDIATE;";

            // 1) 새 테이블 생성: cyclePerSecond 를 REAL 로, 나머지는 기존과 동일
            db << "CREATE TABLE IF NOT EXISTS settings_new ("
                  "id INTEGER PRIMARY KEY,"
                  "voltageVal REAL,"
                  "currentVal REAL,"
                  "currentPhase INTEGER,"
                "frequency REAL,"
                "timeScale INTEGER,"
                "cyclePerSecond REAL,"
                "samplePerCycle INTEGER,"
                "graphWidth REAL,"
                "displayUpdateMode TEXT);";

            // 2) 기존 데이터 복사 (없으면 건너뜀)
            //    타입은 SQLite가 알아서 변환. CAST를 원하면 CAST(cyclePerSecond AS REAL)로 써도 됨.
            db << "INSERT OR IGNORE INTO settings (id) VALUES (1);"; // 안전망
            db << "INSERT INTO settings_new "
                  "(id, voltageVal, currentVal, currentPhase, frequency, timeScale, "
                  " cyclePerSecond, samplePerCycle, graphWidth, displayUpdateMode) "
                  "SELECT id, voltageVal, currentVal, currentPhase, frequency, timeScale, "
                  "       cyclePerSecond, samplePerCycle, graphWidth, displayUpdateMode "
                  "FROM settings WHERE id=1;";

            // 3) 기존 테이블 교체
            db << "DROP TABLE settings;";
            db << "ALTER TABLE settings_new RENAME TO settings;";

            // 4) 기본행 보장 (비어있다면)
            db << "INSERT OR IGNORE INTO settings "
                  "(id, voltageVal, currentVal, currentPhase, frequency, timeScale, "
                  " cyclePerSecond, samplePerCycle, graphWidth, displayUpdateMode) "
                  "VALUES (1, 300.0, 10.0, 60, 60.0, 5, 20.0, 20, 0.5, 'OneSample');";

            // 5) 버전 올리기
            db << "PRAGMA user_version=2;";

            db << "COMMIT;";
            qDebug() << "[DB 마이그레이션 완료] user_version=2";
        }

    } catch (sqlite::sqlite_exception& e) {
        try { sqlite::database db(databasePath); db << "ROLLBACK;"; } catch (...) {}
        qDebug() << "[DB 마이그레이션 오류]:" << e.what();
    }
}


//세이브 시 update
void settingStorage::save(dataManagement& dataMng)
{
    try {
        sqlite::database db(databasePath);

        // 아이디를 똑같이 해서 마지막 값만 저장..
        db << "UPDATE settings SET "
              "voltageVal=?, currentVal=?, currentPhase=?, frequency=?, timeScale=?, "
              "cyclePerSecond=?, samplePerCycle=?, graphWidth=?, displayUpdateMode=? "
              "WHERE id=1;"
           << dataMng.getInputValue()
           << dataMng.getCurrentValue()
           << dataMng.getCurrentPhaseDeg()
           << dataMng.getInputFreq()
           << dataMng.getTimeScale()
           << dataMng.getCyclePerSecond()
           << dataMng.getSamplePerCycle()
           << dataMng.getGraphWidth()
           << [&]() {
                  switch (dataMng.getUpdateMode()) {
                  case updateMode::OneSample: return "OneSample";
                  case updateMode::HalfCycle: return "HalfCycle";
                  case updateMode::FullCycle: return "FullCycle";
                  default: return "OneSample";
                  }
              }();

        qDebug() << "[설정 저장 완료]";


    } catch (sqlite::sqlite_exception& e) {
        qDebug() << "[설정 저장 오류]:" << e.what();
    }
}


//불러올 때 select
void settingStorage::load(dataManagement& dataMng)
{
    try {
        sqlite::database db(databasePath);

        //아이디를 불러온다.
        db << "SELECT voltageVal, currentVal, currentPhase, frequency, timeScale, "
              "cyclePerSecond, samplePerCycle, graphWidth, displayUpdateMode "
              "FROM settings WHERE id=1;"
            >> [&](double voltage, double current, int phase, double freq, int scale,
                   double cps, int sps, double width, std::string mode) {

                  dataMng.setInputValue(voltage);
                  dataMng.setCurrentValue(current);
                  dataMng.setCurrentPhaseDeg(phase);
                  dataMng.setInputFreq(freq);
                  dataMng.setTimeScale(scale);
                  dataMng.setGraphWidth(width);
                  dataMng.setCyclePerSecond(cps);
                  dataMng.setSamplePerCycle(sps);

                  if (mode == "OneSample") dataMng.setUpdateMode(updateMode::OneSample);
                  else if (mode == "HalfCycle") dataMng.setUpdateMode(updateMode::HalfCycle);
                  else if (mode == "FullCycle") dataMng.setUpdateMode(updateMode::FullCycle);

                  qDebug() << "[설정 불러오기 완료]";
              };

    } catch (sqlite::sqlite_exception& e) {
        qDebug() << "[설정 불러오기 오류]:" << e.what();
    }
}
