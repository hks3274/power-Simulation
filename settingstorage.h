#ifndef SETTINGSTORAGE_H
#define SETTINGSTORAGE_H

#include "dataManagement.h"
#include "sqlite_modern_cpp.h"

#include <QSettings>


class settingStorage
{
public:
    settingStorage(const std::string& dbPath = "mydb.sqlite");

    //저장
    void save(const std::string& saveName, dataManagement& dataMng);
    //불러오기
    void loadByName(const std::string& saveName, dataManagement& dataMng);
    //새로운 데이터베이스 만들기
    void createNewDatabase(sqlite::database& db);

    //리스트이름을 보고 값 text로 불러오기
    QString previewLoadText(const std::string& saveName);
    //리스트에 적을 savename 목록 가져오기
    std::vector<std::string> getAllSaveNames();


    // 마지막 불러온 이름 저장/불러오기
    std::string getLastLoadedName();
    void saveLastLoadedName(const std::string& name);

    //삭제
    void deleteByName(const std::string& saveName);


private:
    std::string databasePath;
};

#endif // SETTINGSTORAGE_H
