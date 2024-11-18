#ifndef PLUGINSIMUNPSADAPTER_H
#define PLUGINSIMUNPSADAPTER_H
#include "CoreLib/Module.h"
#include "DDXTargetAdpterForBlock.h"
#include "pluginsimunpsadapter_global.h"
#include "server/DataDictionary/IDataDictionaryDDXServer.h"
#include "server/DataDictionary/IDataDictionaryServer.h"
#include <QObject>

class PLUGINSIMUNPSADAPTER_EXPORT PluginSimuNPSAdapter : public QObject, public Module
{
    Q_OBJECT
    DEFINE_MODULE
public:
    PluginSimuNPSAdapter(QString strName, QObject *parent = NULL);
    ~PluginSimuNPSAdapter();

    // Module
    virtual void init(KeyValueMap &params);
    virtual void unInit(KeyValueMap &saveParams);

private slots:
    void onDataDictionaryViewServerNotify(unsigned int code, const NotifyStruct &cmdStruct);
    void TestDataDicServer();

private:
    QSharedPointer<DDXTargetAdpterForBlock> _targetAdpter;
};

#endif // PLUGINSIMUNPSADAPTER_H
