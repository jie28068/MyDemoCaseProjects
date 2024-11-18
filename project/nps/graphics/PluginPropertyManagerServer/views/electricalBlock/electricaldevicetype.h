#ifndef ELECTRICALDEVICETYPE_H
#define ELECTRICALDEVICETYPE_H

#include <QComboBox>
#include <QSplitter>
#include <QStandardItemModel>
#include <QTableView>
#include <QTextEdit>

#include "CoreLib/ServerBase.h"
#include "ElideLineText.h"
#include "KLWidgets/KCustomDialog.h"
#include "KLineEdit.h"
#include "TypeItemView.h"
#include "ui_electricaldevicetype.h"

#include "TypeItemView.h"
#include "customtablewidget.h"
#include "devicetypemodel.h"

class BlockObj;
class EditDeviceType;
class DeviceTypeListWidget;
class SelectNodeWidget;
namespace Kcc {
namespace BlockDefinition {
class Model;
class ElectricalBlock;
class DeviceModel;
}
}

static const int DLGLIST_WIDTH = 770;
static const int DLGLIST_HEIGHT = 500;
static const int WARNING_DLG_WIDTH = 300;
static const int WARNING_DLG_HEIGHT = 100;
static const int TABLE_ITEM_COLUMN = 200;
static const int INVISIBLE = 0;
static const int READONLY = 1;
static const int CANEDIT = 2;
static const QString DEVICE_TYPE = QObject::tr("Device Type");               // 设备类型
static const QString SELECT_DEVICE_TYPE = QObject::tr("Select Device Type"); // 选择设备类型
// static const QString SELECT_PROJECT_TYPE = "选择工程类型";
static const QString NEW_DEVICE_TYPE = QObject::tr("New Device Type");       // 新建设备类型
static const QString DEL_DEVICE_TYPE = QObject::tr("Delete Device Type");    // 删除设备类型
static const QString REMOVE_DEVICE_TYPE = QObject::tr("Remove Device Type"); // 移除设备类型
static const QString NODE_OBJECT = QObject::tr("Node Object");               // 节点对象
static const QString SELECT_NODE = QObject::tr("Select Node");               // 选择节点
static const QString REMOVE_NODE = QObject::tr("Remove Node");               // 移除节点
// title
static const QString EDIT_DEVICE_TYPE = QObject::tr("Edit Device Type"); // 编辑设备类型

// 电气属性窗口的"设备类型"tag页内容
class ElectricalDeviceType : public CWidget
{
    Q_OBJECT
public:
    ElectricalDeviceType(QSharedPointer<Kcc::BlockDefinition::ElectricalBlock> block, QWidget *parent = nullptr);
    ~ElectricalDeviceType();
    bool saveData();
    virtual void setCWidgetReadOnly(bool bReadOnly) override;
    virtual bool checkLegitimacy(QString &errorinfo);

private slots:
    void onEditClicked(bool checked);
    void onTypenameChanged(const QString &text);
    void onSelectDevieTypeClicked();
    void onNewProjTypeClicked();
    void onRemoveTypeClicked();
    void onTypeButtonClicked(bool checked);

