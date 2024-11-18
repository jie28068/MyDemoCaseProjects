#include "SimuNPSDataDictionaryServer.h"
#include "CoreLib/ServerManager.h"
#include "DataPackList.h"
#include "KLModelDefinitionCore/PublicDefine.h"
#include "server/MonitorPanel/IMonitorPanelPlotServer.h"
#include "server/SimuNPSAdapter/def.h"
#include "Json/json.h"

KCC_USE_NAMESPACE_MONITORPANEL

USE_LOGOUT_("SimuNPSDataDictionaryServer")

#define DICTIONARY_ROOT "Dictionary."

SimuNPSDataDictionaryServer::SimuNPSDataDictionaryServer()
    : _bRun(false), _beginElementChannelId(10000), currentdicId(0) /*,_runStatuscount(0)*/
{
}

SimuNPSDataDictionaryServer::~SimuNPSDataDictionaryServer(void)
{
    _dataDicServer.clear();
    _dataDicViewServer.clear();
    _dataDicDDXServer.clear();
}

void SimuNPSDataDictionaryServer::Init()
{
    _dataDicServer = RequestServer<IDataDictionaryServer>();
    if (!_dataDicServer) {
        LOGOUT(QObject::tr("Failed to obtain the DataDictionaryServer service!"),
               LOG_WARNING); // DataDictionaryServer服务获取失败!
    }

    _dataDicDDXServer = RequestServer<IDataDictionaryDDXServer>();
    if (!_dataDicDDXServer) {
        LOGOUT(QObject::tr("Failed to obtain the DataDictionaryDDXServer service!"),
               LOG_WARNING); // DataDictionaryDDXServer服务获取失败!
    }

    _dataDicViewServer = RequestServer<IDataDictionaryViewServer>();
    if (!_dataDicViewServer) {
        LOGOUT(QObject::tr("Failed to obtain the DataDictionaryViewServer service!"),
               LOG_WARNING); // DataDictionaryViewServer服务获取失败!
    }
    // _dataManagerServer = RequestServer<IDataManagerServer>();
    // if (!_dataManagerServer) {
    //     LOGOUT("DataManagerServer服务获取失败!", LOG_WARNING);
    // }
}

void SimuNPSDataDictionaryServer::UnInit()
{
    _dataDicServer.clear();
    _dataDicDDXServer.clear();
    _dataDicViewServer.clear();

    _targetAdpter.clear();
}

bool SimuNPSDataDictionaryServer::BuildAndRunDataDictionary(const QString &str)
{
    if (getRunStatus())
        return false;
    if (str.isNull()) {
        return false;
    }
    DictionaryParamMap paraMap;
    paraMap["json"] = str;
    // paraMap["json"] = "{\"层次1\":{\"层次2\":{\"层次3\":{\"层次4\":\"double\"}}}}";   //test

    unsigned long long dicId = 0;
    if (_dataDicServer) {
        dicId = _dataDicServer->GetDictionaryID(paraMap);
        if (dicId != 0) {
            PIDataDictionaryBuilder builder = _dataDicServer->GetDictionaryBuilder(dicId);
            builder->Load(paraMap);
        } else {
            dicId = _dataDicServer->LoadDictionary(paraMap);
        }
    }
    currentdicId = dicId;
    // AddDataDictionary("{\"层次2\":{\"层次2\":{\"层次3\":{\"层次4\":\"double\"}}}}");
    // AddDataDictionary("{\"层次1\":{\"层次2\":{\"层次33\":{\"层次4\":\"double\"}}}}");   /test
    // 更新 数据字典视图
    if (_dataDicViewServer) {
        IElementTreeViewController *pTreeViewControl = _dataDicViewServer->GetDefaultElementTreeViewController();
        if (pTreeViewControl) {
            pTreeViewControl->LoadDataDictionary(dicId);
        }
    }

    _dicEleChannelMap.clear();

    // 进入运行
    if (_dataDicServer) {
        _dataDicServer->SwitchRuntimeDictionary(dicId);
        _dataDicServer->StartRunTime();
    }

    setRunStatus(true);

    return true;
}

bool SimuNPSDataDictionaryServer::QuitRunDataDictionary()
{
    if (!getRunStatus())
        return false;

    if (_dataDicServer) {
        _dataDicServer->StopRunTime();
    }

    if (_dataDicViewServer) {
        IElementTreeViewController *pTreeViewControl = _dataDicViewServer->GetDefaultElementTreeViewController();
        if (pTreeViewControl) {
            pTreeViewControl->UnloadDataDictionary();
        }
    }

    setRunStatus(false);

    _dicEleChannelMap.clear();
    return true;
}

bool SimuNPSDataDictionaryServer::IsRuning()
{
    return getRunStatus();
}

