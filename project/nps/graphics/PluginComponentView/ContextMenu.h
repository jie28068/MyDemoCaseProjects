#ifndef CONTEXTMENU_H
#define CONTEXTMENU_H

#include <QAction>
#include <QMenu>
#include <QObject>
#include <QSharedPointer>

class ContextMenuManagerPrivate;

class ContextMenu;
typedef QSharedPointer<ContextMenu> PContextMenu;

class CAction : public QAction
{
    Q_OBJECT
public:
    CAction(QObject *parent = nullptr);
    CAction(const QString &text, QObject *parent = nullptr);
    CAction(const QIcon &icon, const QString &text, QObject *parent = nullptr);

    void setCustomData(const QVariantMap &propertymap);
    QVariantMap getCustomData();

private:
    QVariantMap m_propertymap;
};
class ContextMenuManager : public QObject
{
    Q_OBJECT
private:
    ContextMenuManager();

public:
    ~ContextMenuManager();

    static ContextMenuManager *instance(const QString &strModuleName);

    PContextMenu createMenu();

signals:
    // 代码型模块
    void addCodeComponent();
    void importCodeComponent();
    void removeCodeComponent(const QString &strName);
    void ModifyControlComponent(const QString &strName);

    // 组合型模块
    void addBoardComponent();
    void removeBoardComponent(const QString &strName);
    void modifyBoardComponent(const QString &strName);

    // 通用
    void showProperty(const QString &strTypeName);
    // void switchView();
    void refreshView();
    void addBuildInComponent(int nClassification);
    void expandAll();
    void collapseAll();
    void adjustListView();
    void addComponentToBoard(const QString &strPrototypeName);
    void createComponentInstance(const QString &strPrototypeName);
    void showHelp(const QString &strPrototypeName);

    // 新增电气向导
    void addElcBuildInComponent(int nClassification);
    /// @brief 修改电气模块
    void changeElcBuildInComponent(const QString &strName);
    /// @brief 删除电气模块
    /// @param strName
    void removeElcBuildInComponent(const QString &strName);

private slots:
    void onActionTriggered(bool bChecked);

private:
    ContextMenuManagerPrivate *d;
};

class ContextMenu : public QMenu
{
    Q_OBJECT
private:
    explicit ContextMenu(ContextMenuManagerPrivate *manager, QWidget *parent = nullptr);
    friend class ContextMenuManager;

public:
    ~ContextMenu();

    // 提供flag给支持ContextType的Model的元素使用，用于在元素上显示不同的交互菜单
    enum ContextType {
        // 通用
        Ignore = 0,
        ShowProperty = 0x0001,
        SwitchView = 0x0002,
        AddComponentToBoard = 0x0004,
        CreateComponentInstance = 0x0008,
        // 代码型
        AddCodeComponent = 0x0010,
        RemoveCodeComponent = 0x0020,
        ModifyControlComponent = 0x0040,
        // 组合型
        AddBoardComponent = 0x0100,
        RemoveBoardComponent = 0x0200,
        ModifyBoardComponent = 0x0400,
        // 电气编辑
        elecModifyCodeComponent = 0x1000,
        // 电气删除
        elecRemoveCodeComponent = 0x2000,
        //帮助
        Help = 0x4000,
    };

    void setShowPropertyEnable(const QString &strName);
    // void setSwitchViewEnable();
    void setRefreshViewEnable();
    void setAddBuildInEnable(int nClassification);
    void setExpandAllEnable();
    void setCollapseAllEnable();

    void setAdjustListViewEnable();

    void setAddCodeEnable();
    void setImportCodeEnable();
    void setRemoveCodeEnable(const QString &strName);
    void setModifyCodeEnable(const QString &strName);

    void setAddBoardEnable();
    void setRemoveBoardEnable(const QString &strName);
    void setModifyBoardEnable(const QString &strName);

    void setAddComponentToBoardEnable(const QString &strPrototypeName);
    void setCreateComponentInstanceEnable(const QString &strPrototypeName);

    /// @brief  新增电气向导
    void setAddElcBuildInEnable(int nClassification);
    /// @brief 修改电气向导
    void setChangeElcBuildInEnable(const QString &strPrototypeName);
    /// @brief 删除电气模块
    /// @param strPrototypeName
    void removeElcBuildInEnable(const QString &strPrototypeName);

    void showHelpEnable(const QString &strPrototypeName);

private:
    ContextMenuManagerPrivate *d;
};

#endif // CONTEXTMENU_H
