#include "ContextMenu.h"
#include "GlobalDefinition.h"
#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

CAction::CAction(QObject *parent) : QAction(parent) { }
CAction::CAction(const QString &text, QObject *parent) : QAction(text, parent) { }
CAction::CAction(const QIcon &icon, const QString &text, QObject *parent) : QAction(icon, text, parent) { }
void CAction::setCustomData(const QVariantMap &propertymap)
{
    m_propertymap = propertymap;
}

QVariantMap CAction::getCustomData()
{
    return m_propertymap;
}

class ContextMenuManagerPrivate
{
public:
    ContextMenuManagerPrivate()
    {
        pActionAddCodeComponent = new CAction(QObject::tr("New"), nullptr);
        pActionImportCodeComponent = new CAction(QObject::tr("Import"), nullptr);
        pActionRemoveCodeComponent = new CAction(QObject::tr("Delete"), nullptr);
        pActionModifyControlComponent = new CAction(QObject::tr("Edit"), nullptr);

        pActionAddBoardComponent = new CAction(QObject::tr("New"), nullptr);
        pActionRemoveBoardComponent = new CAction(QObject::tr("Delete"), nullptr);
        pActionModifyBoardComponent = new CAction(QObject::tr("Edit"), nullptr);

        pActionShowProperty = new CAction(QObject::tr("Property"), nullptr);
        pActionRefreshView = new CAction(QObject::tr("Refresh View"), nullptr);
        pActionAddBuildIn = new CAction(QObject::tr("New"), nullptr);
        pActionExpandAll = new CAction(QObject::tr("Expand All"), nullptr);
        pActionCollapseAll = new CAction(QObject::tr("Collapse All"), nullptr);

        pActionAdjustListView = new CAction(QObject::tr("Adjust List Order"), nullptr);

        pActionAddComponentToBoard = new CAction(QObject::tr("Add To Board"), nullptr);
        pActionCreateComponentInstance = new CAction(QObject::tr("Create Instance"), nullptr);

        pActionAddElcBuildIn = new CAction(QObject::tr("New"), nullptr);
        pActionChangeElcBuildIn = new CAction(QObject::tr("Edit"), nullptr);
        pActionRemoveElcBuildIn = new CAction(QObject::tr("Delete"), nullptr);

        pActionHelp = new CAction(QObject::tr("Help"),nullptr);
    }

public:
    CAction *pActionAddCodeComponent;
    CAction *pActionImportCodeComponent;
    CAction *pActionRemoveCodeComponent;
    CAction *pActionModifyControlComponent;

    CAction *pActionAddBoardComponent;
    CAction *pActionRemoveBoardComponent;
    CAction *pActionModifyBoardComponent;

    CAction *pActionShowProperty;
    // CAction *pActionSwitchView;
    CAction *pActionRefreshView;
    CAction *pActionAddBuildIn;
    CAction *pActionExpandAll;
    CAction *pActionCollapseAll;

    CAction *pActionAdjustListView;

    CAction *pActionAddComponentToBoard;
    CAction *pActionCreateComponentInstance;

    CAction *pActionAddElcBuildIn;    // 新建电气模块
    CAction *pActionChangeElcBuildIn; // 修改电气模块
    CAction *pActionRemoveElcBuildIn; // 删除电气模块

    CAction *pActionHelp;//模块帮助

    QString m_strModuleName;
    static QMap<QString, QSharedPointer<ContextMenuManager>> ms_mapModuleManager;
};

QMap<QString, QSharedPointer<ContextMenuManager>> ContextMenuManagerPrivate::ms_mapModuleManager;

