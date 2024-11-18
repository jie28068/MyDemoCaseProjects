#ifndef SIMUNPSDATADICTIONARYSERVER_H
#define SIMUNPSDATADICTIONARYSERVER_H

#include "CoreLib/ServerBase.h"
#include "server/DataDictionary/IDataDictionaryDDXServer.h"
#include "server/DataDictionary/IDataDictionaryServer.h"
#include "server/DataDictionary/IDataDictionaryViewServer.h"
#include "server/SimuNPSAdapter/ISimuNPSDataDictionaryServer.h"
#include <QReadWriteLock>

using namespace Kcc;
using namespace Kcc::DataDictionary;
Kcc_SimuNPSAdapter_Namespace

#define SERVER_VERSION 1

        enum actionType {
            Action_Add, // 添加
            Action_Del  // 删除
        };
typedef struct {
    actionType action;    // 动作行为  添加or删除
    QString variableName; // 字典元素路径
    QString variableType; // 字典元素类型
    QString describe;     // 字典元素描述信息
} BufferData;

class SimuNPSDataDictionaryServer : public ServerBase, public ISimuNPSDataDictionaryServer
{
public:
    SimuNPSDataDictionaryServer(void);
    ~SimuNPSDataDictionaryServer(void);

    // IServerInterfaceBase
    // virtual QString GetServerGroupName() const {	return SERVER_GROUP_SIMUNPSADAPTER_NAME;	}
    // virtual QString GetInterfaceDefName() const {	return SERVER_INTERFACE_ISIMUNPSDATADICTIONARY_NAME;	}
    // virtual unsigned int GetVersion() const {	return SERVER_VERSION;	}
    virtual void Init();
    virtual void UnInit();
    // new add
    // virtual bool BuildDataDictionary(const QString &str);
    // //编译
    virtual bool AddDataDictionary(const QString &variableName, const QString &VariableType = "double",
                                   const QString describe = "");     // 添加
    virtual bool RunDataDictionary();                                // 运行
    virtual bool DelDataDictionary(const QString &variableName);     // 删除
    virtual void DelDataDictionary(const QStringList &variableList); // 删除多个字典元素
    virtual bool StopDataDictionary();                               // 停止
    virtual bool ClearAllCurrentDataDictionary();
    virtual QStringList GetAllCurrentDataDictionary();
    virtual QStringList GetDictionaryElement(const QString &variableName);
    virtual bool RenameDictionaryElement(const QString &elementName, const QString &newName);
    virtual bool isHaveDictionaryElement(const QString &variableName);

    // ISimuNPSDataDictionaryServer
    virtual bool BuildAndRunDataDictionary(const QString &str);
    virtual bool QuitRunDataDictionary();
    virtual bool IsRuning();
    virtual bool InputChannelData(const QString &paramName, double d, quint64 time64);

    virtual bool InputChannelData(const QString &paramName, unsigned char *data, unsigned int len,
                                  unsigned int bytePos = 0);
    virtual bool InputChannelData(const QString &paramName, IDataPackList *pDataList);
    virtual bool InputChannelData(int paramId, unsigned char *data, unsigned int len, unsigned int bytePos = 0);
    virtual bool InputChannelData(int paramId, IDataPackList *pDataList);
    virtual bool InputChannelData(const QString &paramName, double *dArr, int dataCount, quint64 firstDataTimeStamp,
                                  quint64 timeSpace);

    virtual bool AddDictionaryElementToPlotPanel(int plotPos, const QString name);

    // 设置字典ChannaelId
    virtual void SetValidElementChannelId(int ElementChannelId);
    // 获取字典ChannaelId
    virtual int GetValidElementChannelId();

private:
    // 设置状态，防止多次进入run 状态
    inline void setRunStatus(bool b)
    {
        _lock.lockForWrite();
        _bRun = b;
        _lock.unlock();
    }
    inline bool getRunStatus()
    {
        _lock.lockForRead();
        bool b = _bRun;
        _lock.unlock();
        return b;
    }
    // 解析Json层级数据、添加字典元素
    bool AddDataDictionaryHandle(const PIDataDictionaryBuilder &dictionaryBuilder, const QStringList &variableNameList,
                                 const QString &VariableType, const QString describe = "");
    // 解析Json层级数据、删除字典元素
    bool DelDataDictionaryHandle(const PIDataDictionaryBuilder &dictionaryBuilder, const QString &variableName);
    // 创建字典元素
    PIElementStructBuilder createElement(const PIDataDictionaryBuilder &dictionaryBuilder,
                                         const PIElementStructBuilder &parentStruct, std::string str);
    void createdoubleElement(const PIDataDictionaryBuilder &dictionaryBuilder,
                             const PIElementStructBuilder &parentStruct, std::string str, QString describe = "");
    PIElementBase FindDataDictionary(const PIDataDictionaryBuilder &dictionaryBuilder, QString &Findpath,
                                     QString Elementname);
    bool BuildDataDictionary(const QStringList &variableList, const QString &variableType);
    // bool isStatuschange(int oldStatus,int newStatus);
    // void saveActionToBuffer(actionType Type,const QString &variableName,const QString &variableType = "",const
    // QString &describe = ""); void loadBufferAction();
    bool isHaveChildNode(PIElementBase Element); // 判断元素是否含有子节点
    void delNotHaveChildNodeElement(PIElementBase pElement); // 删除元素后,自动删除无子节点的父节点元素,直到root节点停止
    void releaseDoubleMemory(PIElementBase Element); // 删除节点时,释放double层元素内存
    void releasedicEleChannelMap(PIElementBase Element); // 删除元素后,同步删除_dicEleChannelMap中存储的元素路径
    void GetDictionaryElementHandle(PIElementBase element, QStringList &elementList);
    bool DelDataDictionaryStartWith(const QString &variableName);
    bool renameDictionaryElementStartWith(const QString &elementName, const QString &newName);
    bool checkDictionaryName(const QString &checkName,
                             const QString &parentName); // 若字典原始有是四层结构,检测两字典元素是否是父子类关系

private:
    PIDataDictionaryServer _dataDicServer;
    PIDataDictionaryDDXServer _dataDicDDXServer;
    PIDataDictionaryViewServer _dataDicViewServer;
    // PIDataManagerServer _dataManagerServer;
    PIDDXTargetAdpter _targetAdpter;
    QReadWriteLock _lock;
    bool _bRun;

    unsigned long long currentdicId;
    QMap<QString, void *> _dicEleChannelMap;
    int _beginElementChannelId;
    QString _firstElement; // 记录第一层元素名称   一般为当前仿真或者current
    // QVector<PIElementBase> pElementList;
    // //通过add每添加一个字典元素都分配了内存,每加一个存储一个；用来当clearAll时,释放它们内存 int _runStatuscount;
    // QVector<BufferData> vectorBuffer;
};

#endif
