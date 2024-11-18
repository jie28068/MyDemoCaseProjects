#include "variablewidget.h"

#include "CommonModelAssistant.h"
#include "KLModelDefinitionCore/KLModelDefinitionCore.h"
#include <QFocusEvent>
#include <QPushButton>
#include <QRegExpValidator>
#include <QSpacerItem>
using namespace Kcc::BlockDefinition;

static const int MAX_VAR_NAME_COUNT = 300; // 模块变量最大个数

void VarLineEdit::focusOutEvent(QFocusEvent *event)
{
    QLineEdit::focusOutEvent(event);
    if (event->lostFocus() && isModified()) {
        setModified(false);
        VariableWidget *varWidget = dynamic_cast<VariableWidget *>(parentWidget());
        QGridLayout *layout = varWidget ? qobject_cast<QGridLayout *>(varWidget->layout()) : nullptr;
        int index = layout ? layout->indexOf(this) : -1;
        if (index >= 0) {
            int row = -1, _;
            layout->getItemPosition(index, &row, &_, &_, &_);
            if (row >= 0) {
                varWidget->setVars(row, text().split(','));
            }
        }
    }
}

VariableWidget::VariableWidget(QSharedPointer<Kcc::BlockDefinition::Model> model, QWidget *parent /*= nullptr*/)
    : CWidget(parent), m_model(model), m_pUpdateBtn(nullptr)
{
    m_allDataList.clear();
    m_VarNames.clear();
    m_allGroupName.clear();
    initUI();
    m_VarNames = getAllCtrlBlockParaNames(m_model);
}

VariableWidget::~VariableWidget() { }

bool VariableWidget::saveData(QSharedPointer<Kcc::BlockDefinition::Model> model)
{
    if (model == nullptr) {
        return false;
    }
    bool changed = false;
    for (VarWidgetData varData : m_allDataList) {
        QList<PVariable> varlist = CMA::getVarGroupList(model, varData.groupName);
        if (varData.lineEdit == nullptr) {
            continue;
        }
        QList<QString> varNames = varData.lineEdit->text().split(',');
        for (int i = 0; i < varlist.size(); ++i) {
            if (varlist[i] == nullptr) {
                continue;
            }
            QString realAlias;
            if (varNames.size() > i && !varNames[i].isEmpty()) {
                realAlias = varNames[i];
            } else {
                realAlias = updateVarName(varlist[i]->getName());
            }
            if (!varlist[i]->getData(RoleDataDefinition::ParameterNameRole).toString().isEmpty()
                && varlist[i]->getData(RoleDataDefinition::ParameterNameRole).toString() != realAlias) {
                varlist[i]->setData(RoleDataDefinition::ParameterNameRole, realAlias);
                changed = true;
            } else {
                varlist[i]->setData(RoleDataDefinition::ParameterNameRole, realAlias);
            }
        }
    }
    return changed;
}

bool VariableWidget::checkLegitimacy(QString &errorinfo)
{
    errorinfo = "";
    // 判断个数
    if (!checkVarSum(MAX_VAR_NAME_COUNT)) {
        errorinfo = tr("the number of variables cannot exceed %1!").arg(MAX_VAR_NAME_COUNT); // 变量个数不能超过 %1!
        return false;
    }
    if (!checkVarnameValid(errorinfo)) {
        return false;
    }
    return true;
}

void VariableWidget::setCWidgetReadOnly(bool bReadOnly)
{
    for (VarWidgetData vardata : m_allDataList) {
        if (vardata.lineEdit != nullptr) {
            vardata.lineEdit->setDisabled(bReadOnly);
        }
    }

    if (m_pUpdateBtn != nullptr) {
        m_pUpdateBtn->setDisabled(bReadOnly);
    }
}

void VariableWidget::setVars(int index, const QList<QString> &varLst)
{
    // for (int i = 0; i < m_tabList.size(); ++i) {
    //     if (i != index) {
    //         continue;
    //     }
    //     QList<QString> orgLst = m_Vars[m_tabList[i]];
    //     // 可能存在模块变量名相同的情况，显示时隐藏了同名变量，这里更新时要还原回来
    //     // e.g. 原始变量名为"yi, yo, y1, yo, y2"，显示为"yi,yo,y1,y2"，用户修改为"yi1,yo1"
    //     // 先建立映射，yi -> yi1, yo -> yo1，再还原到内部时为"yi1,yo1,y1,yo1,y2"
    //     QList<QString> orgNotDumplicatedLst = removeDumplicates(orgLst);
    //     const int orgLen = orgNotDumplicatedLst.length();
    //     const int curLen = varLst.length();
    //     QList<QString> curLst = varLst.mid(0, curLen > orgLen ? orgLen : curLen);
    //     QMap<QString, QString> newVarMap;
    //     auto curIt = curLst.begin();
    //     auto orgIt = orgNotDumplicatedLst.begin();
    //     for (; curIt != curLst.end() && orgIt != orgNotDumplicatedLst.end(); curIt++, orgIt++) {
    //         if (*curIt != *orgIt) {
    //             newVarMap[*orgIt] = *curIt;
    //         }
    //     }

    //     for (orgIt = orgLst.begin(); orgIt != orgLst.end(); orgIt++) {
    //         if (newVarMap.contains(*orgIt)) {
    //             *orgIt = newVarMap[*orgIt];
    //         }
    //     }

    //     m_Vars[m_tabList[i]] = orgLst;
    //     break;
    // }
}