ContextMenuManager::ContextMenuManager() : d(new ContextMenuManagerPrivate)
{
    connect(d->pActionAddCodeComponent, SIGNAL(triggered(bool)), this, SLOT(onActionTriggered(bool)));
    connect(d->pActionImportCodeComponent, SIGNAL(triggered(bool)), this, SLOT(onActionTriggered(bool)));
    connect(d->pActionRemoveCodeComponent, SIGNAL(triggered(bool)), this, SLOT(onActionTriggered(bool)));
    connect(d->pActionModifyControlComponent, SIGNAL(triggered(bool)), this, SLOT(onActionTriggered(bool)));

    connect(d->pActionAddBoardComponent, SIGNAL(triggered(bool)), this, SLOT(onActionTriggered(bool)));
    connect(d->pActionRemoveBoardComponent, SIGNAL(triggered(bool)), this, SLOT(onActionTriggered(bool)));
    connect(d->pActionModifyBoardComponent, SIGNAL(triggered(bool)), this, SLOT(onActionTriggered(bool)));

    connect(d->pActionShowProperty, SIGNAL(triggered(bool)), this, SLOT(onActionTriggered(bool)));
    // connect(d->pActionSwitchView, SIGNAL(triggered(bool)), this, SLOT(onActionTriggered(bool)));
    connect(d->pActionRefreshView, SIGNAL(triggered(bool)), this, SLOT(onActionTriggered(bool)));
    connect(d->pActionAddBuildIn, SIGNAL(triggered(bool)), this, SLOT(onActionTriggered(bool)));
    connect(d->pActionExpandAll, SIGNAL(triggered(bool)), this, SLOT(onActionTriggered(bool)));
    connect(d->pActionCollapseAll, SIGNAL(triggered(bool)), this, SLOT(onActionTriggered(bool)));

    connect(d->pActionAdjustListView, SIGNAL(triggered(bool)), this, SLOT(onActionTriggered(bool)));

    connect(d->pActionAddComponentToBoard, SIGNAL(triggered(bool)), this, SLOT(onActionTriggered(bool)));
    connect(d->pActionCreateComponentInstance, SIGNAL(triggered(bool)), this, SLOT(onActionTriggered(bool)));

    connect(d->pActionAddElcBuildIn, SIGNAL(triggered(bool)), this, SLOT(onActionTriggered(bool)));
    connect(d->pActionChangeElcBuildIn, SIGNAL(triggered(bool)), this, SLOT(onActionTriggered(bool)));
    connect(d->pActionRemoveElcBuildIn, SIGNAL(triggered(bool)), this, SLOT(onActionTriggered(bool)));

    connect(d->pActionHelp,SIGNAL(triggered(bool)),this,SLOT(onActionTriggered(bool)));
}

ContextMenuManager::~ContextMenuManager()
{
    delete d;
}

ContextMenuManager *ContextMenuManager::instance(const QString &strModuleName)
{
    if (!ContextMenuManagerPrivate::ms_mapModuleManager.contains(strModuleName)) {
        ContextMenuManagerPrivate::ms_mapModuleManager.insert(
                strModuleName, QSharedPointer<ContextMenuManager>(new ContextMenuManager));
    }
    return ContextMenuManagerPrivate::ms_mapModuleManager[strModuleName].data();
}

PContextMenu ContextMenuManager::createMenu()
{
    return QSharedPointer<ContextMenu>(new ContextMenu(d));
}

void ContextMenuManager::onActionTriggered(bool bChecked)
{
    QObject *pSender = sender();
    // 代码型
    if (pSender == d->pActionAddCodeComponent) {
        emit addCodeComponent();
    } else if (pSender == d->pActionImportCodeComponent) {
        emit importCodeComponent();
    } else if (pSender == d->pActionRemoveCodeComponent) {
        emit removeCodeComponent(d->pActionRemoveCodeComponent->data().toString());
    } else if (pSender == d->pActionModifyControlComponent) {
        emit ModifyControlComponent(d->pActionModifyControlComponent->data().toString());

        // 组合型
    } else if (pSender == d->pActionAddBoardComponent) {
        emit addBoardComponent();
    } else if (pSender == d->pActionRemoveBoardComponent) {
        emit removeBoardComponent(d->pActionRemoveBoardComponent->data().toString());
    } else if (pSender == d->pActionModifyBoardComponent) {
        emit modifyBoardComponent(d->pActionModifyBoardComponent->data().toString());

        // 通用
    } else if (pSender == d->pActionShowProperty) {
        emit showProperty(d->pActionShowProperty->data().toString());
        //} else if (pSender == d->pActionSwitchView) {
        //    emit switchView();
    } else if (pSender == d->pActionRefreshView) {
        emit refreshView();
    } else if (pSender == d->pActionAddBuildIn) {
        emit addBuildInComponent(d->pActionAddBuildIn->data().toInt());
    } else if (pSender == d->pActionExpandAll) {
        emit expandAll();
    } else if (pSender == d->pActionCollapseAll) {
        emit collapseAll();
    } else if (pSender == d->pActionAdjustListView) {
        emit adjustListView();
    } else if (pSender == d->pActionAddComponentToBoard) {
        emit addComponentToBoard(d->pActionAddComponentToBoard->data().toString());
    } else if (pSender == d->pActionCreateComponentInstance) {
        emit createComponentInstance(d->pActionCreateComponentInstance->data().toString());
    } else if (pSender == d->pActionAddElcBuildIn) {
        emit addElcBuildInComponent(d->pActionAddElcBuildIn->data().toInt());
    } else if (pSender == d->pActionChangeElcBuildIn) {
        emit changeElcBuildInComponent(d->pActionChangeElcBuildIn->data().toString());
    } else if (pSender == d->pActionRemoveElcBuildIn) {
        emit removeElcBuildInComponent(d->pActionRemoveElcBuildIn->data().toString());
    }else if(pSender == d->pActionHelp){
        emit showHelp(d->pActionHelp->data().toString());
    }
}

