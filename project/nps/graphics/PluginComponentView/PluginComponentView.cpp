#include "PluginComponentView.h"
#include "CodeManagerServer/ICodeManagerServer.h"
#include "CombineBoardModel.h"
#include "ComplexBoardModel.h"
#include "ControlBoardModel.h"
#include "CoreLib/ServerManager.h"
#include "ElecBoardModel.h"
#include "ImportBlockProperty.h"
#include "KLWidgets/KCustomDialog.h"
#include "PluginComponentServer.h"
#include <QBitmap>
#include <QBoxLayout>
#include <QDebug>
#include <QDir>
#include <QTranslator>
#include <QVariant>

using namespace Global;

static const int LAYOUT_MARGIN_TOP = 10;
static const int LAYOUT_MARGIN_LEFT = 10;
static const int LAYOUT_MARGIN_RIGHT = 10;
static const int LAYOUT_MARGIN_BOTTOM = 10;

static const QString TABCLASSTYPE_CANVASWIDGET = QString("CanvasWidget");
static const QString TABCLASSTYPE_MONITORPANEL = QString("MonitorPanel");
/**
静态注册（当dll被加载时会调用），每个模块都必须使用此宏（第一个参数是当前类的类型；第二个是组的名称，
一般为空；第三个是显示名称，一般是类型名称）
**/
REG_MODULE_BEGIN(PluginComponentView, "", "PluginComponentView")
REG_MODULE_END(PluginComponentView)

USE_LOGOUT_("PluginComponentView")

PluginComponentView::PluginComponentView(QString strName)
    : Module(Module_Type_Normal, strName),
      m_category(Unknow),
      m_pToolBoxModel(nullptr),
      m_pCfgInfoManager(),
      m_pConfigInfo(nullptr),
      m_strProjectName(""),
      m_pComponentServer(nullptr)
{
    ui.setupUi(this);
    m_pToolBox = new MyToolBox(ui.widget);
    QVBoxLayout *hLayout = new QVBoxLayout(ui.widget);
    hLayout->setContentsMargins(0, 0, 0, 0);
    hLayout->addWidget(m_pToolBox);
    ui.widget->setLayout(hLayout);

    // 注册服务
    PropertyMap map;
    map.insert("name", strName);
    m_pComponentServer = new PluginComponentServer(this);
    RegServer<IPluginComponentServer>(m_pComponentServer, map);

    m_pContextMenuManager = nullptr;
}

PluginComponentView::~PluginComponentView()
{   
}

void PluginComponentView::init(KeyValueMap &params)
{
    // 用QObject::tr包装字符串，平台会加载对应的qm文件翻译
    m_strModuleName = QObject::tr(params["ModuleName"].toUtf8());
    m_category = categoryFromName(m_strModuleName);
    m_strProjectName = QObject::tr(params["ProjectName"].toUtf8());

    // 绑定右键菜单
    bindMenu(m_strModuleName);
    // 初始化需要用到的服务
    initServer();
    m_pCfgInfoManager = PConfigInfoManager(new ConfigInfoManager(m_pModelServer,m_pProjectServer));
    if (m_pCfgInfoManager != nullptr) {
        if (m_pCfgInfoManager->initGroup(m_category)) {
            m_pConfigInfo = m_pCfgInfoManager->getConfigInfo(m_category);
        }
    }
    // 初始化数据
    initComponentData();
    // 默认显示列表视图
    ui.stackedWidget->setCurrentIndex(1);
}

void PluginComponentView::bindMenu(const QString &m_strModuleName)
{
    // 绑定右键菜单
    m_pContextMenuManager = ContextMenuManager::instance(m_strModuleName);
    QObject::connect(m_pContextMenuManager, SIGNAL(addCodeComponent()), this, SLOT(onAddCodeComponent()));
    QObject::connect(m_pContextMenuManager, SIGNAL(importCodeComponent()), this, SLOT(onImportCodeComponent()));
    QObject::connect(m_pContextMenuManager, SIGNAL(removeCodeComponent(QString)), this, SLOT(onRemoveModule(QString)));
    QObject::connect(m_pContextMenuManager, SIGNAL(ModifyControlComponent(QString)), this,
                     SLOT(onModifyControlComponent(QString)));
    QObject::connect(m_pContextMenuManager, SIGNAL(addBoardComponent()), this, SLOT(onAddBoardComponent()));
    QObject::connect(m_pContextMenuManager, SIGNAL(removeBoardComponent(QString)), this, SLOT(onRemoveModule(QString)));
    QObject::connect(m_pContextMenuManager, SIGNAL(modifyBoardComponent(QString)), this,
                     SLOT(onModifyBoardComponent(QString)));
    QObject::connect(m_pContextMenuManager, SIGNAL(refreshView()), this, SLOT(onRefreshView()));
    QObject::connect(m_pContextMenuManager, SIGNAL(addBuildInComponent(int)), this, SLOT(onAddBuildInComponent(int)));
    QObject::connect(m_pContextMenuManager, SIGNAL(expandAll()), this, SLOT(onExpandAll()));
    QObject::connect(m_pContextMenuManager, SIGNAL(collapseAll()), this, SLOT(onCollapseAll()));

    QObject::connect(m_pContextMenuManager, SIGNAL(adjustListView()), this, SLOT(onAdjustListView()));
    QObject::connect(m_pContextMenuManager, SIGNAL(addComponentToBoard(const QString &)), this,
                     SLOT(onAddComponentToBoard(const QString &)));
    QObject::connect(m_pContextMenuManager, SIGNAL(createComponentInstance(const QString &)), this,
                     SLOT(onCreateComponentInstance(const QString &)));

    QObject::connect(m_pContextMenuManager, SIGNAL(addElcBuildInComponent(int)), this,
                     SLOT(onAddElcBuildInComponent(int)));
    QObject::connect(m_pContextMenuManager, SIGNAL(changeElcBuildInComponent(QString)), this,
                     SLOT(onChangeElcBuildInComponent(QString)));
    QObject::connect(m_pContextMenuManager, SIGNAL(removeElcBuildInComponent(QString)), this,
                     SLOT(onRemoveModule(QString)));

    QObject::connect(m_pContextMenuManager, SIGNAL(showHelp(const QString &)), this, SLOT(onShowHelp(const QString &)));
}

void PluginComponentView::unInit(KeyValueMap &saveParams)
{
    // 反初始化的时候记得要clear一下
    m_pUIServer.clear();
    // m_pDataServer.clear();
    m_pDrawingBoardServer.clear();
    // m_pPropertyEditViewEx.clear();
    m_pCodeManagerServer.clear();
}

