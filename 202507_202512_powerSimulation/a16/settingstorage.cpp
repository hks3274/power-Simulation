#include "settingstorage.h"
#include <qdebug.h>

//db 초기화
settingStorage::settingStorage(const std::string& dbPath)
    : databasePath(dbPath)
{
    bool needsRecreation = false;

    try {
        sqlite::database db(databasePath);

        // 현재 DB 버전 확인
        int currentVersion = 0;
        db << "PRAGMA user_version;" >> currentVersion;

        if (currentVersion == 0) {
            // 첫 실행이거나 마이그레이션이 필요한 경우만 실행
            qDebug() << "[첫 실행] DB 초기화 시작";
            needsRecreation = true;
        } else {
            // 기존 DB가 있는 경우 테이블 구조 검증
            try {
                db << "SELECT vAValue, iAValue, iAPhaseDeg, "
                      "vBValue, iBValue, iBPhaseDeg, "
                      "vCValue, iCValue, iCPhaseDeg, "
                      "frequency, timeScale, cyclePerSecond, samplePerCycle, graphWidth, displayUpdateMode, "
                      "vHarmonicOrder, vHarmonicMag, vHarmonicPhase, "
                      "cHarmonicOrder, cHarmonicMag, cHarmonicPhase "
                      "FROM settings WHERE id=1 LIMIT 1;";
                qDebug() << "[기존 DB 사용] user_version=" << currentVersion;
            } catch (sqlite::sqlite_exception& e) {
                qDebug() << "[DB 구조 오류 감지] 재생성 필요:" << e.what();
                needsRecreation = true;
            }
        }

        if (needsRecreation) {
            createNewDatabase(db);
        }
    }
    catch (sqlite::sqlite_exception& e) {
        qDebug() << "[DB 초기화 오류]:" << e.what() << " - 강제 재생성 시도";

        // 오류 발생 시 강제로 새 DB 생성
        try {
            sqlite::database db(databasePath);
            createNewDatabase(db);
        } catch (sqlite::sqlite_exception& e2) {
            qDebug() << "[DB 강제 재생성 실패]:" << e2.what();
        }
    }
}

void settingStorage::createNewDatabase(sqlite::database& db)
{
    try {
        db << "BEGIN IMMEDIATE;";

        // 기존 테이블 완전 삭제
        db << "DROP TABLE IF EXISTS settings;";
        db << "DROP TABLE IF EXISTS settings_new;";

        // 새 테이블 생성
        db << "CREATE TABLE settings ("
              "id INTEGER PRIMARY KEY,"
              // --- 3상 전압/전류/위상 ---
              "vAValue REAL DEFAULT 300.0,"
              "iAValue REAL DEFAULT 10.0,"
              "iAPhaseDeg INTEGER DEFAULT 0,"
              "vBValue REAL DEFAULT 300.0,"
              "vBPhaseDeg INTEGER DEFAULT -120,"
              "iBValue REAL DEFAULT 10.0,"
              "iBPhaseDeg INTEGER DEFAULT -120,"
              "vCValue REAL DEFAULT 300.0,"
              "vCPhaseDeg INTEGER DEFAULT 120,"
              "iCValue REAL DEFAULT 10.0,"
              "iCPhaseDeg INTEGER DEFAULT 120,"
              // --- 기타 ---
              "frequency REAL DEFAULT 20.0,"
              "timeScale INTEGER DEFAULT 5,"
              "cyclePerSecond REAL DEFAULT 20.0,"
              "samplePerCycle INTEGER DEFAULT 20,"
              "graphWidth REAL DEFAULT 0.5,"
              "displayUpdateMode TEXT DEFAULT 'FullCycle',"
              "vHarmonicOrder INTEGER DEFAULT 1,"
              "vHarmonicMag REAL DEFAULT 0.0,"
              "vHarmonicPhase INTEGER DEFAULT 0,"
              "cHarmonicOrder INTEGER DEFAULT 1,"
              "cHarmonicMag REAL DEFAULT 0.0,"
              "cHarmonicPhase INTEGER DEFAULT 0"
              ");";

        // 기본값 삽입
        db << "INSERT INTO settings (id) VALUES (1);";

        // 버전 설정
        db << "PRAGMA user_version=2;";

        db << "COMMIT;";
        qDebug() << "[DB 생성 완료] user_version=2";

    } catch (sqlite::sqlite_exception& e) {
        try {
            db << "ROLLBACK;";
        } catch (...) {}
        throw; // 오류를 다시 던져서 상위에서 처리
    }
}

