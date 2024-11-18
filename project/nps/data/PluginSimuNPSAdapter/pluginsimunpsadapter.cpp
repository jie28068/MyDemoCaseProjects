#include "pluginsimunpsadapter.h"

#include "CoreLib/ServerManager.h"
#include "SimuNPSDataDictionaryContentLoader.h"
#include "SimuNPSDataDictionaryServer.h"
#include "server/DataDictionary/IDataDictionaryViewServer.h"

using namespace Kcc;
using namespace Kcc::DataDictionary;

USE_LOGOUT_("PluginSimuNPSAdapter")

REG_MODULE_BEGIN(PluginSimuNPSAdapter, "Plugin", "PluginSimuNPSAdapter")
REG_MODULE_END(PluginSimuNPSAdapter)

PluginSimuNPSAdapter::PluginSimuNPSAdapter(QString strName, QObject *parent)
    : QObject(parent), Module(Module_Type_Normal, strName)
{
    RegServer<ISimuNPSDataDictionaryServer>(new SimuNPSDataDictionaryServer());
}

PluginSimuNPSAdapter::~PluginSimuNPSAdapter()
{
    _targetAdpter.clear();
}

void PluginSimuNPSAdapter::init(KeyValueMap &params)
{
    Module::init(params);

    PIDataDictionaryServer dataDictionaryServer = RequestServer<IDataDictionaryServer>();
    if (dataDictionaryServer) {
        PIDataDictionaryContentLoader contentLoader =
                PIDataDictionaryContentLoader(new SimuNPSDataDictionaryContentLoader());
        dataDictionaryServer->BuildDictionaryStorage(contentLoader);
    } else
        LOGOUT(tr("Failed to obtain the DataDictionaryServer service!"),
               LOG_WARNING); // DataDictionaryServer服务获取失败!

    PIDataDictionaryDDXServer dataDictionaryDDXServer = RequestServer<IDataDictionaryDDXServer>();

    _targetAdpter = QSharedPointer<DDXTargetAdpterForBlock>(new DDXTargetAdpterForBlock(dataDictionaryDDXServer));

    if (dataDictionaryDDXServer) {
        dataDictionaryDDXServer->AddDDXTargetAdpter(_targetAdpter);
    } else
        LOGOUT(tr("Failed to obtain the DataDictionaryDDXServer service!"),
               LOG_WARNING); // DataDictionaryDDXServer服务获取失败!

    // 操作数据字典视图的默认控制器
    PIServerInterfaceBase dataDictionaryViewServer = RequestServerInterface<IDataDictionaryViewServer>();
    if (dataDictionaryViewServer) {
        dataDictionaryViewServer->connectNotify(
                Notify_DefaultElementTreeViewController_Created, this,
                SLOT(onDataDictionaryViewServerNotify(unsigned int, const NotifyStruct &)));

        // dataDictionaryViewServer->connectNotify(Notify_TreeView_ClickedTreeViewItem,
        // this, SLOT(onDataDictionaryViewServerNotify(unsigned int, const NotifyStruct &)) );

        dataDictionaryViewServer->connectNotify(
                Notify_TreeView_DoubleClickedTreeViewItem, this,
                SLOT(onDataDictionaryViewServerNotify(unsigned int, const NotifyStruct &)));
    } else
        LOGOUT(tr("DataDictionaryViewServer service failed to be obtained!"),
               LOG_WARNING); // DataDictionaryViewServer服务获取失败!

    // QTimer::singleShot(2000, this, SLOT(TestDataDicServer()));
}

void PluginSimuNPSAdapter::unInit(KeyValueMap &saveParams)
{
    PISimuNPSDataDictionaryServer server = RequestServer<ISimuNPSDataDictionaryServer>();
    if (server) {
        server->QuitRunDataDictionary();
    }

    Module::unInit(saveParams);
}

void PluginSimuNPSAdapter::onDataDictionaryViewServerNotify(unsigned int code, const NotifyStruct &cmdStruct)
{
    if (code == Notify_DefaultElementTreeViewController_Created) {
        PIDataDictionaryViewServer dataDictionaryViewServer = RequestServer<IDataDictionaryViewServer>();

        if (dataDictionaryViewServer) {
            IElementTreeViewController *pTreeViewControl =
                    dataDictionaryViewServer->GetDefaultElementTreeViewController();
            if (pTreeViewControl) {
                // pTreeViewControl->SetShowHeader(0);
                pTreeViewControl->EnableDoubleClickedItemNotify(true);
                // pTreeViewControl->EnableClickedItemNotify(true);
                pTreeViewControl->EnableDisplayVariableValue(true);
                pTreeViewControl->EnableShowProperty(true);
                pTreeViewControl->SetEditable(true);
                // todo 是否需要自定义 元素图表 和 toolTip显示文字内容格式
                // pTreeViewControl->SetUserStyleImpl();
            }
        }
    } else if (code == Notify_TreeView_ClickedTreeViewItem) {
        qulonglong cid = cmdStruct.paramMap["ControllerId"].toULongLong();
        qulonglong dicid = cmdStruct.paramMap["DictionaryId"].toULongLong();
        QString fullPath = cmdStruct.paramMap["ElementFullName"].toString();

        PIDataDictionaryServer server = RequestServer<IDataDictionaryServer>();
        if (server) {
            PIDataDictionaryBuilder dic = server->GetCurrentRuntimeDictionary();
            PIElementBase element = dic->FindElement(fullPath);
            Q_ASSERT(element);
            if (element->RefToVariableCount() == 0) {
                PIVariableBase var = element->RefToVariable();
                PIVariableNumber number = var->ToVariableNumber();
                LOGOUT(QObject::tr("Dictionary element")
                       + QString("[%1] value[%2] refCount[%3]")
                                 .arg(fullPath)
                                 .arg(number->Value().toDouble())
                                 .arg(element->RefToVariableCount())); // 字典元素
            }
        }
    } else if (code == Notify_TreeView_DoubleClickedTreeViewItem) {
        qulonglong cid = cmdStruct.paramMap["ControllerId"].toULongLong();
        qulonglong dicid = cmdStruct.paramMap["DictionaryId"].toULongLong();
        QString fullPath = cmdStruct.paramMap["ElementFullName"].toString();
        return;
    }
}

void PluginSimuNPSAdapter::TestDataDicServer()
{
    // 测试
    return;

    PISimuNPSDataDictionaryServer server = RequestServer<ISimuNPSDataDictionaryServer>();
    if (server) {
        server->BuildAndRunDataDictionary("1121");
    }
}