bool SimuNPSDataDictionaryServer::InputChannelData(const QString &paramName, double d, quint64 time64)
{
    if (_dataDicServer && _dataDicDDXServer) {
        PIDataDictionaryBuilder pBuilder = _dataDicServer->GetCurrentRuntimeDictionary();
        if (pBuilder) {
            // 需要根据 paramName 名称 可以查询到 字典元素 （根据路径查询)
            QString fullName = DICTIONARY_ROOT + paramName;
            PIElementBase element = pBuilder->FindElement(fullName);
            if (element) {
                // 通过适配器 构建channelId, DDXInputChannelInfo在创建字典元素的时候设置。
                if (!_targetAdpter) {
                    _targetAdpter = _dataDicDDXServer->FindDDXTargetAdpter(ADAPTER_TYPE_FOR_BLOCK);
                }
                if (_targetAdpter) {
                    // todo 由于监控控制只实现了IDataPackList,这里进行封包
                    DataPackList *dataPack = new DataPackList();
                    dataPack->attachBuff((unsigned char *)&d, sizeof(double), 1, time64, 1000);
                    // bool ret = InputChannelData(paramName, dataPack);
                    bool ret = true;
                    void *channelId = NULL;
                    if (_dicEleChannelMap.contains(fullName)) {
                        channelId = _dicEleChannelMap[fullName];
                    } else {
                        channelId = _targetAdpter->CreateChannelID(element->DDXInputChannelInfo());
                        _dicEleChannelMap.insert(fullName, channelId);
                    }
                    _dataDicDDXServer->InputChannelData(channelId, dataPack);
                    delete dataPack;
                    return ret;
                }
            } else
                LOGOUT(QObject::tr("Dictionary element not found") + QString(":%1").arg(fullName),
                       LOG_WARNING); // 字典元素未找到
        }
    }
    return false;
}

bool SimuNPSDataDictionaryServer::InputChannelData(const QString &paramName, unsigned char *data, unsigned int len,
                                                   unsigned int bytePos /*= 0*/)
{
    if (_dataDicServer && _dataDicDDXServer) {
        PIDataDictionaryBuilder pBuilder = _dataDicServer->GetCurrentRuntimeDictionary();
        if (pBuilder) {
            // 需要根据 paramName 名称 可以查询到 字典元素 （根据路径查询)
            QString fullName = DICTIONARY_ROOT + paramName;
            PIElementBase element = pBuilder->FindElement(fullName);
            if (element) {
                // 通过适配器 构建channelId, DDXInputChannelInfo在创建字典元素的时候设置。
                if (!_targetAdpter) {
                    _targetAdpter = _dataDicDDXServer->FindDDXTargetAdpter(ADAPTER_TYPE_FOR_BLOCK);
                }
                if (_targetAdpter) {
                    DataInfo df;
                    void *channelId = _targetAdpter->CreateChannelID(element->DDXInputChannelInfo());
                    bool ret = _dataDicDDXServer->InputChannelData(channelId, data, len, df, bytePos);
                    return ret;
                }
            }
        }
    }
    return false;
}

bool SimuNPSDataDictionaryServer::InputChannelData(const QString &paramName, IDataPackList *pDataList)
{
    if (_dataDicServer && _dataDicDDXServer) {
        PIDataDictionaryBuilder pBuilder = _dataDicServer->GetCurrentRuntimeDictionary();
        if (pBuilder) {
            // 需要根据 paramName 名称 可以查询到 字典元素 （根据路径查询)
            QString fullName = DICTIONARY_ROOT + paramName;
            PIElementBase element = pBuilder->FindElement(fullName);
            if (element) {
                // 通过适配器 构建channelId, DDXInputChannelInfo在创建字典元素的时候设置。
                if (!_targetAdpter) {
                    _targetAdpter = _dataDicDDXServer->FindDDXTargetAdpter(ADAPTER_TYPE_FOR_BLOCK);
                }
                if (_targetAdpter) {
                    DataInfo df;
                    void *channelId = _targetAdpter->CreateChannelID(element->DDXInputChannelInfo());
                    return _dataDicDDXServer->InputChannelData(channelId, pDataList);
                }
            }
        }
    }
    return false;
}

bool SimuNPSDataDictionaryServer::InputChannelData(int paramId, unsigned char *data, unsigned int len,
                                                   unsigned int bytePos)
{
    if (_dataDicDDXServer) {
        DataInfo df;
        return _dataDicDDXServer->InputChannelData((void *)paramId, data, len, df, bytePos);
    }
    return false;
}

bool SimuNPSDataDictionaryServer::InputChannelData(int paramId, IDataPackList *pDataList)
{
    if (_dataDicDDXServer) {
        return _dataDicDDXServer->InputChannelData((void *)paramId, pDataList);
    }
    return false;
}

bool SimuNPSDataDictionaryServer::InputChannelData(const QString &paramName, double *dArr, int dataCount,
                                                   quint64 firstDataTimeStamp, quint64 timeSpace)
{
    if (_dataDicServer && _dataDicDDXServer) {
        PIDataDictionaryBuilder pBuilder = _dataDicServer->GetCurrentRuntimeDictionary();
        if (pBuilder) {
            // 需要根据paramName名称可以查询到字典元素（根据路径查询）
            QString fullName = DICTIONARY_ROOT + paramName;
            PIElementBase element = pBuilder->FindElement(fullName);
            if (element) {
                // 通过适配器构建channelId,DDXInputChannelInfo在创建字典元素的时候设置
                if (!_targetAdpter) {
                    _targetAdpter = _dataDicDDXServer->FindDDXTargetAdpter(ADAPTER_TYPE_FOR_BLOCK);
                }
                if (_targetAdpter) {
                    DataPackList *dataPack = new DataPackList();
                    dataPack->attachBuff((unsigned char *)dArr, sizeof(double) * dataCount, dataCount,
                                         firstDataTimeStamp, timeSpace);
                    bool ret = true;
                    {
                        void *channelId = NULL;
                        if (_dicEleChannelMap.contains(fullName)) {
                            channelId = _dicEleChannelMap[fullName];
                        } else {
                            channelId = _targetAdpter->CreateChannelID(element->DDXInputChannelInfo());
                            _dicEleChannelMap.insert(fullName, channelId);
                        }
                        _dataDicDDXServer->InputChannelData(channelId, dataPack);
                    }
                    delete dataPack;
                    return true;
                }
            } else {
                LOGOUT(QObject::tr("Dictionary element not found") + QString("：%1").arg(fullName),
                       LOG_WARNING); // 字典元素未找到
            }
        }
    }
    return false;
}

