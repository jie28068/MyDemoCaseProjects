#ifndef ELECCONTAINERPROPERTY_H
#define ELECCONTAINERPROPERTY_H

#include <QComboBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QStandardItemModel>
#include <QTableView>
#include <QTableWidgetItem>
#include <QWidget>

#include "ElideLineText.h"
#include "KLineEdit.h"
#include "TypeItemView.h"
#include "blockproperty.h"
#include "customtreewidget.h"
#include "tableheaderview.h"

class ElecTypeWidget;
class DrawScene;
class ElecContainerPortTableWidget;
class SourceProxy;

namespace Kcc {
namespace BlockDefinition {
class VariableGroup;
class Model;
class ElectricalBlock;
class ElectricalContainerBlock;
class DrawingBoardClass;
}
}
class ElecContainerProperty : public BlockProperty
{
    Q_OBJECT
public:
    ElecContainerProperty(QSharedPointer<Kcc::BlockDefinition::Model> model, bool isReadOnly = false);
    ~ElecContainerProperty();

    virtual void init();
    void refreshProperty();
    ElecTypeWidget *getTypeWidget();

    // CommonWrapper
    virtual QPixmap getBlockPixmap();
    virtual void onDialogExecuteResult(QDialog::DialogCode code);
    virtual bool checkValue(QString &errorinfo = QString()) override;
    virtual QString getHelpUrl() override;

private:
    void setPortTableWidgetDisable();

    bool setElecInterfaceBlockVariables(const QString &varType);

    bool savePicToSlot(QSharedPointer<Kcc::BlockDefinition::Model> model);

private slots:
    void refreshChoosePortsWidget(QSharedPointer<Kcc::BlockDefinition::ElectricalBlock> EleBlock);

    void refreshPropertyWidgetPix(QString propertyName);

private:
    QSharedPointer<Kcc::BlockDefinition::ElectricalContainerBlock> m_pElecContainerBlock;
    ElecTypeWidget *m_typeWidget; // 容器类型widget
    ElecContainerPortTableWidget *m_pInPortTableWidget;
    ElecContainerPortTableWidget *m_pOutPortTableWidget;
    QString m_strBlockOldName;
    QVariantMap m_variant;
    QString m_strAssociateType;
};

class ElecTypeWidget : public CWidget
{
    Q_OBJECT
public:
    ElecTypeWidget(QSharedPointer<Kcc::BlockDefinition::Model> eleInterfaceModel, QWidget *parent = nullptr);

    QSharedPointer<Kcc::BlockDefinition::ElectricalBlock> getElecBlock() const;

    QString getElecBlockName() const;

    void setElecBlockNewInputCount(const int count);

    virtual bool checkLegitimacy(QString &errorinfo);

signals:
    void blockChanged(QSharedPointer<Kcc::BlockDefinition::ElectricalBlock> EleBlock);
    void typeChanged(QString propertyName);

private slots:
    void onDrawBorderNameChanged(QString drawBorderName);
    void onDrawBorderNameClicked(QTreeWidgetItem *item, int column = -1);

    void onElecTypeChanged(QString ElementName);

    void onElectricalElementChanged(QString ElementName);

    void onGotoPushButtonClicked();

private:
    void initUI();
    void initTreeWidget(QSharedPointer<Kcc::BlockDefinition::DrawingBoardClass> drawingBoard,
                        QTreeWidgetItem *item = nullptr);
    QSharedPointer<Kcc::BlockDefinition::DrawingBoardClass> getBlockDrawBoard(const QString &boarduuid);
    bool hasBlockName(const QString &namestr);
    void creatTypeMenu();

    // 清除之前连接该电气元件的电气接口模块中的信息
    void clearModelInfo(QSharedPointer<Kcc::BlockDefinition::Model> eleInterfaceModel);

    QSharedPointer<Kcc::BlockDefinition::Model>
    checkBlockIsConnected(QList<QWeakPointer<Kcc::BlockDefinition::Model>> modelList);

