#ifndef SETTINGSTORAGE_H
#define SETTINGSTORAGE_H

#include "datamanagement.h"

class settingStorage
{
public:
    settingStorage(const std::string& dbPath = "mydb.sqlite");

    void save(dataManagement& dataMng);
    void load(dataManagement& dataMng);

private:
    std::string databasePath;
};

#endif // SETTINGSTORAGE_H