    void onGotoClicked(bool checked);
    void onSelectNodeClicked();
    void onRemoveNodeClicked();

public slots:
    void onSyncLineComponentInfo();

signals:
    void deviceTypeChanged(QSharedPointer<Kcc::BlockDefinition::DeviceModel> deviceType);

private:
    void initUI();
    QString getPath(const QString &devicename);
    QString getNodePath(QSharedPointer<Kcc::BlockDefinition::ElectricalBlock> curLineComponent);
    QString getNewName();
    void updateViewData();
    void openDeviceListDlg(const QString &strtype);
    void openDeviceTypeDlg(const QString &title, QSharedPointer<Kcc::BlockDefinition::DeviceModel> devicemodel,
                           bool isnewtype = false);
    void openWarningDlg(const QString &tips);
    void creatTypeMenu();
    void setEnabelSelectNodeStatus();

private:
    Ui::ElectricalDeviceType ui;
    QSharedPointer<Kcc::BlockDefinition::ElectricalBlock> m_pElectricalBlock;
    CustomTableWidget *m_customTableWidget;
    SelectNodeWidget *m_pSelectNodeWidget;
    QList<CustomModelItem> m_oldList;
    QString m_oldName;
    QList<QSharedPointer<Kcc::BlockDefinition::DeviceModel>> m_DeviceList;
    QSharedPointer<Kcc::BlockDefinition::DeviceModel> m_currentDevicetype;
    QSharedPointer<Kcc::BlockDefinition::DeviceModel> m_originalDevicetype;
    QSharedPointer<Kcc::BlockDefinition::ElectricalBlock> m_pCurLineComponent;
    QMenu *m_menu;
    QAction *m_pActSelectDeviceType;
    QAction *m_pActNewProj;
    QAction *m_pActRemoveType;
    QMenu *m_menuNode;
    QAction *m_pActSelectNode;
    QAction *m_pActRemoveNode;
};

// 设备类型选择、删除画面
class DeviceTypeListWidget : public QWidget
{
    Q_OBJECT
public:
    DeviceTypeListWidget(QWidget *parent = nullptr);
    ~DeviceTypeListWidget();
    void updateDeviceListModelData(const QString &widgettype,
                                   const QList<QSharedPointer<Kcc::BlockDefinition::DeviceModel>> &devicelist,
                                   QSharedPointer<Kcc::BlockDefinition::DeviceModel> sysdevice);
    QSharedPointer<Kcc::BlockDefinition::DeviceModel> getSelectDeviceType() { return m_tableSelectedType; }
    QString getWidgetType() { return m_widgetType; }
private slots:
    void onSearchColBoxChanged(int col);
    void onSearchTextChanged(const QString &strtext);
    void onTableViewClicked(const QModelIndex &index);

private:
    void updateFilterContents();

    void setTableColumFixedWidth(const int &width);

private:
    QString m_widgetType;
    DeviceTypeModel *m_model;
    CustomSortFilterProxyModel *m_sortFilterProxyModel;
    TableView *m_tableview;
    KLineEdit *m_LineEdit;
    QComboBox *m_comboBox;
    QSharedPointer<Kcc::BlockDefinition::DeviceModel> m_tableSelectedType;
    bool m_bFirstInit;
};

// 线路元件节点选择窗口
class SelectNodeWidget : public QWidget
{
    Q_OBJECT
public:
    SelectNodeWidget(const QString &curDrawingBoardName, QWidget *parent = nullptr);
    ~SelectNodeWidget();
    void setInitNodeInfo(const QString &nodeDrawingBoardName = QString(), const QString &nodeName = QString());
    QSharedPointer<Kcc::BlockDefinition::Model> getSelectLineComponent() { return m_pNodeLineModel; }

private slots:
    void onClickedTreeItem(const QModelIndex &index);
    void onTableViewClicked(const int &row, const int &col);

private:
    void initUI();
    void initData(const QString &curDrawingBoardName);
    void initBoardData(QSharedPointer<Kcc::BlockDefinition::Model> drawboard);

private:
    QString m_widgetType;
    QComboBox *m_pComboBox;
    CustomSortFilterProxyModel *m_pSortFilterProxyModel;
    QTableWidget *m_pTableWidget;
    TreeView *m_pTreeView;
    KLineEdit *m_pLineEdit;
    QStandardItemModel *m_pTreeModel;
    QSharedPointer<Kcc::BlockDefinition::Model> m_pNodeLineModel;

    bool m_bFirstInit;
    QMap<QString, QSharedPointer<Kcc::BlockDefinition::Model>> m_lineComponentsMap;
    QMap<QString, QMap<QString, QSharedPointer<Kcc::BlockDefinition::Model>>> m_nodeInfoMap;
};
#endif // ELECTRICALDEVICETYPE_H