bool SimuNPSDataDictionaryServer::AddDictionaryElementToPlotPanel(int plotPos, const QString name)
{
    // 调用监控面板的服务
    PIMonitorPanelPlotServer panelServer = RequestServer<IMonitorPanelPlotServer>();
    if (panelServer) {
        QString fullName = DICTIONARY_ROOT + name;
        int ret = panelServer->AddCurvesToPlot(QStringList() << fullName, plotPos);
        return ret == 0;
    } else {
        LOGOUT(QObject::tr("MonitorPanelPlotServer failed to obtain service!"),
               LOG_WARNING); // MonitorPanelPlotServer服务获取失败!
        return false;
    }
}

bool SimuNPSDataDictionaryServer::BuildDataDictionary(const QStringList &variableList, const QString &variableType)
{
    if (variableList.length() != 4) {
        return false;
    }
    DictionaryParamMap paraMap;
    // 构造Json格式
    QString str = QString("{\"%1\":{\"%2\":{\"%3\":{\"%4\":\"%5\"}}}}")
                          .arg(variableList[0])
                          .arg(variableList[1])
                          .arg(variableList[2])
                          .arg(variableList[3])
                          .arg(variableType);
    paraMap["json"] = str;

    unsigned long long dicId = 0;
    if (_dataDicServer) {
        dicId = _dataDicServer->GetDictionaryID(paraMap);
        if (dicId != 0) {
            PIDataDictionaryBuilder builder = _dataDicServer->GetDictionaryBuilder(dicId);
            builder->Load(paraMap);
        } else {
            dicId = _dataDicServer->LoadDictionary(paraMap);
        }
    }
    currentdicId = dicId;

    _dicEleChannelMap.clear();

    if (dicId != 0)
        return true;
    else
        return false;
}

bool SimuNPSDataDictionaryServer::RunDataDictionary()
{
    // int oldstatus = _runStatuscount;
    //_runStatuscount++;
    // if(!isStatuschange(oldstatus,_runStatuscount))     //状态未变化 不是真的开始
    //	return false;
    if (IsRuning())
        return false;
    // 运行数据字典
    if (0 == currentdicId) {
        // LOGOUT("无字典元素,请先添加数据字典元素");
        // return false;
        AddDataDictionary("仿真结果2.画板1.测试模块1.端口xxxx");
        DelDataDictionary("仿真结果2");
    }
    unsigned long long dicId = 0;
    dicId = currentdicId;

    // 进入运行
    if (_dataDicServer) {
        _dataDicServer->SwitchRuntimeDictionary(dicId);
        _dataDicServer->StartRunTime();
    }
    setRunStatus(true);

    return true;
}

bool SimuNPSDataDictionaryServer::AddDataDictionary(const QString &variableName, const QString &VariableType,
                                                    const QString describe)
{
    // if(IsRuning())      //数据字典为运行状态,则缓存该动作,等字典停止再加载动作
    //{
    //	//存储动作至缓存区
    //	saveActionToBuffer(Action_Add,variableName,VariableType,describe);
    //	return true;
    // }

    if (variableName.isNull())
        return false;

    QStringList variableNameList = variableName.split(".");
    if (variableNameList.length() != 4) {
        LOGOUT(QObject::tr("The incoming dictionary element must have four layers"),
               Kcc::LOG_DEBUG); // 传入字典元素必须为四层
        return false;
    }
    if (currentdicId == 0) // 当前字典ID为0,说明无数据字典,先加载进平台
    {
        if (!BuildDataDictionary(variableNameList, VariableType)) {
            LOGOUT(QObject::tr("Dictionary element loading failed")); // 字典元素加载失败
            return false;
        }
        //_firstElement = variableNameList[0];   //记录第一次存入的 第一次元素名称 "当前仿真"
        LOGOUT(QObject::tr("Data dictionary initialization is complete")); // 数据字典初始化完成
        return true;
    }

    unsigned long long dicId = 0;
    dicId = currentdicId;

    PIDataDictionaryBuilder dictionaryBuilder = _dataDicServer->GetDictionaryBuilder(dicId);

    if (isHaveDictionaryElement(variableName)) {
        return true;
    }

    // 解析paraMap层级,并创建对应字典元素
    bool bAdd = AddDataDictionaryHandle(dictionaryBuilder, variableNameList, VariableType, describe);
    if (!bAdd) {
        LOGOUT(QObject::tr("Failed to add dictionary element. Procedure")); // 添加字典元素失败
    }
    return bAdd;
}

