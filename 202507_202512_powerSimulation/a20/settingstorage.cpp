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
                db << "SELECT saveName, vAValue, iAValue, iAPhaseDeg, "
                      "vBValue, vBPhaseDeg, iBValue, iBPhaseDeg, "
                      "vCValue, vCPhaseDeg, iCValue, iCPhaseDeg, "
                      "frequency, timeScale, cyclePerSecond, samplePerCycle, graphWidth, displayUpdateMode, "
                      "vHarmonicOrder, vHarmonicMag, vHarmonicPhase, "
                      "cHarmonicOrder, cHarmonicMag, cHarmonicPhase "
                      "FROM settings LIMIT 1;";
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
        // 새 테이블 생성
        db << "CREATE TABLE settings ("
              "id INTEGER PRIMARY KEY AUTOINCREMENT,"
              "saveName TEXT UNIQUE,"
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
        db << "INSERT INTO settings (id, saveName) VALUES (1, '기본값');";

        // 버전 설정
        db << "PRAGMA user_version=3;";

        db << "COMMIT;";
        qDebug() << "[DB 생성 완료] user_version=3";


    } catch (sqlite::sqlite_exception& e) {
        try {
            db << "ROLLBACK;";
        } catch (...) {}
        throw; // 오류를 다시 던져서 상위에서 처리
    }
}

//저장
void settingStorage::save(const std::string& saveName, dataManagement& dataMng)
{
    if (saveName.empty()) {
        qDebug() << "[설정 저장 실패] 저장 이름이 비어 있습니다.";
        return;
    }

    try {
        sqlite::database db(databasePath);

        // 새 데이터 추가
        db << "INSERT INTO settings (saveName, "
              "vAValue, iAValue, iAPhaseDeg, "
              "vBValue, vBPhaseDeg, iBValue, iBPhaseDeg, "
              "vCValue, vCPhaseDeg, iCValue, iCPhaseDeg, "
              "frequency, timeScale, cyclePerSecond, samplePerCycle, graphWidth, displayUpdateMode, "
              "vHarmonicOrder, vHarmonicMag, vHarmonicPhase, "
              "cHarmonicOrder, cHarmonicMag, cHarmonicPhase) "
              "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);"
           << saveName
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

        qDebug() << "[새로운 설정 저장 완료]:" << QString::fromStdString(saveName);


    } catch (sqlite::sqlite_exception& e) {
        qDebug() << "[설정 저장 오류]:" << e.what();
    }
}


//불러오기
void settingStorage::loadByName(const std::string& saveName, dataManagement& dataMng)
{
    try {
        sqlite::database db(databasePath);

        db << "SELECT vAValue, iAValue, iAPhaseDeg, "
              "vBValue, vBPhaseDeg, iBValue, iBPhaseDeg, "
              "vCValue, vCPhaseDeg, iCValue, iCPhaseDeg, "
              "frequency, timeScale, cyclePerSecond, samplePerCycle, graphWidth, displayUpdateMode, "
              "vHarmonicOrder, vHarmonicMag, vHarmonicPhase, "
              "cHarmonicOrder, cHarmonicMag, cHarmonicPhase "
              "FROM settings WHERE saveName=?;"
           << saveName
            >> [&](double vA, double iA, int phA,
                   double vB, int vBph, double iB, int iBph,
                   double vC, int vCph, double iC, int iCph,
                   double freq, int scale, double cps, int sps,
                   double width, std::string mode,
                   int vOrd, double vMag, int vPhs, int cOrd, double cMag, int cPhs) {

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
              };

        qDebug() << "[불러오기 완료]:" << QString::fromStdString(saveName);
    }
    catch (sqlite::sqlite_exception& e) {
        qDebug() << "[불러오기 오류]:" << e.what();
    }
}