void VariableWidget::onUpdate()
{
    int idx = 0;
    QStringList curVarName;
    for (int i = 0; i < m_allDataList.size(); ++i) {
        if (m_allDataList[i].lineEdit == nullptr) {
            continue;
        }
        QStringList editVarNames = m_allDataList[i].lineEdit->text().split(",");
        // 如果输入框个数不等于变量数，同步变量个数
        QStringList realNameList;
        for (int varno = 0; varno < m_allDataList[i].varList.size(); ++varno) {
            if (editVarNames.size() > varno && !editVarNames[varno].isEmpty()) {
                realNameList.append(editVarNames[varno]);
            } else {
                realNameList.append(m_allDataList[i].varList[varno]);
            }
        }

        for (int j = 0; j < realNameList.size(); ++j) {
            QString strAliasName = realNameList[j];
            for (int k = 1; m_VarNames.contains(strAliasName) || curVarName.contains(strAliasName); ++k) {
                strAliasName = splitVariable(strAliasName); // 去掉后面的数字
                strAliasName += QString::number(k);         // 加上新数字
            }
            curVarName.append(strAliasName);
            realNameList[j] = strAliasName;
        }
        m_allDataList[i].lineEdit->setText(realNameList.join(","));
        // setVars(i, realNameList);
    }
}

void VariableWidget::initUI()
{
    QGridLayout *gridlayout = new QGridLayout(this);
    gridlayout->setMargin(0);
    gridlayout->setVerticalSpacing(8);
    gridlayout->setHorizontalSpacing(10);
    addVarGroupWidget(gridlayout, CMA::BLOCK_TAB_NAME_CTRL_PARAM, RoleDataDefinition::Parameter);
    addVarGroupWidget(gridlayout, CMA::BLOCK_TAB_NAME_DISCRETE_STATE, RoleDataDefinition::DiscreteStateVariable);
    addVarGroupWidget(gridlayout, CMA::BLOCK_TAB_NAME_CONTINUE_STATE, RoleDataDefinition::ContinueStateVariable);

    if (gridlayout->rowCount() > 0) {
        m_pUpdateBtn = new QPushButton(this);
        m_pUpdateBtn->setText(tr("update")); // 更新
        m_pUpdateBtn->setFixedSize(50, 25);
        m_pUpdateBtn->setAccessibleName("VariableWidget_UpdateBtn");
        connect(m_pUpdateBtn, SIGNAL(clicked()), this, SLOT(onUpdate()));
        gridlayout->addWidget(m_pUpdateBtn, gridlayout->rowCount(), 1, 1, 1);
    }

    QSpacerItem *verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    gridlayout->addItem(verticalSpacer, gridlayout->rowCount(), 0, 1, 12);
    this->setLayout(gridlayout);
}

void VariableWidget::initLabel(QLabel *&label)
{
    if (label != nullptr) {
        label->setMinimumSize(QSize(80, 0));
        label->setMaximumSize(QSize(80, 16777215));
    }
}

void VariableWidget::initLineEdit(QLineEdit *&lineEdit)
{
    if (lineEdit != nullptr) {
        QRegExpValidator *pVariableValidator = new QRegExpValidator(QRegExp(NPS::REG_VARIABLES), this);
        QSizePolicy sizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(lineEdit->sizePolicy().hasHeightForWidth());
        lineEdit->setSizePolicy(sizePolicy);
        lineEdit->setValidator(pVariableValidator);
    }
}

QString VariableWidget::splitVariable(const QString &name)
{
    if (name.isEmpty()) {
        return name;
    }

    for (int i = name.size() - 1; i >= 0; i--) {
        if (name[i].isLetter()) {
            return name.mid(0, i + 1);
        }
    }

    return name;
}

QStringList VariableWidget::removeDumplicates(const QStringList &orgLst)
{
    QStringList newLst = orgLst;
    newLst.removeDuplicates();
    return newLst;
}

