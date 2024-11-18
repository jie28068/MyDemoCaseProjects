#include "sessionfactory.h"
//#include "sessionbaselogin.h"
//#include "sessionbaseregist.h"
//#include "sessionhrmfilterresume.h"
//#include "sessionhrmqueryresume.h"
//#include "sessionhrmreplacearchive.h"
//#include "sessionhrmuploadarchive.h"
//#include "sessionhrmuploadresume.h"

QMap<QString, const QMetaObject*> SessionFactory::metaMap;
SessionFactory::SessionFactory(QObject *parent) : QObject(parent)
{

}

void SessionFactory::registeSessions()
{
//    registeSession(SessionBaseLogin::staticMetaObject);
//    registeSession(SessionBaseRegist::staticMetaObject);
//    registeSession(SessionHrmFilterResume::staticMetaObject);
//    registeSession(SessionHrmQueryResume::staticMetaObject);
//    registeSession(SessionHrmReplaceArchive::staticMetaObject);
//    registeSession(SessionHrmUploadArchive::staticMetaObject);
//    registeSession(SessionHrmUploadResume::staticMetaObject);
}

void SessionFactory::registeSession(const QMetaObject& metaObj)
{
    qDebug()<<"registeSession"<<metaObj.className();
    metaMap.insert(metaObj.className(), &metaObj);
}