ComponentInfo PluginComponentView::getGroupComponentInfo(GetDataType type)
{
    ComponentInfo result;
    if (m_pConfigInfo == nullptr) {
        return result;
    }

    result.listGroup = m_pConfigInfo->mapGroups.keys();
    // 获取分组并排序
    if (type == GetDataType_onlyProj && m_category == Controller) {
        result.listGroup.clear();
        result.listGroup << str_ControllerGroupCustomBoard << str_ControllerGroupCustomCode;
    } else if (type == GetDataType_onlyCfg && m_category == Controller) {
        // 只有控制才有本地项目模块
        result.listGroup.removeOne(str_ControllerGroupCustomCode);
        result.listGroup.removeOne(str_ControllerGroupCustomBoard);
    }

    qSort(result.listGroup.begin(), result.listGroup.end(), [this](const QString &lhs, const QString &rhs) -> bool {
        Q_ASSERT(m_pConfigInfo->mapGroups.value(lhs));
        Q_ASSERT(m_pConfigInfo->mapGroups.value(rhs));
        return m_pConfigInfo->mapGroups.value(lhs)->groupOrder < m_pConfigInfo->mapGroups.value(rhs)->groupOrder;
    });

    // 调整列表顺序中组名进行翻译
    QStringList translateList;
    for (QString groupName : result.listGroup) {
        translateList << QObject::tr(groupName.toUtf8());
    }
    result.listGroup.clear();
    result.listGroup = translateList;

    QMapIterator<QString, PGroupClass> iter(m_pConfigInfo->mapGroups);
    while (iter.hasNext()) {
        iter.next();
        PGroupClass pGroup = iter.value();
        if (pGroup == nullptr) {
            continue;
        }
        //  GetDataType_onlyProj只获取本地代码型和本地构造型模块
        if (type == GetDataType_onlyProj && Controller == m_category && m_pToolBoxModel != nullptr
            && (QObject::tr(iter.key().toUtf8()) == ControllerGroupCustomCode
                || QObject::tr(iter.key().toUtf8()) == ControllerGroupCustomBoard)) {
            QStringList listBlocks;
            QList<PModel> datalist;
            if (QObject::tr(iter.key().toUtf8()) == ControllerGroupCustomCode) {
                datalist = m_pProjectServer->GetAllBoardModel(ControlBlock::Type);
            } else if (QObject::tr(iter.key().toUtf8()) == ControllerGroupCustomBoard) {
                datalist = m_pProjectServer->GetAllBoardModel(CombineBoardModel::Type);
            }
            foreach (PModel pBlock, datalist) {
                listBlocks << pBlock->getPrototypeName();
            }
            result.mapComponents.insert(QObject::tr(iter.key().toUtf8()), listBlocks);
            // 获取所有模块
        } else if (type == GetDataType_all) {
            // 分组内元件排序
            QMap<QString, GroupClass::GroupBlockParamater> groupblockmap = pGroup->groupBlocksMap;
            QStringList listBlocks = groupblockmap.keys();
            qSort(listBlocks.begin(), listBlocks.end(), [&](const QString &lhs, const QString &rhs) -> bool {
                return groupblockmap.value(lhs).displayorder < groupblockmap.value(rhs).displayorder;
            });
            result.mapComponents.insert(QObject::tr(iter.key().toUtf8()), listBlocks);
            // 只获取配置文件内的模块(不含构造和代码型)
        } else if (type == GetDataType_onlyCfg) {
            // 只有控制才有项目中的模块
            if (m_category == Controller) {
                if (QObject::tr(iter.key().toUtf8()) == ControllerGroupCustomCode
                    || QObject::tr(iter.key().toUtf8()) == ControllerGroupCustomBoard) {
                    continue;
                }
            }

            QMap<QString, GroupClass::GroupBlockParamater> groupblockmap = pGroup->groupBlocksMap;
            QStringList listBlocks = groupblockmap.keys();
            qSort(listBlocks.begin(), listBlocks.end(), [&](const QString &lhs, const QString &rhs) -> bool {
                return groupblockmap.value(lhs).displayorder < groupblockmap.value(rhs).displayorder;
            });
            result.mapComponents.insert(QObject::tr(iter.key().toUtf8()), listBlocks);
        }
    }

    return result;
}

QPixmap PluginComponentView::getComponentIcon(const QString &prototypename, const int &blocktype, bool bcombinedBlock)
{
    if (m_componentIconMap.contains(prototypename)) {
        return m_componentIconMap[prototypename][blocktype];
    } else {
        return drawTextIcon(prototypename, blocktype, bcombinedBlock);
    }
}

void PluginComponentView::createLocalCodePrototype()
{
    onAddCodeComponent();
}

void PluginComponentView::deleteLocalCodePrototype(const QString &prototype)
{
    onRemoveModule(prototype);
}

void PluginComponentView::editLocalCodePrototype(const QString &prototype)
{
    onModifyControlComponent(prototype);
}

