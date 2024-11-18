#include "customtypeselectwidget.h"
#include <QComboBox>

CustomTypeSelectWidget::CustomTypeSelectWidget(QWidget *parent) : CWidget(parent)
{
    ui.setupUi(this);
    ui.gridLayout->setContentsMargins(0, 0, 0, 0);

    m_menu = new QMenu(this);

    m_pActSelectGlobal = new QAction(tr("Select Global Type"), this);   // 选择全局类型
    m_pActSelectProject = new QAction(tr("Select Project Type"), this); // 选择工程类型
    m_pActCreateNew = new QAction(tr("New Type"), this);                // 新建类型
    m_pActRemove = new QAction(tr("Remove Type"), this);                // 移除类型

    m_menu->addAction(m_pActSelectGlobal);
    m_menu->addAction(m_pActSelectProject);
    m_menu->addSeparator();
    m_menu->addAction(m_pActCreateNew);
    m_menu->addSeparator();
    m_menu->addAction(m_pActRemove);
    ui.buttonType->setMenu(m_menu);
    QIcon icon = QApplication::style()->standardIcon(QStyle::SP_TitleBarUnshadeButton);
    QPixmap pixmap = icon.pixmap(QSize(20, 20));
    ui.buttonType->setIcon(QIcon(pixmap.scaled(QSize(100, 100))));
    ui.buttonType->setStyleSheet("QPushButton::menu-indicator{image:none;}");

    connect(ui.buttonEdit, SIGNAL(clicked(bool)), this, SIGNAL(buttonEditClicked()));
    connect(m_pActSelectGlobal, SIGNAL(triggered()), this, SLOT(onActionTriggered()));
    connect(m_pActSelectProject, SIGNAL(triggered()), this, SLOT(onActionTriggered()));
    connect(m_pActCreateNew, SIGNAL(triggered()), this, SLOT(onActionTriggered()));
    connect(m_pActRemove, SIGNAL(triggered()), this, SLOT(onActionTriggered()));
}

CustomTypeSelectWidget::~CustomTypeSelectWidget() { }

void CustomTypeSelectWidget::setNameRowVisible(bool visible)
{
    ui.labelName->setVisible(visible);
    ui.lineEditName->setVisible(visible);
}

void CustomTypeSelectWidget::setLabelNameText(const QString &strText)
{
    ui.labelName->setText(strText);
}

void CustomTypeSelectWidget::setLineEditNameReadOnly(bool readOnly)
{
    return ui.lineEditName->setReadOnly(readOnly);
}

void CustomTypeSelectWidget::setLineEditNameText(const QString &strText)
{
    return ui.lineEditName->setText(strText);
}

QString CustomTypeSelectWidget::getLineEditNameText() const
{
    return ui.lineEditName->text();
}

void CustomTypeSelectWidget::setLabelTypeText(const QString &strText)
{
    ui.labelType->setText(strText);
}

void CustomTypeSelectWidget::setButtonTypeVisible(bool visible)
{
    ui.buttonType->setVisible(visible);
}

QMenu *CustomTypeSelectWidget::getMenu() const
{
    return m_menu;
}

void CustomTypeSelectWidget::setMenuActions(MenuActionTypes types)
{
    m_menu->clear();
    if (types & SelectGlobal || types & SelectProject) {
        if (types & SelectGlobal) {
            m_menu->addAction(m_pActSelectGlobal);
        }
        if (types & SelectProject) {
            m_menu->addAction(m_pActSelectProject);
        }
        m_menu->addSeparator();
    }
    if (types & CreateNew) {
        m_menu->addAction(m_pActCreateNew);
        m_menu->addSeparator();
    }
    if (types & Remove) {
        m_menu->addAction(m_pActRemove);
        m_menu->addSeparator();
    }
}

void CustomTypeSelectWidget::setActionEnable(MenuActionTypes types, bool enable)
{
    if (types & SelectGlobal) {
        m_pActSelectGlobal->setEnabled(enable);
    }
    if (types & SelectProject) {
        m_pActSelectProject->setEnabled(enable);
    }
    if (types & CreateNew) {
        m_pActCreateNew->setEnabled(enable);
    }
    if (types & Remove) {
        m_pActRemove->setEnabled(enable);
    }
}

void CustomTypeSelectWidget::setActionText(MenuActionType type, const QString &strText)
{
    switch (type) {
    case SelectGlobal:
        m_pActSelectGlobal->setText(strText);
        break;
    case SelectProject:
        m_pActSelectProject->setText(strText);
        break;
    case CreateNew:
        m_pActCreateNew->setText(strText);
        break;
    case Remove:
        m_pActRemove->setText(strText);
        break;
    }
}

void CustomTypeSelectWidget::setButtonEditEnable(bool enable)
{
    ui.buttonEdit->setEnabled(enable);
}

void CustomTypeSelectWidget::setButtonEditVisiable(bool visible)
{
    ui.buttonEdit->setVisible(visible);
}

void CustomTypeSelectWidget::setLineEditPathText(const QString &strText)
{
    ui.linePath->setElideText(strText);
}

QString CustomTypeSelectWidget::getLineEditPathText() const
{
    return ui.linePath->text();
}

void CustomTypeSelectWidget::onActionTriggered()
{
    QAction *action = dynamic_cast<QAction *>(sender());
    if (action == nullptr) {
        return;
    }

    if (action == m_pActSelectGlobal) {
        emit actionTriggered(SelectGlobal, m_pActSelectGlobal);
    } else if (action == m_pActSelectProject) {
        emit actionTriggered(SelectProject, m_pActSelectProject);
    } else if (action == m_pActCreateNew) {
        emit actionTriggered(CreateNew, m_pActCreateNew);
    } else if (action == m_pActRemove) {
        emit actionTriggered(Remove, m_pActRemove);
    } else {
        emit actionTriggered(UserCustom, action);
    }
}

//////////////////////////////////////////////////////////////////////////

SelectNameListWidget::SelectNameListWidget(QWidget *parent /*= nullptr*/) : CWidget(parent)
{
    auto layout = new QHBoxLayout(this);
    auto label = new QLabel(tr("Select Module"), this); // 选择模块：
    m_pComboBox = new QComboBox(this);
    m_pComboBox->setView(new QListView());

    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_pComboBox->setSizePolicy(sizePolicy);

    layout->addWidget(label);
    layout->addWidget(m_pComboBox);
    setLayout(layout);
}

void SelectNameListWidget::setNameList(const QStringList &listBlockNames)
{
    m_pComboBox->clear();
    m_pComboBox->addItems(listBlockNames);
}

QString SelectNameListWidget::getSelectedName() const
{
    return m_pComboBox->currentText();
}
