#include "openboardwidget.h"
#include "GlobalAssistant.h"
#include "ProjectViewServers.h"

OpenBoardWidget::OpenBoardWidget(QWidget *parent) : QWidget(parent)
{
    ui.setupUi(this);
    ui.cbox_boardName->setAccessibleName(AssistantDefine::ACCESSNAME_OPENBOARDNAME);
    ui.cbox_boardType->setAccessibleName(AssistantDefine::ACCESSNAME_OPENBOARDTYPE);
    QObject::connect(ui.cbox_boardType, SIGNAL(currentIndexChanged(const QString &)), this,
                     SLOT(onBoardTypeChanged(const QString &)));
}

OpenBoardWidget::~OpenBoardWidget() { }

void OpenBoardWidget::onBoardTypeChanged(const QString &text)
{
    if (ui.cbox_boardName == nullptr) {
        return;
    }
    QStringList boardnames = m_cboxData.value(text);
    ui.cbox_boardName->clear();
    if (boardnames.size() > 0) {
        ui.cbox_boardName->addItems(boardnames);
    }
}

void OpenBoardWidget::setActionType(OpenBoardWidget::ActionType actionType)
{
    m_actionType = actionType;
    initCombox(getInitComboxData());
}
QSharedPointer<Kcc::BlockDefinition::DrawingBoardClass> OpenBoardWidget::getDrawboard()
{
    QString boardname = getBoardName();
    if (boardname.isEmpty()) {
        return PDrawingBoardClass(nullptr);
    }

    QList<PModel> allboards = ProjectViewServers::getInstance().m_projectManagerServer->GetAllBoardModel();
    for (PModel model : allboards) {
        if (model == nullptr) {
            continue;
        }
        if (model->getName() == boardname) {
            return model.dynamicCast<DrawingBoardClass>();
        }
    }
    return PDrawingBoardClass(nullptr);
}

void OpenBoardWidget::initCombox(QMap<QString, QStringList> cboxData)
{
    if (ui.cbox_boardType == nullptr) {
        return;
    }

    m_cboxData = cboxData;
    QStringList boardtypes = m_cboxData.keys();
    ui.cbox_boardType->clear();
    ui.cbox_boardType->addItems(boardtypes);

    ui.cbox_boardType->setCurrentIndex(0);

    QStringList boardnames = m_cboxData.value(ui.cbox_boardType->currentText());
    ui.cbox_boardName->clear();
    if (boardnames.size() > 0) {
        ui.cbox_boardName->addItems(boardnames);
    }
}

QMap<QString, QStringList> OpenBoardWidget::getInitComboxData()
{
    if (ProjectViewServers::getInstance().m_projectManagerServer == nullptr
        || ProjectViewServers::getInstance().m_pGraphicModelingServer == nullptr) {
        return QMap<QString, QStringList>();
    }
    // 初始化打开画板窗口
    auto boardList = ProjectViewServers::getInstance().m_projectManagerServer->GetAllBoardModel();
    QMap<QString, QStringList> cboxData;
    for (auto board : boardList) {
        auto pDrawingBoardClass = board.dynamicCast<DrawingBoardClass>();
        if (pDrawingBoardClass == nullptr) {
            continue;
        }
        int boardType = pDrawingBoardClass->getModelType();
        QString strBoardType;
        if (ElecBoardModel::Type == boardType) {
            strBoardType = AssistantDefine::STR_BOARDTYPE_ELECTRICALSYSTEM;
        } else if (ControlBoardModel::Type == boardType) {
            strBoardType = AssistantDefine::STR_BOARDTYPE_CONTROLSYSTEM;
        } else if (CombineBoardModel::Type == boardType) {
            strBoardType = AssistantDefine::STR_BOARDTYPE_COMBINEMODEL;
        } else if (ComplexBoardModel::Type == boardType) {
            strBoardType = AssistantDefine::STR_BOARDTYPE_COMPLEXMODEL;
        } else if (ElecCombineBoardModel::Type == boardType) {
            strBoardType = AssistantDefine::STR_BOARDTYPE_ELECCOMBINEMODEL;
        } else {
            continue;
        }

        if (pDrawingBoardClass->getName().isEmpty()
            || ProjectViewServers::getInstance().m_pGraphicModelingServer->hasDrawingBoardById(
                    pDrawingBoardClass->getUUID())) {
            continue;
        }
        if (cboxData.contains(strBoardType)) {
            cboxData[strBoardType].append(pDrawingBoardClass->getName());
        } else {
            QStringList names(pDrawingBoardClass->getName());
            cboxData[strBoardType] = names;
        }
    }
    // 排序画板
    QStringList textList = cboxData.keys();
    for (QString textstr : textList) {
        QStringList boardnames = cboxData[textstr];
        qSort(boardnames.begin(), boardnames.end(), [&boardnames](const QString &plhs, const QString &prhs) -> bool {
            return NPS::compareString(plhs, prhs);
        });
        cboxData[textstr] = boardnames;
    }
    return cboxData;
}

QString OpenBoardWidget::getBoardName()
{
    if (ui.cbox_boardName != nullptr) {
        return ui.cbox_boardName->currentText();
    }
    return QString();
}