void PluginComponentView::initServer()
{
    // 请求UI服务，模板名称为请求的服务名称，参数见IUIMainLayoutServer.h文件(其他请求服务类似)
    m_pUIServer = RequestServer<ICoreManUILayoutServer>();
    if (!m_pUIServer) {
        LOGOUT(QObject::tr("ICoreManUILayoutServer unregistered"), LOG_ERROR);
    }

    // 模型管理器服务
    m_pModelServer = RequestServer<IModelManagerServer>();
    if (!m_pModelServer) {
        LOGOUT(QObject::tr("IModelManagerServer unregistered"), LOG_ERROR);
    }
    // 项目管理服务
    m_pProjectServer = RequestServer<IProjectManagerServer>();
    if (!m_pProjectServer) {
        LOGOUT(QObject::tr("IProjectManagerServer unregistered"), LOG_ERROR);
    }

    // 控制模块接收模型数据管理器服务发送的消息
    PIServerInterfaceBase pProjectManagerServer = RequestServerInterface<IProjectManagerServer>();
    if (Controller == m_category) {
        if (pProjectManagerServer) {
            pProjectManagerServer->connectNotify(IPM_Notify_CodePrototypeSaved, this,
                                                 SLOT(onRecieveProjectMngMsg(unsigned int, const NotifyStruct &)));
            pProjectManagerServer->connectNotify(IPM_Notify_CodePrototypeDestroyed, this,
                                                 SLOT(onRecieveProjectMngMsg(unsigned int, const NotifyStruct &)));
            pProjectManagerServer->connectNotify(IPM_Notify_DrawingBoardCreate, this,
                                                 SLOT(onRecieveProjectMngMsg(unsigned int, const NotifyStruct &)));
            pProjectManagerServer->connectNotify(IPM_Notify_DrawingBoardDestroyed, this,
                                                 SLOT(onRecieveProjectMngMsg(unsigned int, const NotifyStruct &)));
            pProjectManagerServer->connectNotify(IPM_Notify_DrawingBoardSaved, this,
                                                 SLOT(onRecieveProjectMngMsg(unsigned int, const NotifyStruct &)));
            pProjectManagerServer->connectNotify(IPM_Notify_DrawingBoardFileRenamed, this,
                                                 SLOT(onRecieveProjectMngMsg(unsigned int, const NotifyStruct &)));
            pProjectManagerServer->connectNotify(IPM_Notify_CurrentProjectChanged, this,
                                                 SLOT(onRecieveProjectMngMsg(unsigned int, const NotifyStruct &)));
            pProjectManagerServer->connectNotify(IPM_Notify_CurrentProjectDeactivate, this,
                                                 SLOT(onRecieveProjectMngMsg(unsigned int, const NotifyStruct &)));
            pProjectManagerServer->connectNotify(IPM_Notify_CopyModelFinished, this,
                                                 SLOT(onRecieveProjectMngMsg(unsigned int, const NotifyStruct &)));
        }
    } else if (Device == m_category) {
        pProjectManagerServer->connectNotify(IPM_Notify_CreateElecCombineBoardSave, this,
                                             SLOT(onRecieveProjectMngMsg(unsigned int, const NotifyStruct &)));
        pProjectManagerServer->connectNotify(IPM_Notify_CurrentProjectChanged, this,
                                             SLOT(onRecieveProjectMngMsg(unsigned int, const NotifyStruct &)));
        pProjectManagerServer->connectNotify(IPM_Notify_CopyModelFinished, this,
                                             SLOT(onRecieveProjectMngMsg(unsigned int, const NotifyStruct &)));
        pProjectManagerServer->connectNotify(IPM_Notify_DrawingBoardDestroyed, this,
                                             SLOT(onRecieveProjectMngMsg(unsigned int, const NotifyStruct &)));
        pProjectManagerServer->connectNotify(IPM_Notify_DrawingBoardFileRenamed, this,
                                             SLOT(onRecieveProjectMngMsg(unsigned int, const NotifyStruct &)));
    }
    // 画板服务
    m_pDrawingBoardServer = RequestServer<IPluginGraphicModelingServer>();
    if (!m_pDrawingBoardServer) {
        LOGOUT(QObject::tr("IPluginGraphicModelingServer unregistered"), LOG_ERROR);
    }

    // 代码编辑器、编译服务
    m_pCodeManagerServer = RequestServer<ICodeManagerServer>();
    if (!m_pCodeManagerServer) {
        LOGOUT(QObject::tr("ICodeManagerServer unregistered"), LOG_ERROR);
    }
    m_pPropertyManagerServer = RequestServer<Kcc::PropertyManager::IPropertyManagerServer>();
    if (!m_pPropertyManagerServer) {
        LOGOUT(QObject::tr("IPropertyManagerServer unregistered"), LOG_ERROR);
    }

    // 向导服务
    m_pPluginModuleWizardServer = RequestServer<IPluginModuleWizardServer>();
    if (!m_pPluginModuleWizardServer) {
        LOGOUT(QObject::tr("IPluginModuleWizardServer unregistered"), LOG_ERROR);
    }

    // 接收画板服务发送的页面切换消息
    PIServerInterfaceBase pBoardServer = RequestServerInterface<IPluginGraphicModelingServer>();
    if (pBoardServer) {
        pBoardServer->connectNotify(Notify_DrawingBoardActived, this,
                                    SLOT(onRecieveBoardMsg(unsigned int, const NotifyStruct &)));
    }
}

void PluginComponentView::initComponentData()
{
    if (m_pToolBoxModel == nullptr) {
        if (m_category == Device) {
            // 设备，目前固定只有一组
            m_pToolBoxModel = new DeviceBlockListModel(m_pConfigInfo, this);
            initDeviceGroup();
        } else if (m_category == Measurer) {
            // 测量，目前固定只有一组
            /*m_pToolBoxModel = new MeasurerBlockListModel(m_pConfigInfo,this);
            ToolPage *page = m_pToolBox->addGroup(m_category, MeasurerGroupMeasurer);
            page->setModel(m_pToolBoxModel);*/
        } else if (m_category == Controller) {
            // 控制，多个分组
            m_pToolBoxModel = new ControlBlockListModel(m_pConfigInfo, getControlPrototype(), this); //@txy fix
            initControllerGroup();
        }
        if (m_pToolBoxModel != nullptr) {
            QObject::connect(m_pToolBoxModel, SIGNAL(itemGroupChanged(QMap<int, QVariant>)), this,
                             SLOT(onToolBoxItemGroupChanged(QMap<int, QVariant>)));
        }

    } else {
        // 修改数据，修改配置文件
        if (m_category == Device) {
            auto pModel = dynamic_cast<DeviceBlockListModel *>(m_pToolBoxModel);
            if (pModel != nullptr) {
                pModel->reloadData(m_pConfigInfo);
            }
        } else if (m_category == Measurer) {
            /*auto pModel = dynamic_cast<MeasurerBlockListModel*>(m_pToolBoxModel);
            if (pModel != nullptr) {
                pModel->reloadData(m_pConfigInfo);
            }*/
        } else if (m_category == Controller) {
            auto pModel = dynamic_cast<ControlBlockListModel *>(m_pToolBoxModel);
            if (pModel != nullptr) {
                pModel->reloadCustomListData(getControlPrototype()); //@txy fix
                pModel->reloadData(m_pConfigInfo);
            }
        }
        // m_pToolBox->expandAll();
    }
}