bool VariableWidget::checkVarSum(int maxSize)
{
    for (VarWidgetData &vardata : m_allDataList) {
        if (vardata.lineEdit == nullptr) {
            continue;
        }
        QList<QString> varNames = vardata.lineEdit->text().split(',');
        if (varNames.size() > maxSize) {
            return false;
        }
    }
    return true;
}

bool VariableWidget::checkVarnameValid(QString &errorinfo)
{
    QRegExp variablereg(NPS::REG_ONEVARIABLE);
    for (VarWidgetData &vardata : m_allDataList) {
        if (vardata.lineEdit == nullptr) {
            continue;
        }
        QStringList varNames = vardata.lineEdit->text().split(',');
        for (QString varname : varNames) {
            if (!variablereg.exactMatch(varname)) {
                //%1[%2:%3]格式错误，仅数字字母下划线可作为名称，且名称不能以数字开头，多个名称用','隔开!
                errorinfo = tr("%1 [%2:%3] format is incorrect, only alphanumeric and underline can be used as "
                               "names, and names cannot start with a number, multiple names are separated by ','!")
                                    .arg(CMA::BLOCK_TAB_NAME_VARIABLE)
                                    .arg(vardata.displayName)
                                    .arg(varname);
                return false;
            }
        }
    }
    return true;
}

bool VariableWidget::addVarGroupWidget(QGridLayout *gridlayout, const QString &displayName, const QString &groupName)
{
    if (gridlayout == nullptr || displayName.isEmpty() || groupName.isEmpty()) {
        return false;
    }
    if (!m_allGroupName.contains(groupName)) {
        m_allGroupName.append(groupName);
    }
    QStringList grouplist = getVarKeyList(CMA::getVarGroupList(m_model, groupName));
    if (grouplist.size() > 0) {
        // 添加控件
        int rowcount = gridlayout->rowCount();
        QLabel *plabel = new QLabel(this);
        plabel->setText(displayName);
        gridlayout->addWidget(plabel, rowcount, 0, 1, 1);
        QLineEdit *pLineEdit = new VarLineEdit(this);
        pLineEdit->setAccessibleName(QString("VariableWidget_LineEdit%1").arg(QString::number(rowcount)));
        initLineEdit(pLineEdit);
        QString variablesStr = grouplist.join(",");
        pLineEdit->setText(variablesStr);
        pLineEdit->setToolTip(variablesStr);
        gridlayout->addWidget(pLineEdit, rowcount, 1, 1, 11);
        m_allDataList.append(VarWidgetData(displayName, groupName, grouplist, pLineEdit));
        return true;
    }
    return false;
}

QStringList VariableWidget::getVarKeyList(const QList<QSharedPointer<Kcc::BlockDefinition::Variable>> &grouplist)
{
    QStringList list;
    for (PVariable pavr : grouplist) {
        if (pavr == nullptr) {
            continue;
        }
        // 别名不为空则使用别名
        if (!pavr->getData(RoleDataDefinition::ParameterNameRole).toString().isEmpty()) {
            list.append(pavr->getData(RoleDataDefinition::ParameterNameRole).toString());
        } else {
            list.append(updateVarName(pavr->getName()));
        }
    }
    return list;
}

QStringList VariableWidget::getAllCtrlBlockParaNames(QSharedPointer<Kcc::BlockDefinition::Model> model)
{
    if (model == nullptr || model->getParentModel() == nullptr
        || (ControlBoardModel::Type != model->getParentModel()->getModelType()
            && CombineBoardModel::Type != model->getParentModel()->getModelType())) {
        return QStringList();
    }

    QStringList allvarList;
    for (PModel cmodel : model->getParentModel()->getChildModels().values()) {
        if (cmodel == nullptr || cmodel == model) {
            continue;
        }
        for (QString groupName : m_allGroupName) {
            PVariableGroup group = cmodel->getVariableGroup(groupName);
            if (group == nullptr) {
                continue;
            }
            for (PVariable var : group->getVariableMap().values()) {
                if (var == nullptr) {
                    continue;
                }
                if (!var->getData(RoleDataDefinition::ParameterNameRole).toString().isEmpty()) {
                    allvarList.append(var->getData(RoleDataDefinition::ParameterNameRole).toString());
                } else {
                    allvarList.append(var->getName());
                }
            }
        }
    }
    return allvarList;
}

QString VariableWidget::updateVarName(const QString &varName)
{
    if (varName.isEmpty()) {
        return varName;
    }
    QString name = varName;
    for (int i = 0; i < varName.size(); ++i) {
        if (!varName[i].isLetter() && !varName[i].isNumber() && varName[i] != '_') {
            name.replace(i, 1, '_');
        }
    }
    return name;
}
