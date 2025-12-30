#ifndef SETTINGSTORAGE_H
#define SETTINGSTORAGE_H

#include "dataManagement.h"
#include "sqlite_modern_cpp.h"

class settingStorage
{
public:
    settingStorage(const std::string& dbPath = "mydb.sqlite");

    void save(dataManagement& dataMng);
    void load(dataManagement& dataMng);
    void createNewDatabase(sqlite::database& db);

private:
    std::string databasePath;
};

#endif // SETTINGSTORAGE_H