void PluginComponentView::initControllerGroup()
{
    if (m_pConfigInfo == nullptr) {
        LOGOUT(tr("group information none"), LOG_ERROR); // 分组信息无
        return;
    }
    for (int i = 1; i <= m_pConfigInfo->mapGroups.size(); ++i) {
        foreach (QString groupname, m_pConfigInfo->mapGroups.keys()) {
            if (m_pConfigInfo->mapGroups[groupname]->groupOrder == i) {
                ToolPage *page =
                        m_pToolBox->addGroup(m_category, groupname, m_pConfigInfo->mapGroups[groupname]->groupIsExpand);
                connect(page, SIGNAL(isExpandChanged(QString, bool)), this, SLOT(onExpandChangedSave(QString, bool)));
#ifdef COMPILER_SUPPORT_BUILTIN_MODIFY
                groupname = QObject::tr(groupname.toUtf8());
                if (ControllerGroupFunction == groupname) {
                    page->setData(BuildInRole, true);
                    page->setData(ControlClassificationRole, BasicFunction);
                } else if (ControllerGroupDataSource == groupname) {
                    page->setData(BuildInRole, true);
                    page->setData(ControlClassificationRole, Datasource);
                } else if (ControllerGroupComputation == groupname) {
                    page->setData(BuildInRole, true);
                    page->setData(ControlClassificationRole, MathematicalOperations);
                } else if (ControllerGroupEnd == groupname) {
                    page->setData(BuildInRole, true);
                    page->setData(ControlClassificationRole, End);
                } else if (ControllerGroupLogicOperation == groupname) {
                    page->setData(BuildInRole, true);
                    page->setData(ControlClassificationRole, Logicaloperations);
                } else if (ControllerGroupSpecial == groupname) {
                    page->setData(BuildInRole, true);
                    page->setData(ControlClassificationRole, SpecialFunction);
                } else if (ControllerGroupCustomCode == groupname) {
                    page->setData(ControlClassificationRole, CustomCode);
                } else if (ControllerGroupCustomBoard == groupname) {
                    page->setData(ControlClassificationRole, CustomBoard);
                } else if (ControllerGroupDiscrete == groupname) {
                    page->setData(BuildInRole, true);
                    page->setData(ControlClassificationRole, Discrete);
                } else if (ControllerGroupContinue == groupname) {
                    page->setData(BuildInRole, true);
                    page->setData(ControlClassificationRole, Continue);
                }
#endif
                page->setModel(m_pToolBoxModel);
                break;
            }
        }
    }
    initComponentIcon();
}

void PluginComponentView::initDeviceGroup()
{
    // 分组
    // #ifdef QT_DEBUG
    if (m_pConfigInfo == nullptr) {
        LOGOUT(tr("group information none"), LOG_ERROR); // 分组信息无
        return;
    }
    for (int i = 0; i <= m_pConfigInfo->mapGroups.size(); ++i) {
        foreach (QString groupname, m_pConfigInfo->mapGroups.keys()) {
            if (m_pConfigInfo->mapGroups[groupname]->groupOrder == i) {
                ToolPage *page =
                        m_pToolBox->addGroup(m_category, groupname, m_pConfigInfo->mapGroups[groupname]->groupIsExpand);
                connect(page, SIGNAL(isExpandChanged(QString, bool)), this, SLOT(onExpandChangedSave(QString, bool)));
                groupname = QObject::tr(groupname.toUtf8());
                if (DeviceGroupBus == groupname) {
                    page->setData(BuildInRole, true);
                    page->setData(DeviceClassificationRole, DeviceBus);
                } else if (DeviceGroupElement == groupname) {
                    page->setData(BuildInRole, true);
                    page->setData(DeviceClassificationRole, DeviceElement);
                } else if (DeviceGroupMachine == groupname) {
                    page->setData(BuildInRole, true);
                    page->setData(DeviceClassificationRole, DeviceMachine);
                } else if (DeviceGroupTransformer == groupname) {
                    page->setData(BuildInRole, true);
                    page->setData(DeviceClassificationRole, DeviceTransformer);
                } else if (DeviceGroupLine == groupname) {
                    page->setData(BuildInRole, true);
                    page->setData(DeviceClassificationRole, DeviceLine);
                } else if (DeviceGroupLoad == groupname) {
                    page->setData(BuildInRole, true);
                    page->setData(DeviceClassificationRole, DeviceLoad);
                } else if (DeviceGroupPE == groupname) {
                    page->setData(BuildInRole, true);
                    page->setData(DeviceClassificationRole, DeviceDC);
                } else if (DeviceGroupMeasur == groupname) {
                    page->setData(BuildInRole, true);
                    page->setData(DeviceClassificationRole, DeviceMeasur);
                } else if (DeviceGroupNewEnergy == groupname) {
                    page->setData(BuildInRole, true);
                    page->setData(DeviceClassificationRole, DeviceNewEnergy);
                } else if (DeviceGroupLocalCombine == groupname) {
                    page->setData(DeviceClassificationRole, DeviceLocalCombine);
                }
                page->setModel(m_pToolBoxModel);
                break;
            }
        }
    }
    initComponentIcon();
}

void PluginComponentView::initComponentIcon()
{
    if (m_pConfigInfo == nullptr) {
        return;
    }
    m_componentIconMap.clear();
    QMap<int, QPixmap> tmppixmap;
    foreach (PGroupClass pgroup, m_pConfigInfo->mapGroups.values()) {
        if (pgroup != nullptr) {
            foreach (GroupClass::GroupBlockParamater blockparam, pgroup->groupBlocksMap.values()) {
                tmppixmap.clear();
                if (!blockparam.viewIcon.value<QImage>().isNull()) {
                    // 暂时不区分实例和原型
                    tmppixmap.insert(BLOCKTYPE_PROTOTYPE, QPixmap::fromImage(blockparam.viewIcon.value<QImage>()));
                    tmppixmap.insert(BLOCKTYPE_INSTANCE, QPixmap::fromImage(blockparam.viewIcon.value<QImage>()));

                    m_componentIconMap.insert(blockparam.prototypeName, tmppixmap);
                } else {
                    tmppixmap.insert(BLOCKTYPE_PROTOTYPE, drawTextIcon(blockparam.prototypeName, BLOCKTYPE_PROTOTYPE));
                    tmppixmap.insert(BLOCKTYPE_INSTANCE, drawTextIcon(blockparam.prototypeName, BLOCKTYPE_INSTANCE));
                    m_componentIconMap.insert(blockparam.prototypeName, tmppixmap);
                }
            }
        }
    }
}

QPixmap PluginComponentView::drawTextIcon(const QString &textstr, const int &blocktype, bool bcombinedBlock)
{
    QPixmap pixmapText(Global::DRAWING_ITEM_WIDTH, Global::DRAWING_ITEM_HEIGHT);
    pixmapText.fill(QColor("#FFFFFF"));
    QPainter painter(&pixmapText);
    QPen pen(Global::TEXTCOLOR_NORMAL);
    // TOOD咱不区分实例和原型
    //  if (BLOCKTYPE_INSTANCE == blocktype) {
    //      pen.setColor(Global::BLOCKCOLOR_INSTANCE);
    //  } else if (BLOCKTYPE_PROTOTYPE == blocktype) {
    //      pen.setColor(Global::BLOCKCOLOR_PROTOTYPE);
    //  }
    pen.setWidth(1);
    painter.setPen(pen);
    painter.setFont(QFont("Arial", 8));
    QRectF textRect(4, 4, Global::DRAWING_ITEM_WIDTH - 8, Global::DRAWING_ITEM_HEIGHT - 8);
    // QRectF rectnoarmal(1, 1, Global::DRAWING_ITEM_WIDTH - 3, Global::DRAWING_ITEM_HEIGHT - 3);
    QFontMetrics fontWidth(painter.font());
    QString elidnote = fontWidth.elidedText(textstr, Qt::ElideRight, 2 * textRect.width() - 1);
    int textW = painter.fontMetrics().width(textstr);
    int textH = painter.fontMetrics().height();
    painter.drawText(textRect, Qt::TextWrapAnywhere | Qt::AlignCenter, elidnote);
    // painter.drawRect(rectnoarmal);
    QRectF rectout(0, 0, Global::DRAWING_ITEM_WIDTH - 1, Global::DRAWING_ITEM_HEIGHT - 1);
    painter.drawRect(rectout);
    if (bcombinedBlock) {
        QRectF inRect(3, 3, Global::DRAWING_ITEM_WIDTH - 7, Global::DRAWING_ITEM_HEIGHT - 7);
        painter.drawRect(inRect);
    }
    return pixmapText;
}

