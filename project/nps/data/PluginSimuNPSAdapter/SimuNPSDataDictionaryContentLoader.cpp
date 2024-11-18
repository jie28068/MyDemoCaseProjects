#include "SimuNPSDataDictionaryContentLoader.h"
#include "CoreLib/ServerManager.h"
#include "server/SimuNPSAdapter/def.h"
#include "Json/json.h"

USE_LOGOUT_("SimuNPSDataDictionaryContentLoader")

SimuNPSDataDictionaryContentLoader::SimuNPSDataDictionaryContentLoader(void) { }

SimuNPSDataDictionaryContentLoader::~SimuNPSDataDictionaryContentLoader(void) { }

bool SimuNPSDataDictionaryContentLoader::Load(const PIDataDictionaryBuilder &dictionaryBuilder,
                                              const DictionaryParamMap &dictionaryParam)
{
    if (!dictionaryBuilder)
        return false;
    PIElementBase root = dictionaryBuilder->GetRoot();
    if (!root)
        return false;
    PIElementStructBuilder structRoot = QueryInterface<IElementStructBuilder>(root);
    if (!structRoot)
        return false;

    // todo 准备加载字典 把simuNPS的数据结构转变为 dictionaryBuilder 的字典元素
    QString jsonStr = dictionaryParam["json"];

    /*
    QString jsonStr;
    QFile file("C:/Users/KLJS381/Desktop/json.txt");
    if(file.open(QIODevice::ReadOnly))
    {
            jsonStr = file.readAll();
            file.close();
    }
    */
    Json::Reader reader;
    Json::Value rootValue;
    if (!reader.parse(jsonStr.toStdString(), rootValue)) {
        LOGOUT(QObject::tr("unable to parse"), LOG_ERROR); // 无法解析
        return false;
    }

    int newLevelSize = rootValue.size(); // 新增第一层 ，共四层
    Json::Value::Members newLeveNames = rootValue.getMemberNames();
    for (int newLevenum = 0; newLevenum < newLevelSize; ++newLevenum) {
        std::string newLevelName = newLeveNames[newLevenum];
        // 创建结构元素
        PIElementStructBuilder newLeveStruct = createBuilderElement(dictionaryBuilder, structRoot, newLevelName);
        Json::Value newLeveValue = rootValue[newLevelName];

        int boardSize = newLeveValue.size();
        Json::Value::Members boardNames = newLeveValue.getMemberNames();
        for (int iboard = 0; iboard < boardSize; ++iboard) // 画板
        {
            std::string boardName = boardNames[iboard];
            // 创建结构元素
            PIElementStructBuilder boardStruct = createBuilderElement(dictionaryBuilder, newLeveStruct, boardName);
            Json::Value board = newLeveValue[boardName];
            int contentSize = board.size();
            Json::Value::Members contentNames = board.getMemberNames();
            for (int iContent = 0; iContent < contentSize; ++iContent) // 画板内部元素
            {
                std::string contentName = contentNames[iContent];
                // 创建结构元素
                PIElementStructBuilder contentStruct =
                        createBuilderElement(dictionaryBuilder, boardStruct, contentName);
                Json::Value level2Value = board[contentName];

                if (level2Value.isObject()) {
                    Json::Value::Members merbers = level2Value.getMemberNames(); // name
                    int mSize = merbers.size();
                    for (int iM = 0; iM < mSize; ++iM) {
                        std::string dataName = merbers[iM];
                        Json::Value level3Value = level2Value[dataName];
                        std::string dataType = level3Value.asString(); // double
                        if (0 == dataType.compare("double"))           // 创建double元素
                            createDoubleElement(dictionaryBuilder, contentStruct, dataName);
                    }

                } else if (level2Value.isArray()) {
                    int arraySize = level2Value.size();
                    for (int iArray = 0; iArray < arraySize; ++iArray) {
                        Json::Value defaultValue;
                        Json::Value iValue = level2Value.get(iArray, defaultValue);
                        Json::Value::Members merbers = iValue.getMemberNames();
                        int mSize = merbers.size();
                        for (int iM = 0; iM < mSize; ++iM) {
                            std::string dataName = merbers[iM];
                            Json::Value level3Value = iValue[dataName];
                            std::string dataType = level3Value.asString(); // double
                            if (0 == dataType.compare("double"))           // 创建double元素
                                createDoubleElement(dictionaryBuilder, contentStruct, dataName);
                        }
                    }
                }
            }
        }
    }
    return true;
}

bool SimuNPSDataDictionaryContentLoader::Save(const PIDataDictionaryBuilder &dictionaryBuilder,
                                              const DictionaryParamMap &dictionaryParam)
{
    // todo 字典结构保存

    return true;
}