bool SimuNPSDataDictionaryServer::AddDataDictionaryHandle(const PIDataDictionaryBuilder &dictionaryBuilder,
                                                          const QStringList &variableNameList,
                                                          const QString &VariableType, const QString describe)
{
    if (!dictionaryBuilder)
        return false;
    PIElementBase root = dictionaryBuilder->GetRoot();
    if (!root)
        return false;
    PIElementStructBuilder structRoot = QueryInterface<IElementStructBuilder>(root);
    if (!structRoot)
        return false;

    QString findpath = "Dictionary"; // root的名字

    // 第一层
    QString newLevel = variableNameList[0];
    std::string newLevelName = newLevel.toStdString();
    PIElementStructBuilder newLeveStruct;
    // 查找第一层
    PIElementBase level0Element = FindDataDictionary(dictionaryBuilder, findpath, QString::fromStdString(newLevelName));
    // 创建结构元素
    if (level0Element == NULL) // 没找到 ，创建一个字典元素
    {
        newLeveStruct = createElement(dictionaryBuilder, structRoot, newLevelName);
    } else // 找到了
    {
        PIElementStructBuilder tempStructBuilder = QueryInterface<IElementStructBuilder>(level0Element);
        newLeveStruct = tempStructBuilder;
    }

    // 第二层
    QString boardname = variableNameList[1];
    std::string boardName = boardname.toStdString();
    PIElementStructBuilder boardStruct;
    // 查找第二层
    PIElementBase level1Element = FindDataDictionary(dictionaryBuilder, findpath, QString::fromStdString(boardName));
    // 创建结构元素
    if (level1Element == NULL) {
        boardStruct = createElement(dictionaryBuilder, newLeveStruct, boardName);
    } else {
        PIElementStructBuilder tempStructBuilder = QueryInterface<IElementStructBuilder>(level1Element);
        boardStruct = tempStructBuilder;
    }
    // 第三层
    QString Contentname = variableNameList[2];
    std::string contentName = Contentname.toStdString();
    PIElementStructBuilder contentStruct;
    // 查找第三层
    PIElementBase level2Element = FindDataDictionary(dictionaryBuilder, findpath, QString::fromStdString(contentName));
    // 创建结构元素
    if (level2Element == NULL) {
        contentStruct = createElement(dictionaryBuilder, boardStruct, contentName);
    } else {
        PIElementStructBuilder tempStructBuilder = QueryInterface<IElementStructBuilder>(level2Element);
        contentStruct = tempStructBuilder;
    }

    // 第四层
    QString merbers = variableNameList[3];
    std::string dataName = merbers.toStdString();
    // 查找第四层
    findpath = findpath + "." + QString::fromStdString(dataName);
    PIElementBase level3Element = dictionaryBuilder->FindElement(findpath);
    if (level3Element != NULL) // 最后一层如果还查找到，说明加进来的是之前有的,直接返回
    {
        level3Element->SetDescription(describe);
        // LOGOUT(QString("请勿重复添加"));
        return true;
    }
    if (0 == VariableType.compare("double")) // 创建double元素
        createdoubleElement(dictionaryBuilder, contentStruct, dataName, describe);

    // LOGOUT(QString("添加字典%1成功").arg(findpath));

    NotifyStruct notify;
    notify.code = Notify_ElementAdded;
    notify.paramMap["name"] = variableNameList;
    emitNotify(notify);

    return true;
}

bool SimuNPSDataDictionaryServer::DelDataDictionary(const QString &variableName)
{
    // if(IsRuning())      //数据字典为运行状态,则缓存该动作,等字典停止再加载动作
    //{
    //	//存储动作至缓存区
    //	saveActionToBuffer(Action_Del,variableName);
    //	return true;
    // }

    if (variableName.isNull())
        return false;
    if (currentdicId == 0) // 当前字典ID为0,说明未加载过字典
    {
        LOGOUT(QObject::tr(
                "If no data dictionary is included, load or add a data dictionary first")); // 未含有数据字典,请先加载或添加数据字典
        return false;
    }
    // QStringList variableNameList = variableName.split(".");
    // if(variableNameList.length()!=4)
    //{
    //	LOGOUT("传入字典元素必须为四层",Kcc::LOG_DEBUG );
    //	return false;
    // }

    unsigned long long dicId = 0;
    dicId = currentdicId;
    PIDataDictionaryBuilder dictionaryBuilder = _dataDicServer->GetDictionaryBuilder(dicId);

    if (!isHaveDictionaryElement(variableName)) {
        return false;
    }

    // 删除前先停止运行数据字典
    StopDataDictionary();
    // 若variableName有四层结构,在这里面处理
    if (DelDataDictionaryStartWith(variableName)) {
        RunDataDictionary();
        return true;
    }

    // 解析传过来删除字典元素的层级路径,并执行删除操作
    bool bAdd = DelDataDictionaryHandle(dictionaryBuilder, variableName);
    RunDataDictionary();

    return bAdd;
}

void SimuNPSDataDictionaryServer::DelDataDictionary(const QStringList &variableList)
{
    if (variableList.isEmpty())
        return;
    if (currentdicId == 0) // 当前字典ID为0,说明未加载过字典
    {
        LOGOUT(QObject::tr(
                "If no data dictionary is included, load or add a data dictionary first")); // 未含有数据字典,请先加载或添加数据字典
        return;
    }

    unsigned long long dicId = 0;
    dicId = currentdicId;
    PIDataDictionaryBuilder dictionaryBuilder = _dataDicServer->GetDictionaryBuilder(dicId);
    // 删除前先停止运行数据字典
    StopDataDictionary();

    for (auto variableName : variableList) {
        // 若variableName有四层结构,在这里面处理
        if (DelDataDictionaryStartWith(variableName)) {
            continue;
        }

        // 解析传过来删除字典元素的层级路径,并执行删除操作
        DelDataDictionaryHandle(dictionaryBuilder, variableName);
    }

    RunDataDictionary();
    return;
}