QList<PModel> PluginComponentView::getControlPrototype()
{
    QList<PModel> controlPrototype;
    // 获取工具箱,代码控制原型
    QList<PModel> toolKitModel = m_pModelServer->GetToolkitModels(KL_TOOLKIT::CONTROL_TOOLKIT);
    controlPrototype.append(toolKitModel);
    // 获取项目代码控制原型和组合构造型原型
    QList<PModel> controlModel = m_pProjectServer->GetAllBoardModel(ControlBlock::Type);
    controlPrototype.append(controlModel);
    QList<PModel> combineModel = m_pProjectServer->GetAllBoardModel(CombineBoardModel::Type);
    controlPrototype.append(combineModel);
    return controlPrototype;
}

bool PluginComponentView::getCodePrototype(const QString &name, PControlBlock prototype)
{
    // 从工具箱获取
    prototype = m_pModelServer->GetToolkitModel(name, KL_TOOLKIT::CONTROL_TOOLKIT).dynamicCast<ControlBlock>();
    if (prototype) {
        return true;
    }
    // 从本地项目获取,本地代码或者构造
    prototype = m_pProjectServer->GetBoardModelByName(name).dynamicCast<ControlBlock>();
    if (prototype) {
        return true;
    }

    return false;
}

bool PluginComponentView::destroyCodePrototype(const QString &name)
{
    // 工具箱原型
    PModel model = m_pModelServer->GetToolkitModel(name, KL_TOOLKIT::CONTROL_TOOLKIT);
    if (model) {
        m_pModelServer->RemoveToolkitModel(name, KL_TOOLKIT::CONTROL_TOOLKIT);
        return true;
    }
    // 本地代码型
    QList<PModel> controlBlockList = m_pProjectServer->GetAllBoardModel(ControlBlock::Type);
    for (PModel block : controlBlockList) {
        if (block->getPrototypeName() == name) {
            m_pProjectServer->DestroyBoardModel(block);
            return true;
        }
    }
    // 本地构造型
    QList<PModel> comBinBlockList = m_pProjectServer->GetAllBoardModel(CombineBoardModel::Type);
    for (PModel block : comBinBlockList) {
        if (block->getPrototypeName() == name) {
            m_pProjectServer->DestroyBoardModel(block);
            return true;
        }
    }
    return false;
}

bool PluginComponentView::modifyCodePrototype(const QString &name, PControlBlock prototype)
{
    // 工具箱原型
    QStringList toolKitNames = m_pModelServer->GetToolkitNames();
    foreach (QString toolKitName, toolKitNames) {
        PModel model = m_pModelServer->GetToolkitModel(name, toolKitName);
        if (model) {
            m_pModelServer->SaveToolkitModel(toolKitName, prototype);
            return true;
        }
    }
    // 本地原型
    PKLProject project = m_pProjectServer->GetCurProject();
    if (project) {
        return project->saveModel(prototype);
    }

    return false;
}

void PluginComponentView::onImportCodeComponent() // 导入模块
{
    ImportBlockProperty *importWidget = new ImportBlockProperty();
    KCustomDialog dlg(importWidget->windowTitle(), importWidget, KBaseDlgBox::Ok | KBaseDlgBox::Cancel,
                      KBaseDlgBox::Ok);
    dlg.resize(550, 130);
    if (KBaseDlgBox::Ok == dlg.exec()) {
        if (importWidget->getBlockName() != NULL && importWidget->getFileName() != NULL) {
            ImportSimulinkBlock *importBlock = new ImportSimulinkBlock(m_pProjectServer, importWidget->getBlockName());
            connect(importBlock, SIGNAL(refreshView()), this, SLOT(onRefreshView()));
            m_pCodeManagerServer->ImportSimulinkMdl(importWidget->getFilePath(), importWidget->getBlockName(),
                                                    importBlock->importCallback, importBlock);
        } else {
            LOGOUT(tr("must select a file and fill in the module name!"), LOG_ERROR); // 必须选择文件，填写模块名称！
        }
    }
}

void PluginComponentView::onAddCodeComponent()
{
    if (!m_pPluginModuleWizardServer) {
        LOGOUT(QObject::tr("IPluginModuleWizardServer unregistered"), LOG_ERROR);
        return;
    }
    if (!m_pProjectServer) {
        LOGOUT(QObject::tr("IProjectManagerServer unregistered"), LOG_ERROR);
        return;
    }
    PKLProject project = m_pProjectServer->GetCurProject();
    if (project == nullptr) {
        LOGOUT(QObject::tr("There are currently no active projects, unable to create a new module！"), LOG_ERROR);
        return;
    }
    // 模块代码编译文件路径
    QString path = project->getProjectDir() + "/ComponentsBin/";
    QDir isExist(path);
    if (!isExist.exists()) {
        isExist.mkdir(path);
    }
    // 调用创建项目代码型模块向导接口
    auto block = m_pPluginModuleWizardServer->NewWizardDialog(path);
    if (block) {
        if (m_pProjectServer) {
            if (project) {
                project->saveModel(block);
            }
        }
    }
    updateView();
}

void PluginComponentView::onRemoveModule(const QString &strName)
{
    removeBlcok(strName);
}

