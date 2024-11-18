#include "ControlParameterWidget.h"

#include "KLModelDefinitionCore/KLModelDefinitionCore.h"
#include "PropertyServerMng.h"
#include <QTextBrowser>

using namespace Kcc::BlockDefinition;

USE_LOGOUT_("ControlParameterWidget")
ControlParameterWidget::ControlParameterWidget(QSharedPointer<Kcc::BlockDefinition::Model> model, QWidget *parent)
    : CWidget(parent),
      m_tableWidget(nullptr),
      m_pModelNameListWidget(nullptr),
      m_pModel(model),
      m_pCurrentDrawBoard(nullptr)
{
    InitUI();
}

ControlParameterWidget::~ControlParameterWidget() { }

QList<CustomModelItem> ControlParameterWidget::getNewListData()
{
    if (m_tableWidget != nullptr) {
        return m_tableWidget->getNewListData();
    }
    return QList<CustomModelItem>();
}

void ControlParameterWidget::updateCustomTableWidget()
{
    QList<CustomModelItem> listdata = CMA::getPropertyModelItemList(m_pModel, RoleDataDefinition::Parameter);
    QStringList listheader;
    bool havepath = false;
    for (CustomModelItem item : listdata) {
        if (item.keywords.contains("/")) {
            havepath = true;
            break;
        }
    }
    if (havepath) {
        listheader = QStringList() << CMA::HEADER_PATH << CMA::HEADER_PARAMNAME << CMA::HEADER_PARAMVALUE
                                   << CMA::HEADER_DESCRIPTION;
    } else {
        listheader = QStringList() << CMA::HEADER_PARAMNAME << CMA::HEADER_PARAMVALUE << CMA::HEADER_DESCRIPTION;
    }
    m_tableWidget->setListData(listdata, listheader);
}

void ControlParameterWidget::setCWidgetReadOnly(bool bReadOnly)
{
    if (m_tableWidget != nullptr) {
        m_tableWidget->setCWidgetReadOnly(bReadOnly);
    }
    if (m_pModelNameListWidget != nullptr) {
        m_pModelNameListWidget->setDisabled(bReadOnly);
    }
}

bool ControlParameterWidget::checkLegitimacy(QString &errorinfo)
{
    if (m_tableWidget != nullptr) {
        return m_tableWidget->checkLegitimacy(errorinfo);
    }
    return true;
}

void ControlParameterWidget::onShowSpecifiedModel(const QUrl &url)
{
    QTextBrowser *pTextBrowser = qobject_cast<QTextBrowser *>(sender());
    QString str = url.toString();
    if (pTextBrowser) {
        auto strShowText =
                QString("<a href='%1' style='color: blue; text-decoration: underline;'>%2</a>").arg(str).arg(str);
        pTextBrowser->setHtml(strShowText);
    }

    QStringList pathList = str.split("/");
    if (pathList.isEmpty()) {
        return;
    }

    auto mainDrawBoardModel = getTopParentBoard(m_pModel);
    if (mainDrawBoardModel == nullptr) {
        return;
    }

    int nIndex = 1; // 用于忽略pathList中第一个元素，因为pathList中第一个元素名称就是顶层画板名称
    // 获取用户选择的顶层画板mainDrawBoardModel中的子系统对象
    PModel subSystemModel = getSubSystemModel(mainDrawBoardModel, pathList, nIndex);
    if (subSystemModel == nullptr) {
        return;
    }

    auto blockModelsMap = subSystemModel->getChildModels();
    auto mapValues = blockModelsMap.values();
    for (auto blockModel : mapValues) {
        if (blockModel->getName() == pathList.last()) {
            if (mainDrawBoardModel->getUUID() == subSystemModel->getUUID()) { // 说明顶层画板没有子系统
                subSystemModel = PModel();                                    // 子系统对象置空
            }
            if (!PropertyServerMng::getInstance().m_pGraphicsModelingServer->setDisplayAndHighlightModel(
                        mainDrawBoardModel, blockModel, subSystemModel)) {
                LOGOUT(tr("Failed to locate and redirect to the specified module! Please check if the module data "
                          "exists."),
                       LOG_ERROR); // 定位跳转到指定模块失败！请检查是否存在该模块数据。
            }
            break;
        }
    }
}