bool SimuNPSDataDictionaryServer::DelDataDictionaryStartWith(const QString &variableName)
{
    if (variableName.isNull())
        return false;
    if (currentdicId == 0) {
        LOGOUT(QObject::tr(
                "If no data dictionary is included, load or add a data dictionary first")); // 未含有数据字典,请先加载或添加数据字典
        return false;
    }

    QStringList variableNameList = variableName.split(".");
    if (variableNameList.length() != 4) {
        return false;
    }

    unsigned long long dicId = 0;
    dicId = currentdicId;
    PIDataDictionaryBuilder dictionaryBuilder = _dataDicServer->GetDictionaryBuilder(dicId);

    QStringList elements = GetDictionaryElement(
            QString("%1.%2.%3").arg(variableNameList[0]).arg(variableNameList[1]).arg(variableNameList[2]));

    for (auto element : elements) {
        if (checkDictionaryName(element, DICTIONARY_ROOT + variableName)) {
            DelDataDictionaryHandle(dictionaryBuilder, element.remove(DICTIONARY_ROOT));
        }
    }

    return true;
}

bool SimuNPSDataDictionaryServer::DelDataDictionaryHandle(const PIDataDictionaryBuilder &dictionaryBuilder,
                                                          const QString &variableName)
{
    if (!dictionaryBuilder)
        return false;
    PIElementBase root = dictionaryBuilder->GetRoot();
    if (!root)
        return false;
    PIElementStructBuilder structRoot = QueryInterface<IElementStructBuilder>(root);
    if (!structRoot)
        return false;

    QString findpath = DICTIONARY_ROOT + variableName;
    // 根据findpath查找到对应字典元素对象；
    PIElementBase Element = dictionaryBuilder->FindElement(findpath);
    if (!Element) {
        // LOGOUT(QString("数据字典中不存在该字典元素"));
        return false;
    }

    PIElementBase ParentElement = Element->Parent();
    PIElementStructBuilder structParent = QueryInterface<IElementStructBuilder>(ParentElement);
    Element->Grants() |= ElementGrant_Delectable;     // 设置删除权限
    ParentElement->Grants() |= ElementGrant_DelChild; // 设置父节点删除子节点权限

    // 字典处在运行状态下删除字典时,需手动释放double元素内存
    PIDataDictionaryBuilder CurrentRunDictionary = _dataDicServer->GetCurrentRuntimeDictionary();
    if (CurrentRunDictionary) {
        if (CurrentRunDictionary->GetId() == dictionaryBuilder->GetId()) {
            releaseDoubleMemory(Element);
        }
    }

    // 在其父节点上将其移除
    bool bRemove = structParent->Remove(Element);
    if (bRemove) {
        // LOGOUT(QString("删除元素成功%1").arg(findpath));
        // 删除字典元素后,若其父节点无任何子节点则删除父节点
        delNotHaveChildNodeElement(ParentElement);

        NotifyStruct notify;
        notify.code = Notify_ElementDestroyed;
        notify.paramMap["name"] = variableName;
        emitNotify(notify);
    } else {
        LOGOUT(QObject::tr("Element deletion failure") + QString("%1").arg(findpath), LOG_WARNING); // 删除元素失败
    }

    return bRemove;
}

void SimuNPSDataDictionaryServer::SetValidElementChannelId(int ElementChannelId)
{
    _beginElementChannelId = ElementChannelId;
}

int SimuNPSDataDictionaryServer::GetValidElementChannelId()
{
    return _beginElementChannelId;
}

PIElementStructBuilder SimuNPSDataDictionaryServer::createElement(const PIDataDictionaryBuilder &dictionaryBuilder,
                                                                  const PIElementStructBuilder &parentStruct,
                                                                  std::string str)
{
    PIElementBase pElement = dictionaryBuilder->CreateElement(parentStruct, ElementStructBuilder_RegName, DonotCreated);
    Q_ASSERT(pElement);
    PIElementStructBuilder pElementStruct = QueryInterface<IElementStructBuilder>(pElement);
    Q_ASSERT(pElementStruct);
    pElement->SetName(QString(str.c_str()));
    pElement->SetAliaseName(QString(str.c_str()));
    // pElement->SetDescription(describe);

    // 设置ddx 输入输出通道属性
    DDXChannelInfoStruct ddxInfo;
    ddxInfo["name"] = QString(str.c_str());
    ddxInfo["id"] = QString::number(_beginElementChannelId);
    ++_beginElementChannelId;
    pElement->SetDDXInputChannelAdpterType(ADAPTER_TYPE_FOR_BLOCK);
    pElement->SetDDXInputChannelEnable(true);
    pElement->SetDDXInputChannelInfo(ddxInfo);
    pElement->SetDDXOutputChannelAdpterType(ADAPTER_TYPE_FOR_BLOCK);
    pElement->SetDDXOutputChannelEnable(true);
    pElement->SetDDXOutputChannelInfo(ddxInfo);
    parentStruct->PushBack(pElement);

    return pElementStruct;
}