//세이브 시 update
void settingStorage::save(dataManagement& dataMng)
{
    try {
        sqlite::database db(databasePath);

        // 아이디를 똑같이 해서 마지막 값만 저장..
        db << "UPDATE settings SET "
              "vAValue=?, iAValue=?, iAPhaseDeg=?, "
              "vBValue=?, vBPhaseDeg=?, iBValue=?, iBPhaseDeg=?, "
              "vCValue=?, vCPhaseDeg=?, iCValue=?, iCPhaseDeg=?, "
              "frequency=?, timeScale=?, cyclePerSecond=?, samplePerCycle=?, graphWidth=?, displayUpdateMode=?, "
              "vHarmonicOrder=?, vHarmonicMag=?, vHarmonicPhase=?, "
              "cHarmonicOrder=?, cHarmonicMag=?, cHarmonicPhase=? "
              "WHERE id=1;"
           << dataMng.getVAValue()
           << dataMng.getIAValue()
           << dataMng.getIAPhaseDeg()
           << dataMng.getVBValue()
           << dataMng.getVBPhaseDeg()
           << dataMng.getIBValue()
           << dataMng.getIBPhaseDeg()
           << dataMng.getVCValue()
           << dataMng.getVCPhaseDeg()
           << dataMng.getICValue()
           << dataMng.getICPhaseDeg()
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
              }()
           << dataMng.getVHarmonicOrder()
           << dataMng.getVHarmonicMag()
           << dataMng.getVHarmonicPhase()
           << dataMng.getCHarmonicOrder()
           << dataMng.getCHarmonicMag()
           << dataMng.getCHarmonicPhase();

        qDebug() << "[설정 저장 완료]";

    } catch (sqlite::sqlite_exception& e) {
        qDebug() << "[설정 저장 오류]:" << e.what() << " - DB 재생성 시도";

        // 저장 오류 시 DB 재생성 후 다시 시도
        try {
            sqlite::database db(databasePath);
            createNewDatabase(db);

            // 재생성 후 다시 저장 시도
            save(dataMng);

        } catch (sqlite::sqlite_exception& e2) {
            qDebug() << "[설정 저장 재시도 실패]:" << e2.what();
        }
    }
}

//불러올 때 select
void settingStorage::load(dataManagement& dataMng)
{
    try {
        sqlite::database db(databasePath);

        //아이디를 불러온다.
        db << "SELECT vAValue, iAValue, iAPhaseDeg, "
              "vBValue, vBPhaseDeg, iBValue, iBPhaseDeg, "
              "vCValue, vCPhaseDeg, iCValue, iCPhaseDeg, "
              "frequency, timeScale, cyclePerSecond, samplePerCycle, graphWidth, displayUpdateMode, "
              "vHarmonicOrder, vHarmonicMag, vHarmonicPhase, "
              "cHarmonicOrder, cHarmonicMag, cHarmonicPhase "
              "FROM settings WHERE id=1;"
            >> [&](double vA, double iA, int phA,
                   double vB, int vBph, double iB, int iBph,
                   double vC, int vCph, double iC, int iCph,
                   double freq, int scale, double cps, int sps,
                   double width, std::string mode,
                   int vOrd, int vMag, int vPhs, int cOrd, int cMag, int cPhs) {

                  dataMng.setVAValue(vA);
                  dataMng.setIAValue(iA);
                  dataMng.setIAPhaseDeg(phA);

                  dataMng.setVBValue(vB);
                  dataMng.setVBPhaseDeg(vBph);
                  dataMng.setIBValue(iB);
                  dataMng.setIBPhaseDeg(iBph);

                  dataMng.setVCValue(vC);
                  dataMng.setVCPhaseDeg(vCph);
                  dataMng.setICValue(iC);
                  dataMng.setICPhaseDeg(iCph);

                  dataMng.setInputFreq(freq);
                  dataMng.setTimeScale(scale);
                  dataMng.setCyclePerSecond(cps);
                  dataMng.setSamplePerCycle(sps);
                  dataMng.setGraphWidth(width);

                  if (mode == "OneSample") dataMng.setUpdateMode(updateMode::OneSample);
                  else if (mode == "HalfCycle") dataMng.setUpdateMode(updateMode::HalfCycle);
                  else if (mode == "FullCycle") dataMng.setUpdateMode(updateMode::FullCycle);

                  dataMng.setVHarmonicOrder(vOrd);
                  dataMng.setVHarmonicMag(vMag);
                  dataMng.setVHarmonicPhase(vPhs);
                  dataMng.setCHarmonicOrder(cOrd);
                  dataMng.setCHarmonicMag(cMag);
                  dataMng.setCHarmonicPhase(cPhs);

                  qDebug() << "[설정 불러오기 완료]";
              };

    } catch (sqlite::sqlite_exception& e) {
        qDebug() << "[설정 불러오기 오류]:" << e.what() << " - DB 재생성 후 기본값 사용";

        // 불러오기 오류 시 DB 재생성
        try {
            sqlite::database db(databasePath);
            createNewDatabase(db);
            qDebug() << "[DB 재생성 완료] 기본값으로 설정됨";

        } catch (sqlite::sqlite_exception& e2) {
            qDebug() << "[설정 불러오기 재시도 실패]:" << e2.what();
        }
    }
}