//리스트이름을 보고 값 text로 불러오기
QString settingStorage::previewLoadText(const std::string& saveName)
{
    QString info;
    try {
        sqlite::database db(databasePath);

        db << "SELECT vAValue, iAValue, iAPhaseDeg, "
              "vBValue, vBPhaseDeg, iBValue, iBPhaseDeg, "
              "vCValue, vCPhaseDeg, iCValue, iCPhaseDeg, "
              "frequency, timeScale, cyclePerSecond, samplePerCycle, graphWidth, displayUpdateMode, "
              "vHarmonicOrder, vHarmonicMag, vHarmonicPhase, "
              "cHarmonicOrder, cHarmonicMag, cHarmonicPhase "
              "FROM settings WHERE saveName=?;"
           << saveName
            >> [&](double vA, double iA, int phA,
                   double vB, int vBph, double iB, int iBph,
                   double vC, int vCph, double iC, int iCph,
                   double freq, int scale, double cps, int sps,
                   double width, std::string mode,
                   int vOrd, double vMag, int vPhs, int cOrd, double cMag, int cPhs) {

                  // --- 현재 dataMng 값 업데이트 없이 단순 표시 ---
                  info += QString("[ %1 ]\n\n").arg(QString::fromStdString(saveName));
                  info += QString("주파수 (Hz):      %1\n").arg(freq);
                  info += QString("초당 Cycle 수:    %1\n").arg(cps);
                  info += QString("Cycle당 샘플 수:  %1\n").arg(sps);
                  info += QString("TimeScale:       %1\n").arg(scale);
                  info += QString("그래프 폭:        %1\n").arg(width);
                  info += "\n--- 3상 전압 ---\n";
                  info += QString("A상:  V=%1\n").arg(vA);
                  info += QString("B상:  V=%1,  φ=%2°\n").arg(vB).arg(vBph);
                  info += QString("C상:  V=%1,  φ=%2°\n").arg(vC).arg(vCph);
                  info += "\n--- 3상 전류 ---\n";
                  info += QString("A상:  I=%1,  φ=%2°\n").arg(iA).arg(phA);
                  info += QString("B상:  I=%1,  φ=%2°\n").arg(iB).arg(iBph);
                  info += QString("C상:  I=%1,  φ=%2°\n").arg(iC).arg(iCph);
                  info += "\n--- 고조파 ---\n";
                  info += QString("전압: %1차, 크기=%2, 위상=%3°\n")
                              .arg(vOrd).arg(vMag).arg(vPhs);
                  info += QString("전류: %1차, 크기=%2, 위상=%3°")
                              .arg(cOrd).arg(cMag).arg(cPhs);
              };

        if (info.isEmpty())
            info = "[해당 이름의 설정을 찾을 수 없습니다.]";
    }
    catch (sqlite::sqlite_exception& e) {
        info = QString("[DB 조회 오류] %1").arg(e.what());
    }

    return info;
}

//리스트에 적을 savename 목록 가져오기
std::vector<std::string> settingStorage::getAllSaveNames()
{
    std::vector<std::string> names;
    try {
        sqlite::database db(databasePath);
        db << "SELECT saveName FROM settings WHERE saveName IS NOT NULL ORDER BY id ASC;"
            >> [&](std::string name) {
                  names.push_back(name);
              };
    }
    catch (sqlite::sqlite_exception& e) {
        qDebug() << "[saveName 목록 불러오기 오류]:" << e.what();
    }
    return names;
}

// 마지막 불러온 이름 저장
void settingStorage::saveLastLoadedName(const std::string& name)
{
    QSettings pref("powerSimulation", "lastname");
    pref.setValue("lastLoadedName", QString::fromStdString(name));
    qDebug() << "[마지막 불러온 설정 저장]:" << QString::fromStdString(name);
}


//  마지막 불러온 이름  가져오기
std::string settingStorage::getLastLoadedName()
{
    QSettings pref("powerSimulation", "lastname");
    QString name = pref.value("lastLoadedName", "").toString();

    // ---- 최초 실행 시 기본값 자동 저장 ----
    if (name.isEmpty()) {
        name = "기본값";
        pref.setValue("lastLoadedName", name);
        qDebug() << "[최초 실행: 기본값으로 초기화]";
    }

    return name.toStdString();
}

//삭제하기
void settingStorage::deleteByName(const std::string& saveName)
{
    try {
        sqlite::database db(databasePath);

        db << "DELETE FROM settings WHERE saveName=?;"
           << saveName;

        qDebug() << "[설정 삭제 완료]:" << QString::fromStdString(saveName);
    }
    catch (sqlite::sqlite_exception& e) {
        qDebug() << "[설정 삭제 오류]:" << e.what();
    }
}