void SimuNPSDataDictionaryServer::createdoubleElement(const PIDataDictionaryBuilder &dictionaryBuilder,
                                                      const PIElementStructBuilder &parentStruct, std::string str,
                                                      QString describe)
{
    PIElementBase pElement = dictionaryBuilder->CreateElement(parentStruct, ElementNumber_RegName, AlwaysCreated);
    Q_ASSERT(pElement);
    PIElementNumber pElementNumber = QueryInterface<IElementNumber>(pElement);
    Q_ASSERT(pElementNumber);
    pElementNumber->SetVariantType(DVariable_Double);

    pElement->SetName(QString(str.c_str()));
    pElement->SetAliaseName(QString(str.c_str()));
    // pElement->SetDescription(QObject::tr("double类型"));
    pElement->SetDescription(describe);
    // pElement->SetAttribute() 设置一些自定义的属性

    // 默认值
    pElementNumber->SetDefaultValue(0);

    // 不允许 删除 移动位置
    pElement->Grants() &= ~(ElementGrant_Delectable | ElementGrant_Movetable);

    // 设置ddx 输入输出通道属性
    DDXChannelInfoStruct ddxInfo;
    ddxInfo["name"] = QString(str.c_str());
    ddxInfo["id"] = QString::number(_beginElementChannelId);
    //	qDebug() << "id " << QString(str.c_str()) << ":" << _beginElementChannelId;
    ++_beginElementChannelId;
    pElement->SetDDXInputChannelAdpterType(ADAPTER_TYPE_FOR_BLOCK);
    pElement->SetDDXInputChannelEnable(true);
    pElement->SetDDXInputChannelInfo(ddxInfo);
    pElement->SetDDXOutputChannelAdpterType(ADAPTER_TYPE_FOR_BLOCK);
    pElement->SetDDXOutputChannelEnable(true);
    pElement->SetDDXOutputChannelInfo(ddxInfo);
    parentStruct->PushBack(pElement);

    PIDataDictionaryBuilder CurrentRunDictionary = _dataDicServer->GetCurrentRuntimeDictionary();
    if (CurrentRunDictionary) {
        if (CurrentRunDictionary->GetId() == dictionaryBuilder->GetId()) {
            if (!pElement->IsRuntime()) {
                QString msg;
                pElement->BuildMemory(msg);
                // pElementList.push_back(pElement);
            }
        }
    }
}

PIElementBase SimuNPSDataDictionaryServer::FindDataDictionary(const PIDataDictionaryBuilder &dictionaryBuilder,
                                                              QString &Findpath, QString Elementname)
{
    Findpath = Findpath + "." + Elementname;
    PIElementBase Elementbase = dictionaryBuilder->FindElement(Findpath);
    return Elementbase;
}

bool SimuNPSDataDictionaryServer::StopDataDictionary()
{
    // int oldstatus = _runStatuscount;
    //_runStatuscount--;
    //  if(!isStatuschange(oldstatus,_runStatuscount))     //状态未变化 不是真的停止
    //	 return false;
    if (!IsRuning())
        return false;
    // 停止数据字典
    if (0 == currentdicId)
        return false;

    if (_dataDicServer) {
        _dataDicServer->StopRunTime();
    }
    setRunStatus(false);
    _dicEleChannelMap.clear();

    // 停止运行后,执行缓存的动作
    //  loadBufferAction();
    return true;
}

// bool SimuNPSDataDictionaryServer::isStatuschange(int oldStatus,int newStatus)
//{
//	if(oldStatus==0&&newStatus==1)    //上升 数据字典run
//	{
//		setRunStatus(true);
//		return true;
//	}
//	if(oldStatus==1&&newStatus==0)    //下降 数据字典stop
//	{
//		setRunStatus(false);
//		return true;
//	}
//	return false;
// }

// void SimuNPSDataDictionaryServer::saveActionToBuffer(actionType Type,const QString &variableName,const QString
// &variableType,const QString &describe)
//{
//	BufferData data;
//	data.action = Type;
//	data.variableName = variableName;
//	data.variableType  = variableType;
//	data.describe  = describe;
//	vectorBuffer.push_back(data);
// }

// void SimuNPSDataDictionaryServer::loadBufferAction()
//{
//	int ActionNum = vectorBuffer.size();
//	for(int i=0;i<ActionNum;i++)
//	{
//		BufferData data = vectorBuffer[i];
//		if(data.action==Action_Add)
//		{
//			AddDataDictionary(data.variableName,data.variableType,data.describe);
//		}
//		else if(data.action==Action_Del)
//		{
//			 DelDataDictionary(data.variableName);
//		}
//	}
//	vectorBuffer.clear();
// }

bool SimuNPSDataDictionaryServer::isHaveChildNode(PIElementBase Element)
{
    if (Element->ElementType().testFlag(ElementType_Struct)) // 目前只考虑结构体
    {
        PIElementStruct pElementStruct = QueryInterface<IElementStruct>(Element);
        if (pElementStruct->ChildCount() == 0) {
            return false;
        }
    }
    return true;
}

void SimuNPSDataDictionaryServer::delNotHaveChildNodeElement(PIElementBase pElement)
{
    PIElementBase Element = pElement;
    while (1) {
        if (Element->Name() == "Dictionary") // 如果Element是root节点,退出
        {
            return;
        }
        if (isHaveChildNode(Element)) // 该元素含有子节点,则退出不操作
            return;
        PIElementBase ParentElement = Element->Parent();
        PIElementStructBuilder structParent = QueryInterface<IElementStructBuilder>(ParentElement);
        Element->Grants() |= ElementGrant_Delectable;     // 设置删除权限
        ParentElement->Grants() |= ElementGrant_DelChild; // 设置父节点删除子节点权限
        // 在其父节点上将其移除
        bool bRemove = structParent->Remove(Element);
        Element = ParentElement;
    }
}