void PluginComponentView::onModifyControlComponent(const QString &strName)
{
    if (!m_pPluginModuleWizardServer) {
        LOGOUT(QObject::tr("IPluginModuleWizardServer unregistered"), LOG_ERROR);
        return;
    }

    if (strName == "Container" || strName == "Slot") {
        return;
    }
    if (!m_pModelServer) {
        LOGOUT(QObject::tr("IModelManagerServer unregistered"), LOG_ERROR);
        return;
    }
    if (!m_pProjectServer) {
        LOGOUT(QObject::tr("IProjectManagerServer unregistered"), LOG_ERROR);
        return;
    }
    PKLProject project = m_pProjectServer->GetCurProject();
    // 获取
    PControlBlock block;
    bool isBuild = true;
    QString projectPath;
    QString modelPath;
    block = qSharedPointerDynamicCast<ControlBlock>(
            m_pModelServer->GetToolkitModel(strName, KL_TOOLKIT::CONTROL_TOOLKIT));

    modelPath = m_pModelServer->GetDllPathByToolkitName(KL_TOOLKIT::CONTROL_TOOLKIT);
    if (!block) {
        if (m_pProjectServer && !project.isNull()) {
            block = qSharedPointerDynamicCast<ControlBlock>(project->getModel(strName));
            projectPath = project->getProjectDir() + "/ComponentsBin/";
            QDir isExist(projectPath);
            if (!isExist.exists()) {
                isExist.mkdir(projectPath);
            }
            isBuild = false;
        }
    }

    bool isChanged;
    // 调用
    if (isBuild) {
        isChanged = m_pPluginModuleWizardServer->ChangeWizardDialog(block, modelPath);
    } else {
        isChanged = m_pPluginModuleWizardServer->ChangeWizardDialog(block, projectPath);
    }

    // 保存
    if (isChanged) {
        if (isBuild) {
            m_pModelServer->SaveToolkitModel(KL_TOOLKIT::CONTROL_TOOLKIT, block);
        } else {
            PKLProject project = m_pProjectServer->GetCurProject();
            project->saveModel(block);
        }
    }
    updateView();
}

void PluginComponentView::onAddBoardComponent()
{
    LOGOUT(tr("please create a new drawing board through the menu")); // 请通过菜单新建画板
}

void PluginComponentView::onModifyBoardComponent(const QString &strName)
{
    if (!m_pDrawingBoardServer) {
        LOGOUT(tr("IUIDrawingBoardServer unregistered"), LOG_ERROR); // IUIDrawingBoardServer unregistered
        return;
    }
    m_pDrawingBoardServer->modifyUserDefineBlock(strName);
}

void PluginComponentView::onSwitchView()
{
    ui.stackedWidget->setCurrentIndex(0 == ui.stackedWidget->currentIndex() ? 1 : 0);
}

void PluginComponentView::onRefreshView()
{
    initComponentData();
    if (m_pToolBox != nullptr) {
        m_pToolBox->expandAll();
    }
}

void PluginComponentView::onAddBuildInComponent(int nClassification)
{
    if (!m_pPluginModuleWizardServer) {
        LOGOUT(QObject::tr("m_pPluginModuleWizardServer unregistered"), LOG_ERROR);
        return;
    }
    // 模块代码编译文件路径
    QString path = m_pModelServer->GetDllPathByToolkitName(KL_TOOLKIT::CONTROL_TOOLKIT);
    // 调用创建模块向导接口
    auto block = m_pPluginModuleWizardServer->NewWizardDialog(path);
    if (block) {
        saveBuiltInModel(block, nClassification);
    }
    updateView();
}

void PluginComponentView::onExpandAll()
{
    m_pToolBox->expandAll();
}

void PluginComponentView::onCollapseAll()
{
    m_pToolBox->collapseAll();
}

void PluginComponentView::onAdjustListView()
{
    ComponentInfo info = getGroupComponentInfo();
    AdjustListViewWidget *listwidget = new AdjustListViewWidget(info.listGroup, info.mapComponents);
    KCustomDialog dlg(tr("component list adjustment"), listwidget, KBaseDlgBox::Ok | KBaseDlgBox::Cancel,
                      KBaseDlgBox::Ok); // 元件列表调整
    dlg.resize(listwidget->width(), listwidget->height());
    if (KBaseDlgBox::Ok == dlg.exec()) {
        // 刷新列表
        QStringList listGroup = listwidget->getGroupList();
        QMap<QString, QStringList> listItemMap = listwidget->getGroupItemMap();
        if (m_pConfigInfo == nullptr) {
            return;
        }
        QList<QString> groupnamelists = m_pConfigInfo->mapGroups.keys();
        // 调整租的顺序
        int pGroupOrder = 0;
        for (QString groupname : groupnamelists) {
            if (listGroup.contains(QObject::tr(groupname.toUtf8()))) {
                m_pConfigInfo->mapGroups[groupname]->groupOrder =
                        listGroup.indexOf(QObject::tr(groupname.toUtf8())) + 1;
            }
        }
        // 删除的组
        foreach (QString groupname, groupnamelists) {
            if (!listGroup.contains(QObject::tr(groupname.toUtf8()))) {
                m_pConfigInfo->mapGroups.remove(groupname);
            }
        }
        // 调整模块顺序
        foreach (QString groupname, m_pConfigInfo->mapGroups.keys()) {
            if (listGroup.contains(QObject::tr(groupname.toUtf8()))) {
                for (int blockorder = 0; blockorder < listItemMap[QObject::tr(groupname.toUtf8())].size();
                     ++blockorder) {
                    if (m_pConfigInfo->mapGroups[groupname]->groupBlocksMap.contains(
                                listItemMap[QObject::tr(groupname.toUtf8())][blockorder])) {
                        m_pConfigInfo->mapGroups[groupname]
                                ->groupBlocksMap[listItemMap[QObject::tr(groupname.toUtf8())][blockorder]]
                                .displayorder = blockorder + 1;
                    }
                }
            }
        }
        // 删除模块
        foreach (QString groupname, m_pConfigInfo->mapGroups.keys()) {
            if (listItemMap.contains(QObject::tr(groupname.toUtf8()))) {
                QList<QString> blocklist = m_pConfigInfo->mapGroups[groupname]->groupBlocksMap.keys();
                foreach (QString blockname, blocklist) {
                    if (!listItemMap[QObject::tr(groupname.toUtf8())].contains(blockname)) {
                        removeBlcok(blockname);
                    }
                }
            }
        }
        // 保存数据
        if (m_pCfgInfoManager != nullptr && m_pConfigInfo != nullptr) {
            m_pCfgInfoManager->saveFile(m_pConfigInfo);
        }
        // 刷新视图
        initComponentData();
        if (m_pToolBox != nullptr) {
            // 每次更改完顺序之后将指针置空
            m_pToolBox->setSelectedPtr();
            m_pToolBox->removeAllGroup();
        }
        if (Global::Device == m_category) {
            initControllerGroup();
        } else if (Global::Controller == m_category) {
            initDeviceGroup();
        }
    }
    if (listwidget != nullptr) {
        delete listwidget;
        listwidget = nullptr;
    }
}

