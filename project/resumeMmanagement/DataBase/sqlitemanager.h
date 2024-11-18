#ifndef SQLITEMANAGER_H
#define SQLITEMANAGER_H

#include "absdbmanager.h"

class SqliteManager : public AbsDBManager
{
public:
    SqliteManager();

    // AbsDBManager interface

    void initDB(QString db)override;
    bool createTable()override;
};

#endif // SQLITEMANAGER_H