bool SimuNPSDataDictionaryServer::ClearAllCurrentDataDictionary()
{
    if (currentdicId == 0) {
        LOGOUT(QObject::tr(
                "Initialize the data dictionary by adding the dictionary element first")); // 请先添加字典元素初始化数据字典
    }

    unsigned long long dicId = currentdicId;
    PIDataDictionaryBuilder dictionaryBuilder = _dataDicServer->GetDictionaryBuilder(dicId);
    if (!dictionaryBuilder)
        return false;
    PIElementBase root = dictionaryBuilder->GetRoot();
    if (!root)
        return false;
    PIElementStructBuilder structRootBuilder = QueryInterface<IElementStructBuilder>(root);
    PIElementStruct rootStruct = QueryInterface<IElementStruct>(root);
    if ((!structRootBuilder) || (!rootStruct))
        return false;

    QStringList childnames = rootStruct->ChildNames();
    // 删除前先停止运行数据字典
    StopDataDictionary();
    for (int i = 0; i < childnames.count(); i++) {
        bool bAdd = DelDataDictionaryHandle(dictionaryBuilder, childnames[i]);
    }
    RunDataDictionary();
    // 清除用来对接通道的map,map存储元素的路径
    _dicEleChannelMap.clear();

    // structRootBuilder->Clear();    //平台清除了,  字典元素内存平台未释放,自己手动释放

    // 手动释放内存
    //  PIDataDictionaryBuilder CurrentRunDictionary= _dataDicServer->GetCurrentRuntimeDictionary();
    //  if(CurrentRunDictionary)
    //{
    //	if(CurrentRunDictionary->GetId()==dictionaryBuilder->GetId())
    //	{
    //		int pElementNum = pElementList.count();
    //		for (int i = 0;i <pElementNum;i++)
    //		{
    //			if(pElementList[i]->IsRuntime())
    //			{
    //				QString msg;
    //				pElementList[i]->ReleaseMemory(msg);
    //			}
    //		}
    //	}
    // }
    return true;
}

QStringList SimuNPSDataDictionaryServer::GetAllCurrentDataDictionary()
{
    if (currentdicId == 0) {
        LOGOUT(QObject::tr("Unable to obtain all dictionary elements, add dictionary elements to initialize the data "
                           "dictionary first")); // 无法获取所有字典元素,请先添加字典元素初始化数据字典
    }

    QStringList elementList;

    unsigned long long dicId = currentdicId;
    PIDataDictionaryBuilder dictionaryBuilder = _dataDicServer->GetDictionaryBuilder(dicId);
    if (!dictionaryBuilder)
        return QStringList();
    PIElementBase root = dictionaryBuilder->GetRoot();
    if (!root)
        return QStringList();
    PIElementStruct rootStruct = QueryInterface<IElementStruct>(root);
    if (!rootStruct)
        return QStringList();
    QString simulationName = Kcc::BlockDefinition::NPS_DataDictionaryNodeName; // fixme 从数据服务中获取 "当前仿真"
    if (simulationName.isEmpty())
        return QStringList();

    PIElementBase _1Level = rootStruct->ChildByName(simulationName);
    if (!_1Level)
        return QStringList();

    PIElementStruct _1LevelStruct = QueryInterface<IElementStruct>(_1Level);
    QStringList boardNames = _1LevelStruct->ChildNames();

    for (int i = 0; i < boardNames.count(); i++) {
        QString elementPath = boardNames[i];
        PIElementBase boardElementBase = _1LevelStruct->ChildByName(boardNames[i]);

        PIElementStruct boardElementStruct = QueryInterface<IElementStruct>(boardElementBase);
        QStringList blockNames = boardElementStruct->ChildNames();

        for (int j = 0; j < blockNames.count(); j++) {
            QString blocklementPath = elementPath + "." + blockNames[j];
            PIElementBase blockElementBase = boardElementStruct->ChildByName(blockNames[j]);

            PIElementStruct blockElementStruct = QueryInterface<IElementStruct>(blockElementBase);
            QStringList portNames = blockElementStruct->ChildNames();

            for (int p = 0; p < portNames.count(); p++) {
                QString portelementPath = blocklementPath + "." + portNames[p];
                elementList << portelementPath;
            }
        }
    }
    return elementList;
}

QStringList SimuNPSDataDictionaryServer::GetDictionaryElement(const QString &variableName)
{
    if (currentdicId == 0) {
        LOGOUT(QObject::tr(
                "Initialize the data dictionary by adding the dictionary element first")); // 请先添加字典元素初始化数据字典
    }

    unsigned long long dicId = currentdicId;
    PIDataDictionaryBuilder dictionaryBuilder = _dataDicServer->GetDictionaryBuilder(dicId);
    if (!dictionaryBuilder)
        return QStringList();
    QString findElement = DICTIONARY_ROOT + variableName;
    PIElementBase element = dictionaryBuilder->FindElement(findElement);
    if (!element) {
        // LOGOUT(QObject::tr("The data dictionary does not contain dictionary elements")
        //        + QString(":%1").arg(variableName)); // 数据字典未含有字典元素
        return QStringList();
    }

    QStringList elementList;
    GetDictionaryElementHandle(element, elementList);
    return elementList;
}

bool SimuNPSDataDictionaryServer::RenameDictionaryElement(const QString &elementName, const QString &newName)
{
    if (0 == currentdicId) {
        LOGOUT(QObject::tr(
                "No data dictionary exists. Please initialize the data dictionary first")); // 当前无数据字典,请先初始化数据字典
    }
    unsigned long long dicId = currentdicId;
    PIDataDictionaryBuilder dictionaryBuilder = _dataDicServer->GetDictionaryBuilder(dicId);
    if (!dictionaryBuilder) {
        return false;
    }

    if (!isHaveDictionaryElement(elementName)) {
        return false;
    }

    // 若为四层结构进入这里面处理
    if (renameDictionaryElementStartWith(elementName, newName)) {
        return true;
    }

    QString findpath = DICTIONARY_ROOT + elementName;
    PIElementBase element = dictionaryBuilder->FindElement(findpath);
    if (!element) {
        // LOGOUT(QObject::tr("Rename failed because the dictionary element was not found"),
        //        LOG_WARNING); // 重命名失败,未查找到该字典元素
        return false;
    }
    // 找到了该元素,重命名该元素
    element->SetName(newName);

    NotifyStruct notify;
    notify.code = Notify_ElementRenameed;
    notify.paramMap["oldName"] = elementName;
    notify.paramMap["newName"] = element->FullName().remove(DICTIONARY_ROOT);
    emitNotify(notify);

    return true;
}