void ControlParameterWidget::InitUI()
{
    QVBoxLayout *pMainVBoxLayout = new QVBoxLayout(this);
    pMainVBoxLayout->setMargin(0);
    pMainVBoxLayout->setSpacing(8);
    m_tableWidget = new CustomTableWidget();
    updateCustomTableWidget();
    pMainVBoxLayout->addWidget(m_tableWidget, 1);
    if (m_pModel != nullptr && m_pModel->getParentModel() != nullptr
        && m_pModel->getPrototypeName() == NPS::PROTOTYPENAME_GOTO) {
        QWidget *pHeader = new QWidget(this);
        pHeader->setObjectName("Header");

        QHBoxLayout *hLayout = new QHBoxLayout;
        hLayout->addStretch();
        hLayout->addWidget(new QLabel(tr("Corresponding module"))); // 对应的模块
        hLayout->addStretch();
        hLayout->setMargin(0);
        pHeader->setLayout(hLayout);

        m_pModelNameListWidget = new QListWidget(this);
        m_pModelNameListWidget->setObjectName("ModelNameListWidget");

        QVBoxLayout *pVBoxLayout = new QVBoxLayout(this);
        pVBoxLayout->setSpacing(0);
        pVBoxLayout->addWidget(pHeader);
        pVBoxLayout->addWidget(m_pModelNameListWidget);
        pMainVBoxLayout->addLayout(pVBoxLayout, 15);

        connect(m_pModelNameListWidget, &QListWidget::itemClicked,
                [&](QListWidgetItem *item) { QString strItemName = item->text(); });

        initData();
    }
}

void ControlParameterWidget::initData()
{
    if (m_pModelNameListWidget == nullptr || m_pModel == nullptr) {
        return;
    }

    if (m_pModel->getPrototypeName() != NPS::PROTOTYPENAME_GOTO
        && m_pModel->getPrototypeName() != NPS::PROTOTYPENAME_FROM) {
        return;
    }

    m_pCurrentDrawBoard = m_pModel->getParentModel();
    if (m_pCurrentDrawBoard == nullptr) {
        return;
    }

    auto drawBoard = m_pCurrentDrawBoard.dynamicCast<DrawingBoardClass>();
    if (drawBoard == nullptr) {
        return;
    }
    QStringList showTextList = initCorrespondModelsData(m_pCurrentDrawBoard, m_pCurrentDrawBoard->getName());

    for (auto &text : showTextList) {
        auto strShowText =
                QString("<a href='%1' style='color: blue; text-decoration: underline;'>%2</a>").arg(text).arg(text);
        QTextBrowser *pTextBrowser = new QTextBrowser(m_pModelNameListWidget);
        pTextBrowser->setObjectName("ControlParameterWidget_TextBrowser");
        pTextBrowser->setHtml(strShowText);

        QListWidgetItem *item = new QListWidgetItem(m_pModelNameListWidget);
        m_pModelNameListWidget->setItemWidget(item, pTextBrowser);
        connect(pTextBrowser, &QTextBrowser::anchorClicked, this, &ControlParameterWidget::onShowSpecifiedModel);
    }
}

QStringList ControlParameterWidget::initCorrespondModelsData(QSharedPointer<Kcc::BlockDefinition::Model> drawBoard,
                                                             QString boardPath)
{
    auto canvasContext = drawBoard->getCanvasContext();
    if (canvasContext == nullptr) {
        return QStringList();
    }

    QStringList showTextList;
    QString strParentBoardPath = getParentBoardPath(drawBoard) + boardPath;
    showTextList += getSubSystemCorrespondData(drawBoard, strParentBoardPath);
    showTextList += getParentBoardCorrespondData(drawBoard, boardPath);
    return showTextList;
}