void PluginComponentView::onAddComponentToBoard(const QString &strPrototypeName)
{
    if (!strPrototypeName.isEmpty() && m_pComponentServer != nullptr) {
        NotifyStruct notify;
        notify.code = Notify_AddComponentToBoard;
        notify.paramMap[PROTOTYPENAME] = strPrototypeName;
        m_pComponentServer->emitNotify(notify);
    }
}
void PluginComponentView::onCreateComponentInstance(const QString &strPrototypeName)
{ // 目前只有本地代码型和构造型有创建实例功能
    if (!strPrototypeName.isEmpty() && m_pComponentServer != nullptr && m_pPropertyManagerServer != nullptr) {
        m_pPropertyManagerServer->CreateBlockInstance(strPrototypeName);
    }
}

void PluginComponentView::onRecieveProjectMngMsg(unsigned int code, const NotifyStruct &param)
{
    if (code == IPM_Notify_DrawingBoardCreate || IPM_Notify_DrawingBoardDestroyed || IPM_Notify_DrawingBoardSaved
        || IPM_Notify_DrawingBoardFileRenamed || code == IPM_Notify_CurrentProjectChanged
        || IPM_Notify_CodePrototypeSaved == code || IPM_Notify_CodePrototypeDestroyed == code
        || IPM_Notify_CurrentProjectDeactivate || IPM_Notify_CopyModelFinished
        || IPM_Notify_CreateElecCombineBoardSave) {
        updateView();
    }
}

void PluginComponentView::onTreeItemMoved(QStandardItem *item)
{
    // 响应拖拽元器件事件并同步到数据管理器
    int nCategory = item->data(CategoryRole).toInt();
    if (nCategory == Controller) {
        int nControllerType = item->data(ControllerTypeRole).toInt();
        QString strName = item->data(Global::NameRole).toString();
        if (nControllerType == Code) {
            PControlBlock block;
            if (!getCodePrototype(strName, block)) { //@TXY
                return;
            }
            // block->setIsBuildIn(item->data(BuildInRole).toBool());
            // //暂时不修改canDelete
            if (item->data(ControlClassificationRole).toInt() > Other) {
                // 目前ControlBlock中没有本地代码型和本地组合型的Classification分类，归入Other
                // block->setClassification(ControlBlock::Other);
            } else {
                // block->setClassification(
                //         static_cast<ControlBlock::Classification>(item->data(ControlClassificationRole).toInt()));
            }
            modifyCodePrototype(strName, block); //@TXY
        }
    }
}

void PluginComponentView::onToolBoxItemGroupChanged(const QMap<int, QVariant> &mapGroupRoleTemplate)
{
    QList<int> listRoles;
    listRoles << Global::NameRole << Global::ControllerTypeRole << Global::BuildInRole << Global::CanDeleteRole
              << Global::ControlClassificationRole;
    foreach (int role, listRoles) {
        if (!mapGroupRoleTemplate.contains(role)) {
            return;
        }
    }

    // 响应拖拽元器件事件并同步到数据管理器
    if (m_category == Controller) {
        QString strName = mapGroupRoleTemplate[Global::NameRole].toString();
        if (mapGroupRoleTemplate[ControllerTypeRole].toInt() == Code) {
            PControlBlock block;
            if (!getCodePrototype(strName, block)) {
                return;
            }
            // block->setIsBuildIn(mapGroupRoleTemplate[BuildInRole].toBool());
            if (mapGroupRoleTemplate[ControlClassificationRole].toInt() > Other) {
                // 目前ControlBlock中没有本地代码型和本地组合型的Classification分类，归入Other
                // block->setClassification(ControlBlock::Other);
            } else {
                // block->setClassification(static_cast<ControlBlock::Classification>(
                // mapGroupRoleTemplate[ControlClassificationRole].toInt()));
            }
            modifyCodePrototype(strName, block); //@txy
        }
    }
}

void PluginComponentView::contextMenuEvent(QContextMenuEvent *e)
{
    auto menu = m_pContextMenuManager->createMenu();
    // menu->setSwitchViewEnable();
    menu->addSeparator();
    menu->setRefreshViewEnable();
    menu->addSeparator();
    menu->setExpandAllEnable();
    menu->setCollapseAllEnable();
#ifdef COMPILER_SUPPORT_BUILTIN_MODIFY
    menu->setAdjustListViewEnable();
#endif
    menu->setObjectName("uniformStyleMenu_cV");
    // menu->exec(mapToGlobal(e->pos()));
}

void PluginComponentView::onAddElcBuildInComponent(int nClassification)
{
    if (!m_pPluginModuleWizardServer) {
        LOGOUT(QObject::tr("m_pPluginModuleWizardServer unregistered"), LOG_ERROR);
        return;
    }
    if (!m_pModelServer) {
        LOGOUT(QObject::tr("IModelManagerServer unregistered"), LOG_ERROR);
    }
    // 调用创建模块向导接口
    auto block = m_pPluginModuleWizardServer->NewElecWizardDialog();
    if (block) {
        saveBuiltInModel(block, nClassification);
    }
    updateView();
}

void PluginComponentView::onChangeElcBuildInComponent(const QString &strName)
{
    if (!m_pPluginModuleWizardServer) {
        LOGOUT(QObject::tr("IPluginModuleWizardServer unregistered"), LOG_ERROR);
        return;
    }
    if (!m_pModelServer) {
        LOGOUT(QObject::tr("IModelManagerServer unregistered"), LOG_ERROR);
    }
    auto block = qSharedPointerDynamicCast<ElectricalBlock>(
            m_pModelServer->GetToolkitModel(strName, KL_TOOLKIT::ELECTRICAL_TOOLKIT));
    // 调用修改模块向导接口
    bool isChanged = m_pPluginModuleWizardServer->ChangeElecWizardDialog(block);
    if (isChanged) {
        m_pModelServer->SaveToolkitModel(KL_TOOLKIT::ELECTRICAL_TOOLKIT, block);
    }
    initComponentData();
}

void PluginComponentView::onRecieveBoardMsg(unsigned int code, const NotifyStruct &param)
{
    if (m_pUIServer == nullptr || m_pUIServer->GetMainUI() == nullptr) {
        return;
    }
    if (code == Notify_DrawingBoardActived) {
        int actboardtype = param.paramMap["BoardType"].toInt();
        if (actboardtype == ElecBoardModel::Type) {
            m_pUIServer->GetMainUI()->SwitchPanel(tr("Electrical"));
        } else if (actboardtype == ControlBoardModel::Type) {
            m_pUIServer->GetMainUI()->SwitchPanel(tr("Control"));
        } else if (actboardtype == CombineBoardModel::Type) {
            m_pUIServer->GetMainUI()->SwitchPanel(tr("Control"));
        } else if (actboardtype == ComplexBoardModel::Type) {
            m_pUIServer->GetMainUI()->SwitchPanel(tr("Control"));
        } else if (actboardtype == ElecCombineBoardModel::Type) {
            m_pUIServer->GetMainUI()->SwitchPanel(tr("Electrical"));
        } else {
        }
    }
}

