#include "PluginModelManagerModule.h"
#include "CoreLib/ServerManager.h"
#include "ModelManagerViewServerMng.h"
#include "TreeModelMng.h"

REG_MODULE_BEGIN(PluginModelManagerModule, "", "PluginModelManagerModule")
REG_MODULE_END(PluginModelManagerModule)

USE_LOGOUT_("PluginModelManagerView")

PluginModelManagerModule::PluginModelManagerModule(QString moduleName)
    : Module(Module_Type_Normal, moduleName), m_modelView(nullptr)
{
    // 注册服务
    m_pModelManagerServer = new ModelManagerServer();
    RegServer<IModelManagerServer>(m_pModelManagerServer);
}

PluginModelManagerModule::~PluginModelManagerModule()
{
    if (m_modelView) {
        delete m_modelView;
    }
}

void PluginModelManagerModule::init(KeyValueMap &params)
{
    if (!ModelManagerViewServerMng::getInstance().initServerMng()) {
        LOGOUT(tr("ModelManagerViewServerMng init false"), LOG_ERROR);
        return;
    }

    TreeModelMng::getInstance().initTreeModel();

    // 创建树形管理窗口
    m_modelWidget = new ModelTreeWidget(this, true);

    // 创建模型管理视图
    m_modelView = new ModelManagerView();

    // 布局
    QVBoxLayout *vLayout = new QVBoxLayout;
    vLayout->setContentsMargins(0, 0, 0, 0);
    vLayout->addWidget(m_modelWidget);
    this->setLayout(vLayout);
}

void PluginModelManagerModule::unInit(KeyValueMap &saveParams)
{
    TreeModelMng::getInstance().saveTreeModel();

    ModelManagerViewServerMng::getInstance().unInitServerMng();
}