QSharedPointer<Kcc::BlockDefinition::Model>
ControlParameterWidget::getSubSystemModel(QSharedPointer<Kcc::BlockDefinition::Model> model,
                                          const QStringList &pathList, const int &index)
{
    QString strName = pathList.at(index);
    auto uuid = model->getUuidByChildName(strName);
    auto subSystemModel = model->getChildModel(uuid);
    if (subSystemModel == nullptr) {
        return PModel();
    }

    if (0 != subSystemModel->getChildModels().size()) {
        int nIndex = index + 1;
        return getSubSystemModel(subSystemModel, pathList, nIndex);
    } else {
        return model;
    }
}

QStringList ControlParameterWidget::getSubSystemCorrespondData(QSharedPointer<Kcc::BlockDefinition::Model> drawBoard,
                                                               QString boardPath)
{
    QStringList showTextList;
    QString strParamName = m_pModel->getValueByRoleData(RoleDataDefinition::Parameter, RoleDataDefinition::NameRole,
                                                        NPS::KEYWORDS_GOTO_TAG)
                                   .toString();

    for (auto childModel : drawBoard->getChildModels()) {
        if (!childModel || m_pCurrentDrawBoard->getUUID() == childModel->getUUID()
            || (childModel->getPrototypeName() != NPS::PROTOTYPENAME_GOTO
                && childModel->getPrototypeName() != NPS::PROTOTYPENAME_FROM
                && childModel->getModelType() != Model::Combine_Board_Type)) {
            continue;
        }
        QString strShowText = boardPath;
        strShowText += "/";
        strShowText += childModel->getName();
        QString strTempPrototypeName = childModel->getPrototypeName();
        QString strTempDisplayData = childModel
                                             ->getValueByRoleData(RoleDataDefinition::Parameter,
                                                                  RoleDataDefinition::NameRole, NPS::KEYWORDS_GOTO_TAG)
                                             .toString();

        if (strTempPrototypeName == NPS::PROTOTYPENAME_FROM && strTempDisplayData == strParamName) {
            showTextList.append(strShowText);
        } else {
            if (0 != childModel->getChildModels().size()) {
                showTextList += getSubSystemCorrespondData(childModel, strShowText);
            }
        }
    }
    std::sort(showTextList.begin(), showTextList.end(), [](const QString &s1, const QString &s2) { return s1 < s2; });
    return showTextList;
}

QStringList ControlParameterWidget::getParentBoardCorrespondData(QSharedPointer<Kcc::BlockDefinition::Model> drawBoard,
                                                                 QString boardPath)
{
    QStringList showTextList;
    QString strParamName = m_pModel->getValueByRoleData(RoleDataDefinition::Parameter, RoleDataDefinition::NameRole,
                                                        NPS::KEYWORDS_GOTO_TAG)
                                   .toString();
    auto drawBoardParent = drawBoard->getParentModel();
    if (drawBoardParent == nullptr) {
        return QStringList();
    }

    QString strParentBoardPath;
    auto parent = drawBoardParent->getParentModel();
    if (parent == nullptr) {
        strParentBoardPath = drawBoardParent->getName();
    } else {
        strParentBoardPath = getParentBoardPath(drawBoard) + drawBoardParent->getName();
    }

    showTextList << getSubSystemCorrespondData(drawBoardParent, strParentBoardPath);
    showTextList << getParentBoardCorrespondData(drawBoardParent, strParentBoardPath);

    return showTextList;
}

QString ControlParameterWidget::getParentBoardPath(QSharedPointer<Kcc::BlockDefinition::Model> model)
{
    QString showTextList;
    auto parent = model->getParentModel();
    if (parent == nullptr) {
        return showTextList;
    }

    QString strShowText;
    strShowText += parent->getName();
    strShowText += "/";
    showTextList.push_front(strShowText);

    QString strPath = getParentBoardPath(parent);
    if (strPath.isEmpty()) {
        return showTextList;
    }

    showTextList.push_front(strPath);
    return showTextList;
}

QSharedPointer<Kcc::BlockDefinition::Model>
ControlParameterWidget::getTopParentBoard(QSharedPointer<Kcc::BlockDefinition::Model> model)
{
    auto parent = model->getParentModel();
    if (parent == nullptr) {
        return model;
    }
    return getTopParentBoard(parent);
}