//////////////////////////////////////////////////////////////////////////

ContextMenu::ContextMenu(ContextMenuManagerPrivate *manager, QWidget *parent) : QMenu(parent), d(manager) { }

ContextMenu::~ContextMenu() { }

void ContextMenu::setAddCodeEnable()
{
    addAction(d->pActionAddCodeComponent);
}

void ContextMenu::setImportCodeEnable()
{
    addAction(d->pActionImportCodeComponent);
}
void ContextMenu::setRemoveCodeEnable(const QString &strName)
{
    d->pActionRemoveCodeComponent->setData(strName);
    addAction(d->pActionRemoveCodeComponent);
}

void ContextMenu::setModifyCodeEnable(const QString &strName)
{
    d->pActionModifyControlComponent->setData(strName);
    addAction(d->pActionModifyControlComponent);
}

void ContextMenu::setAddBoardEnable()
{
    addAction(d->pActionAddBoardComponent);
}

void ContextMenu::setRemoveBoardEnable(const QString &strName)
{
    d->pActionRemoveBoardComponent->setData(strName);
    addAction(d->pActionRemoveBoardComponent);
}

void ContextMenu::setModifyBoardEnable(const QString &strName)
{
    d->pActionModifyBoardComponent->setData(strName);
    addAction(d->pActionModifyBoardComponent);
}

void ContextMenu::setAddComponentToBoardEnable(const QString &strPrototypeName)
{
    d->pActionAddComponentToBoard->setData(strPrototypeName);
    addAction(d->pActionAddComponentToBoard);
}
void ContextMenu::setCreateComponentInstanceEnable(const QString &strPrototypeName)
{
    d->pActionCreateComponentInstance->setData(strPrototypeName);
    addAction(d->pActionCreateComponentInstance);
}

void ContextMenu::setShowPropertyEnable(const QString &strName)
{
    d->pActionShowProperty->setData(strName);
    addAction(d->pActionShowProperty);
}

// void ContextMenu::setSwitchViewEnable()
//{
//     addAction(d->pActionSwitchView);
// }

void ContextMenu::setRefreshViewEnable()
{
    addAction(d->pActionRefreshView);
}

void ContextMenu::setAddBuildInEnable(int nClassification)
{
    d->pActionAddBuildIn->setData(nClassification);
    addAction(d->pActionAddBuildIn);
}

void ContextMenu::setExpandAllEnable()
{
    addAction(d->pActionExpandAll);
}

void ContextMenu::setCollapseAllEnable()
{
    addAction(d->pActionCollapseAll);
}

void ContextMenu::setAdjustListViewEnable()
{
    addAction(d->pActionAdjustListView);
}

void ContextMenu::setAddElcBuildInEnable(int nClassification)
{
    d->pActionAddElcBuildIn->setData(nClassification);
    addAction(d->pActionAddElcBuildIn);
}

void ContextMenu::setChangeElcBuildInEnable(const QString &strPrototypeName)
{
    d->pActionChangeElcBuildIn->setData(strPrototypeName);
    addAction(d->pActionChangeElcBuildIn);
}

void ContextMenu::removeElcBuildInEnable(const QString &strPrototypeName)
{
    d->pActionRemoveElcBuildIn->setData(strPrototypeName);
    addAction(d->pActionRemoveElcBuildIn);
}

void ContextMenu::showHelpEnable(const QString &strPrototypeName)
{
    d->pActionHelp->setData(strPrototypeName);
    addAction(d->pActionHelp);
}