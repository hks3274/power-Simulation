#include "settingstorage.h"
#include "sqlite_modern_cpp.h"
#include <qdebug.h>

//db 초기화
settingStorage::settingStorage(const std::string& dbPath)
    :databasePath(dbPath)
{
    try {
        sqlite::database db(databasePath);

        //테이블 생성 setting
        db << "CREATE TABLE IF NOT EXISTS settings ("
              "id INTEGER PRIMARY KEY,"
              "voltageVal REAL,"
              "currentVal REAL,"
              "currentPhase REAL,"
              "frequency INTEGER,"
              "timeScale INTEGER,"
              "cyclePerSecond INTEGER,"
              "samplePerCycle INTEGER,"
              "graphWidth REAL,"
              "displayUpdateMode TEXT);";

        // 처음 실행 시 초기행 삽입
        //insert or ignore =>settings 테이블에 id = 1인 레코드가 이미 있다면 아무 것도 하지 않고 무시한다
        db << "INSERT OR IGNORE INTO settings (id, voltageVal, currentVal, currentPhase, frequency, timeScale, "
              "cyclePerSecond, samplePerCycle, graphWidth, displayUpdateMode) "
              "VALUES (1, 100.0, 100.0, 60.0, 60, 1, 1, 10, 10.0, 'OneSample');";

    } catch (sqlite::sqlite_exception& e) {
        qDebug() << "[DB 초기화 오류]" << e.what();
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
            >> [&](double voltage, double current, double phase, int freq, int scale,
                   int cps, int sps, double width, std::string mode) {

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
