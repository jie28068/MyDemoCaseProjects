#ifndef CUSTOMTYPESELECTWIDGET_H
#define CUSTOMTYPESELECTWIDGET_H

#include "TypeItemView.h"
#include "ui_customtypeselectwidget.h"
#include <QAction>
#include <QMenu>

// 自定义通用类型选择、名称控件
class CustomTypeSelectWidget : public CWidget
{
    Q_OBJECT

public:
    // 默认的类型选择Action
    enum MenuActionType {
        SelectGlobal = 0x01,
        SelectProject = 0x02,
        CreateNew = 0x04,
        Remove = 0x08,

        UserCustom = 0x0100
    };
    Q_DECLARE_FLAGS(MenuActionTypes, MenuActionType)

    CustomTypeSelectWidget(QWidget *parent = nullptr);
    ~CustomTypeSelectWidget();

    // 名称label以及名称输入行是否可见
    void setNameRowVisible(bool visible);

    // 名称label
    void setLabelNameText(const QString &strText);

    // 名称LineEdit
    void setLineEditNameReadOnly(bool readOnly);
    void setLineEditNameText(const QString &strText);
    QString getLineEditNameText() const;

    // 类型label
    void setLabelTypeText(const QString &strText);

    // 类型向下箭头按钮
    void setButtonTypeVisible(bool visible);
    QMenu *getMenu() const; // 获取Menu对象，方便外部新增自定义Action
    void setMenuActions(MenuActionTypes types);
    void setActionEnable(MenuActionTypes types, bool enable);
    void setActionText(MenuActionType type, const QString &strText);

    // 编辑按钮
    void setButtonEditEnable(bool enable);
    void setButtonEditVisiable(bool visible);

    // 路径显示
    void setLineEditPathText(const QString &strText);
    QString getLineEditPathText() const;

signals:
    // 类型选择按钮弹出菜单后选择action时发出此信号
    void actionTriggered(CustomTypeSelectWidget::MenuActionType type, QAction *action);

    // 编辑按钮点击信号
    void buttonEditClicked();

private slots:
    void onActionTriggered();

private:
    Ui::CustomTypeSelectWidget ui;

    QMenu *m_menu;
    QAction *m_pActSelectGlobal;
    QAction *m_pActSelectProject;
    QAction *m_pActCreateNew;
    QAction *m_pActRemove;
};

Q_DECLARE_METATYPE(CustomTypeSelectWidget::MenuActionType)
Q_DECLARE_OPERATORS_FOR_FLAGS(CustomTypeSelectWidget::MenuActionTypes)

// 简易的名称选择Widget
class SelectNameListWidget : public CWidget
{
public:
    explicit SelectNameListWidget(QWidget *parent = nullptr);

    void setNameList(const QStringList &listBlockNames);

    QString getSelectedName() const;

private:
    QComboBox *m_pComboBox;
};

#endif // CUSTOMTYPESELECTWIDGET_H