    void allComponentsOnDrawingBoard(QSharedPointer<Kcc::BlockDefinition::DrawingBoardClass> elecDrawingBoard,
                                     QStringList namePathsList, QString strElementType);

    QStringList getCurrentModelParents(QSharedPointer<Kcc::BlockDefinition::Model> curModel);
    QString getCurrentModelParentsPathText(QSharedPointer<Kcc::BlockDefinition::Model> curModel);

    QList<QTreeWidgetItem *> getMatchingItems(const QString &itemPath);
    void findChildItems(QTreeWidgetItem *parent, const QString &name, QList<QTreeWidgetItem *> &items);

    QSharedPointer<Kcc::BlockDefinition::Model>
    getElecCombineBoardModel(QSharedPointer<Kcc::BlockDefinition::Model> elecBoardModel, const QStringList &pathList,
                             const int &index);

private:
    QSharedPointer<Kcc::BlockDefinition::Model> m_pElecContainerBlock;
    QSharedPointer<Kcc::BlockDefinition::ElectricalBlock> m_pElectricalBlock;
    QSharedPointer<SourceProxy> m_pSourceProxy;

    bool m_isInitProcess;
    bool m_isMesureType;
    ElideLineText *m_typepath;
    QGridLayout *m_gridLayout;
    KLineEdit *m_lineEdit;
    CustomTreeWidget *m_pElecDrawBoardTreeWidget;
    QComboBox *m_pDrawBoardComboBoxTemp;
    QComboBox *m_pElecTypeComboBoxTemp;
    QComboBox *m_pElecElementComboBoxTemp;
    QString m_strDrawBorderName;
    QString m_strAssociateType;
    QPushButton *m_pGotoPushButton;

    QList<QSharedPointer<Kcc::BlockDefinition::Model>> m_drawingBoardList;
    QList<QString> m_elecDrawingBoardNameList;
    QMap<QString, QSharedPointer<Kcc::BlockDefinition::Model>> m_electricalModelMap;

    // 电气画板上的所有模块，key是模块的电气画板名称，value是m_allBlockMap
    QMap<QString, QMap<QString, QString>> m_allBlockMap;
};

class ElecContainerPortTableWidget : public CWidget
{
    Q_OBJECT
public:
    ElecContainerPortTableWidget(QString name, QSharedPointer<Kcc::BlockDefinition::Model> eleInterfaceModel,
                                 QSharedPointer<Kcc::BlockDefinition::Model> elecModel, QWidget *parent = nullptr);

    void displayBlockPortsInfo(QSharedPointer<Kcc::BlockDefinition::ElectricalBlock> electricalBlock);

    QMap<QString, QSharedPointer<Kcc::BlockDefinition::Variable>> getCheckedVaruablesMap(QString tabWidgetType);

    void setWidgetEnable(bool enable);

    void setColumnAllSelected(int col);
    virtual void setCWidgetReadOnly(bool bReadOnly) override;

private:
    void initUI();

    void setColumnCheckable(int col, bool checkable);

private slots:
    void onItemClicked(QModelIndex item);

    void onColumnAllSelected(int col, bool selected);

private:
    QSharedPointer<Kcc::BlockDefinition::ElectricalContainerBlock> m_pCurrentContainerBlock;
    QSharedPointer<Kcc::BlockDefinition::ElectricalBlock> m_pElectricalBlock;
    QString strWidgetName;
    QStandardItemModel *m_pModel;
    TableHeaderView *m_pHeaderView;
    QTableView *m_pInPortTableView;

public:
    QMap<QString, QSharedPointer<Kcc::BlockDefinition::Variable>> m_mapInputVariables;
    QMap<QString, QSharedPointer<Kcc::BlockDefinition::Variable>> m_mapOutputVariables;
};

#endif // ELECCONTAINERPROPERTY_H
