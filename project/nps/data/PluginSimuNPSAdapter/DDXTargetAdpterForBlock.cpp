#include "DDXTargetAdpterForBlock.h"
#include "CoreLib/ServerManager.h"
#include "server/SimuNPSAdapter/def.h"

USE_LOGOUT_("DDXTargetAdpterForBlock")

// #define Test_SimuNPSDataDictionaryDemo 1

#ifdef Test_SimuNPSDataDictionaryDemo // 测试 更新 字典数据

#include "CoreLib/ServerManager.h"
#include "DataPackList.h"
#include "SimuNPSDataDictionaryServer.h"
#include "server/DataDictionary/IDataDictionaryDDXServer.h"

using namespace Kcc;
using namespace Kcc::DataDictionary;

class SimuNPSDataDictionaryDemo : public QThread
{
public:
    PIDataDictionaryDDXServer _ddxServer;
    PISimuNPSDataDictionaryServer _server;
    bool _bStop;
    double _dData;

    SimuNPSDataDictionaryDemo() : _bStop(false) { }

    ~SimuNPSDataDictionaryDemo()
    {
        _ddxServer.clear();
        _server.clear();
    }

private:
#if 1
    void run() override
    {
        double d = 1;
        while (!_bStop) {
            _dData = 10 * sin(0.435 * d + 10);
            d++;
            _server->InputChannelData("画板1.节点电压.节点1", _dData);
            msleep(100);
        }
    }
#else

    void run() override
    {
        double d = 1;
        double timeOff = 0; // QDateTime::currentDateTime().toTime_t();
        while (!_bStop) {
            _dData = 10 * sin(0.435 * d + 10);
            d++;

            // 组包输入：
            DataPackList *dataPack = new DataPackList();
            dataPack->attachBuff((unsigned char *)&_dData, sizeof(double), 1, timeOff, 1);
            _server->InputChannelData("d1", dataPack);
            delete dataPack;
            //_server->InputChannelData("d1", (unsigned char *)&_dData, sizeof(double));

            qDebug() << "d1 " << _dData;
            msleep(100);
            timeOff += 0.1 * 1000000;
        }
    }
#endif
};

SimuNPSDataDictionaryDemo testDemo;

class TestDataDictionaryVariable : public IVariableNotifyHandler
{
public:
    // IVariableNotifyHandler
    virtual void OnVariableUpdate(PIVariableBase trigger, PIVariableBase &thisVariable, const DataInfo &series,
                                  const int arrayIndex, const VariableType variableType,
                                  const NumberUnit &value) override
    {
        PIVariableNumber number = thisVariable->ToVariableNumber();

        // LOGOUT(QString("OnVariableUpdate[%1]
        // value[%2]").arg(thisVariable->FullName()).arg(number->Value().toDouble()));
    }

    TestDataDictionaryVariable() { }

    ~TestDataDictionaryVariable()
    {
        if (_var) {
            _var->RemoveVariableChangedHandler(this);
        }
    }

    void Init(const QString &varPath)
    {
        PIDataDictionaryServer server = RequestServer<IDataDictionaryServer>(SERVER_GROUP_DATADICTIONARY_NAME,
                                                                             SERVER_INTERFACE_DATADICTIONARY_NAME);
        if (server) {
            PIDataDictionaryBuilder dic = server->GetCurrentRuntimeDictionary();
            PIElementBase element = dic->FindElement(varPath);
            if (element) {
                _var = element->RefToVariable();
                _var->AddVariableChangedHandler(this);
            }
        }
    }

private:
    PIVariableBase _var;
};

TestDataDictionaryVariable testDicVar;

#endif

DDXTargetAdpterForBlock::DDXTargetAdpterForBlock(const PIDataDictionaryDDXServer &pddxServer) : _ddxServer(pddxServer)
{
}

DDXTargetAdpterForBlock::~DDXTargetAdpterForBlock()
{
    _ddxServer.clear();
}

bool DDXTargetAdpterForBlock::ReadyToRun()
{
#ifdef Test_SimuNPSDataDictionaryDemo
    // 启动测试demo
    testDemo._ddxServer = RequestServer<IDataDictionaryDDXServer>(SERVER_GROUP_DATADICTIONARYDDX_NAME,
                                                                  SERVER_INTERFACE_DATADICTIONARYDDX_NAME);
    testDemo._server = RequestServer<ISimuNPSDataDictionaryServer>(SERVER_GROUP_SIMUNPSADAPTER_NAME,
                                                                   SERVER_INTERFACE_ISIMUNPSDATADICTIONARY_NAME);

    testDemo.start(QThread::IdlePriority);

    testDicVar.Init("Dictionary.d1");
#endif
    return true;
}

bool DDXTargetAdpterForBlock::ReadyToStop()
{
#ifdef Test_SimuNPSDataDictionaryDemo
    if (testDemo.isRunning()) {
        testDemo._bStop = true;
        testDemo.wait(5000);
    }
#endif

    return true;
}

bool DDXTargetAdpterForBlock::BeforeAddInMonitor(PIElementBase element)
{
    return true;
}

bool DDXTargetAdpterForBlock::AfterRemoveFromMonitor(PIElementBase element)
{
    return true;
}

QString DDXTargetAdpterForBlock::AdpterType() const
{
    return ADAPTER_TYPE_FOR_BLOCK;
}

void *DDXTargetAdpterForBlock::CreateChannelID(const DDXChannelInfoStruct &configs)
{
    // todo 字典元素的 信息（来源于SimuNPSDataDictionaryContentLoader::Load） 构建成一个通道
    int id = configs["id"].toInt();
    return (void *)id;
}

bool DDXTargetAdpterForBlock::OutputChannelData(const void *channelID, unsigned char *data, unsigned int len,
                                                DataInfo &dinfo)
{
    // 发送通道数据 todo 暂时用不到

    int id = (int)channelID;

    return true;
}