bool SimuNPSDataDictionaryServer::isHaveDictionaryElement(const QString &variableName)
{
    if (0 == currentdicId) {
        LOGOUT(QObject::tr(
                "No data dictionary exists. Please initialize the data dictionary first")); // 当前无数据字典,请先初始化数据字典
    }
    unsigned long long dicId = currentdicId;
    PIDataDictionaryBuilder dictionaryBuilder = _dataDicServer->GetDictionaryBuilder(dicId);
    if (!dictionaryBuilder)
        return false;

    QString findpath = DICTIONARY_ROOT + variableName;
    PIElementBase element = dictionaryBuilder->FindElement(findpath);
    if (element)
        return true;
    else
        return false;
}

void SimuNPSDataDictionaryServer::releaseDoubleMemory(PIElementBase Element)
{
    if (Element->ElementType().testFlag(ElementType_Number)) {
        // 删除_dicEleChannelMap中元素
        releasedicEleChannelMap(Element);
        if (Element->IsRuntime()) {
            QString msg;
            // 释放元素空间
            Element->ReleaseMemory(msg);
            return;
        }
    } else if (Element->ElementType().testFlag(ElementType_Struct)) {
        PIElementStruct structElement = QueryInterface<IElementStruct>(Element);
        for (int i = 0; i < structElement->ChildCount(); i++) {
            PIElementBase childElement = structElement->Child(i);
            releaseDoubleMemory(childElement);
        }
    }
}

void SimuNPSDataDictionaryServer::releasedicEleChannelMap(PIElementBase Element)
{
    QString name = Element->FullName();
    if (_dicEleChannelMap.contains(Element->FullName()))
        _dicEleChannelMap.remove(Element->FullName());
}

void SimuNPSDataDictionaryServer::GetDictionaryElementHandle(PIElementBase element, QStringList &elementList)
{
    if (element->ElementType().testFlag(ElementType_Number)) // element为第四层元素时
    {
        QString elementName = element->FullName();
        elementList << elementName;
        return;
    } else // 不为第四层元素则递归直至第四层
    {
        PIElementStruct elementStruct = QueryInterface<IElementStruct>(element);
        for (int i = 0; i < elementStruct->ChildCount(); i++) {
            PIElementBase childElement = elementStruct->Child(i);
            GetDictionaryElementHandle(childElement, elementList);
        }
    }
}

bool SimuNPSDataDictionaryServer::renameDictionaryElementStartWith(const QString &elementName, const QString &newName)
{
    if (0 == currentdicId) {
        LOGOUT(QObject::tr(
                "No data dictionary exists. Please initialize the data dictionary first")); // 当前无数据字典,请先初始化数据字典
    }
    unsigned long long dicId = currentdicId;
    PIDataDictionaryBuilder dictionaryBuilder = _dataDicServer->GetDictionaryBuilder(dicId);
    if (!dictionaryBuilder) {
        return false;
    }

    QStringList variableNameList = elementName.split(".");
    if (variableNameList.length() != 4) {
        return false;
    }

    QString varPrefixName = variableNameList.last();
    QString nrePrefixName;
    int index = varPrefixName.lastIndexOf("/");
    if (index == -1) {
        nrePrefixName = newName;
    } else {
        nrePrefixName = varPrefixName.mid(0, index + 1) + newName;
    }

    QStringList elements = GetDictionaryElement(
            QString("%1.%2.%3").arg(variableNameList[0]).arg(variableNameList[1]).arg(variableNameList[2]));
    for (auto element : elements) {
        if (!checkDictionaryName(element, DICTIONARY_ROOT + elementName)) {
            continue;
        }
        PIElementBase pBase = dictionaryBuilder->FindElement(element);
        if (!pBase) {
            continue;
        }
        // 找到了该元素,重命名该元素
        QString oldFullName = pBase->FullName();
        QString testNew = pBase->Name().replace(0, varPrefixName.length(), nrePrefixName);
        pBase->SetName(testNew);

        NotifyStruct notify;
        notify.code = Notify_ElementRenameed;
        notify.paramMap["oldName"] = oldFullName.remove(DICTIONARY_ROOT);
        notify.paramMap["newName"] = pBase->FullName().remove(DICTIONARY_ROOT);
        emitNotify(notify);
    }
    return true;
}

bool SimuNPSDataDictionaryServer::checkDictionaryName(const QString &checkName, const QString &parentName)
{
    if (checkName.isEmpty() || parentName.isEmpty()) {
        return false;
    }

    // 必须都是四层结构
    QStringList checkList = checkName.split(".");
    QStringList parentList = parentName.split(".");
    // 加上平台是五层
    if (checkList.size() != 5 || parentList.size() != 5) {
        return false;
    }

    QStringList checkLastList = checkList.last().split("/");
    QStringList parentLastList = parentList.last().split("/");

    if (parentLastList.size() > checkLastList.size()) {
        return false;
    }

    for (int i = 0; i < parentLastList.size(); i++) {
        if (parentLastList.at(i) != checkLastList.at(i)) {
            return false;
        }
    }

    return true;
}