PIElementStructBuilder
SimuNPSDataDictionaryContentLoader::createBuilderElement(const PIDataDictionaryBuilder &dictionaryBuilder,
                                                         const PIElementStructBuilder &parentStruct, std::string str)
{
    PIElementBase pElement = dictionaryBuilder->CreateElement(parentStruct, ElementStructBuilder_RegName, DonotCreated);
    Q_ASSERT(pElement);
    PIElementStructBuilder pElementStruct = QueryInterface<IElementStructBuilder>(pElement);
    Q_ASSERT(pElementStruct);
    pElement->SetName(QString(str.c_str()));
    pElement->SetAliaseName(QString(str.c_str()));

    // 设置ddx 输入输出通道属性
    DDXChannelInfoStruct ddxInfo;
    PISimuNPSDataDictionaryServer _npsdataDictionaryserver = RequestServer<ISimuNPSDataDictionaryServer>();
    int _beginElementChannelId = _npsdataDictionaryserver->GetValidElementChannelId();
    ddxInfo["name"] = QString(str.c_str());
    ddxInfo["id"] = QString::number(_beginElementChannelId);
    ++_beginElementChannelId;
    _npsdataDictionaryserver->SetValidElementChannelId(_beginElementChannelId);
    pElement->SetDDXInputChannelAdpterType(ADAPTER_TYPE_FOR_BLOCK);
    pElement->SetDDXInputChannelEnable(true);
    pElement->SetDDXInputChannelInfo(ddxInfo);
    pElement->SetDDXOutputChannelAdpterType(ADAPTER_TYPE_FOR_BLOCK);
    pElement->SetDDXOutputChannelEnable(true);
    pElement->SetDDXOutputChannelInfo(ddxInfo);
    parentStruct->PushBack(pElement);

    return pElementStruct;
}

void SimuNPSDataDictionaryContentLoader::createDoubleElement(const PIDataDictionaryBuilder &dictionaryBuilder,
                                                             const PIElementStructBuilder &parentStruct,
                                                             std::string str)
{
    PIElementBase pElement = dictionaryBuilder->CreateElement(parentStruct, ElementNumber_RegName, AlwaysCreated);
    Q_ASSERT(pElement);
    PIElementNumber pElementNumber = QueryInterface<IElementNumber>(pElement);
    Q_ASSERT(pElementNumber);
    pElementNumber->SetVariantType(DVariable_Double);

    pElement->SetName(QString(str.c_str()));
    pElement->SetAliaseName(QString(str.c_str()));
    pElement->SetDescription(QObject::tr("double type")); // double类型
    // pElement->SetAttribute() 设置一些自定义的属性

    // 默认值
    pElementNumber->SetDefaultValue(0);

    // 不允许 删除 移动位置
    pElement->Grants() &= ~(ElementGrant_Delectable | ElementGrant_Movetable);

    // 设置ddx 输入输出通道属性
    DDXChannelInfoStruct ddxInfo;

    ddxInfo["name"] = QString(str.c_str());
    PISimuNPSDataDictionaryServer _npsdataDictionaryserver = RequestServer<ISimuNPSDataDictionaryServer>();
    int _beginElementChannelId = _npsdataDictionaryserver->GetValidElementChannelId();
    ddxInfo["id"] = QString::number(_beginElementChannelId);
    //	qDebug() << "id " << QString(str.c_str()) << ":" << _beginElementChannelId;
    ++_beginElementChannelId;
    _npsdataDictionaryserver->SetValidElementChannelId(_beginElementChannelId);
    pElement->SetDDXInputChannelAdpterType(ADAPTER_TYPE_FOR_BLOCK);
    pElement->SetDDXInputChannelEnable(true);
    pElement->SetDDXInputChannelInfo(ddxInfo);
    pElement->SetDDXOutputChannelAdpterType(ADAPTER_TYPE_FOR_BLOCK);
    pElement->SetDDXOutputChannelEnable(true);
    pElement->SetDDXOutputChannelInfo(ddxInfo);
    parentStruct->PushBack(pElement);

    // PIDataDictionaryServer _dataDicServer = RequestServer<IDataDictionaryServer>(SERVER_GROUP_DATADICTIONARY_NAME,
    // SERVER_INTERFACE_DATADICTIONARY_NAME); if(!_dataDicServer)
    //{
    //	LOGOUT("DataDictionaryServer服务获取失败!", LOG_WARNING);
    // }
    // 数据字典先add进入这,这里数据字典还未run,不需要分配内存
    // PIDataDictionaryBuilder CurrentRunDictionary= _dataDicServer->GetCurrentRuntimeDictionary();
    // if(CurrentRunDictionary)
    //{
    //	if(CurrentRunDictionary->GetId()==dictionaryBuilder->GetId())
    //	{
    //		if (!pElement->IsRuntime())
    //		{
    //			QString msg;
    //			pElement->BuildMemory(msg);
    //			pElementList.push_back(pElement);
    //		}
    //	}
    // }
}