void PluginComponentView::onExpandChangedSave(const QString &groupName, const bool &isExpand)
{
    // 保存数据
    if (m_pCfgInfoManager != nullptr && m_pConfigInfo != nullptr) {
        m_pConfigInfo->mapGroups[groupName]->groupIsExpand = isExpand;
        m_pCfgInfoManager->saveFile(m_pConfigInfo);
    }
}

void PluginComponentView::updateView()
{
    m_pCfgInfoManager->initGroup(m_category);
    m_pConfigInfo = m_pCfgInfoManager->getConfigInfo(m_category);
    initComponentData();
}

void PluginComponentView::removeBlcok(const QString &strName)
{
    if (!m_pProjectServer) {
        LOGOUT(QObject::tr("IProjectManagerServer unregistered"), LOG_ERROR);
        return;
    }
    QString strMessage = tr("Are you sure you want to delete the module[%1]?unable to restore after deletion!")
                                 .arg(strName); // 确定要删除模块[%1]吗？删除后无法恢复！
    if (KMessageBox::Ok == KMessageBox::warning(strMessage, KMessageBox::Ok | KMessageBox::Cancel, KMessageBox::Ok)) {
        if (m_category == Controller) {
            // 删除模块库文件
            if (destroyCodePrototype(strName)) {                             //@txy fix
                LOGOUT(tr("[%1] module deleted!").arg(strName), LOG_NORMAL); // 【%1】 模块删除！
                // 删除库文件
                QString strBinDir = QCoreApplication::applicationDirPath() + "/plugins/ComponentsBin";
                QString strFileName = QString("%1/%2.dll").arg(strBinDir).arg(strName);
                if (QFile::exists(strFileName)) {
                    QFile::remove(strFileName);
                }
                strFileName = QString("%1/%2.pdb").arg(strBinDir).arg(strName);
                if (QFile::exists(strFileName)) {
                    QFile::remove(strFileName);
                }
            }
        } else if (m_category == Device) {
            QString toolKitName;
            if (m_pModelServer->ContainToolkitModel(strName, toolKitName)) {
                m_pModelServer->RemoveToolkitModel(toolKitName, strName);
            } else {
                auto findModel = m_pProjectServer->GetBoardModelByName(strName);
                if (findModel) {
                    m_pProjectServer->DestroyBoardModel(findModel);
                }
            }
        }
        // 配置文件删除模块
        for (QString groupName : m_pConfigInfo->mapGroups.keys()) {
            QList<QString> blockList = m_pConfigInfo->mapGroups[groupName]->groupBlocksMap.keys();
            for (QString blockName : blockList) {
                if (blockName == strName) {
                    m_pConfigInfo->mapGroups[groupName]->groupBlocksMap.remove(blockName);
                }
            }
        }
        // 保存数据
        if (m_pCfgInfoManager != nullptr && m_pConfigInfo != nullptr) {
            m_pCfgInfoManager->saveFile(m_pConfigInfo);
        }
        initComponentData();
    }
}

QString PluginComponentView::getGroupName(const int &nClassification)
{
    if (m_category == Controller) {
        switch (nClassification) {
        case BasicFunction:
            return str_ControllerGroupFunction;
            break;
        case Datasource:
            return str_ControllerGroupDataSource;
            break;
        case End:
            return str_ControllerGroupEnd;
            break;
        case MathematicalOperations:
            return str_ControllerGroupComputation;
            break;
        case Logicaloperations:
            return str_ControllerGroupLogicOperation;
            break;
        case SpecialFunction:
            return str_ControllerGroupSpecial;
            break;
        case CustomCode:
            return str_ControllerGroupCustomCode;
            break;
        case CustomBoard:
            return str_ControllerGroupCustomBoard;
            break;
        case Discrete:
            return str_ControllerGroupDiscrete;
            break;
        case Continue:
            return str_ControllerGroupContinue;
            break;
        default:
            break;
        }
    } else if (m_category == Device) {
        switch (nClassification) {
        case DeviceBus:
            return str_DeviceGroupBus;
            break;
        case DeviceElement:
            return str_DeviceGroupElement;
            break;
        case DeviceMachine:
            return str_DeviceGroupMachine;
            break;
        case DeviceTransformer:
            return str_DeviceGroupTransformer;
            break;
        case DeviceLine:
            return str_DeviceGroupLine;
            break;
        case DeviceLoad:
            return str_DeviceGroupLoad;
            break;
        case DeviceDC:
            return str_DeviceGroupPE;
            break;
        case DeviceMeasur:
            return str_DeviceGroupMeasur;
            break;
        case DeviceNewEnergy:
            return str_DeviceGroupNewEnergy;
        default:
            break;
        }
    }
    return QString("Unknow");
}

void PluginComponentView::saveBuiltInModel(const PModel &block, const int &nClassification)
{
    if (m_category == Controller) {
        m_pModelServer->AddToolkitModel(KL_TOOLKIT::CONTROL_TOOLKIT, block);
    } else if (m_category == Device) {
        m_pModelServer->AddToolkitModel(KL_TOOLKIT::ELECTRICAL_TOOLKIT, block);
    }
    // 写入配置文件
    if (m_pConfigInfo == nullptr) {
        LOGOUT(QObject::tr("Unable to write configuration file, module creation failed!"));
        return;
    }

    if (getGroupName(nClassification).isEmpty()) {
        LOGOUT("配置文件错误，创建模块失败！");
        return;
    }

    int porder = 0;
    Global::GroupClass::GroupBlockParamater param;
    param.prototypeName = block->getPrototypeName();
    param.viewName = block->getPrototypeName_CHS();
    param.viewIcon = block->getResource().value((Kcc::BlockDefinition::PNG_NORMAL_PIC));
    auto group = m_pConfigInfo->mapGroups.value(getGroupName(nClassification));
    if (!group) {
        LOGOUT(QObject::tr("Unable to write configuration file, module creation failed!"));
        return;
    }
    porder = group->groupBlocksMap.size();
    param.displayorder = ++porder;
    // 默认模块显示 若不显示在配置文件添加display字段为false
    param.display = true;
    m_pConfigInfo->mapGroups[getGroupName(nClassification)]->groupBlocksMap[block->getPrototypeName()] = param;
    m_pCfgInfoManager->saveFile(m_pConfigInfo);
}

void PluginComponentView::onShowHelp(const QString &strPrototypeName)
{
    if (!strPrototypeName.isEmpty()) {
        DocHelper::openObjectDocAssistant(strPrototypeName);
    }
}