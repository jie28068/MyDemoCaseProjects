#include "drawboardproperty.h"
#include "CommonModelAssistant.h"
#include "GraphicsModelingKernel/CanvasContext.h"
#include "GraphicsModelingKernel/Utility.h"
#include "PropertyServerMng.h"

#include <QHBoxLayout>
#include <QUuid>

// 注册及请求服务所需要的头文件
#include "CoreLib/ServerManager.h"
USE_LOGOUT_("DrawBoardProperty")

using namespace Kcc::PluginGraphicModeling;

DrawBoardProperty::DrawBoardProperty(QSharedPointer<Kcc::BlockDefinition::DrawingBoardClass> pDrawboard,
                                     bool isNewBoard, const QString &initBoardType, bool readOnly)
    : CommonWrapper(readOnly),
      m_pDrawingBoardInfo(pDrawboard),
      m_pCreateBoardWidget(nullptr),
      m_pCodeMngWidget(nullptr),
      m_pConnectorWidget(nullptr),
      m_pRightGraphicsWidget(nullptr),
      m_bNewBoard(isNewBoard),
      m_newBoardInitType(initBoardType),
      m_bIsCodeChanged(false),
      m_pCanvasCtx(nullptr),
      m_coverWidget(nullptr)
{
    //@LY
    if (!m_bNewBoard && m_pDrawingBoardInfo != nullptr) {
        m_pCanvasCtx = m_pDrawingBoardInfo->getCanvasContext();
    }
}

DrawBoardProperty::~DrawBoardProperty()
{
    if (m_pCreateBoardWidget) {
        delete m_pCreateBoardWidget;
        m_pCreateBoardWidget = nullptr;
    }
    if (m_pCodeMngWidget) {
        delete m_pCodeMngWidget;
        m_pCodeMngWidget = nullptr;
    }
    if (m_pConnectorWidget) {
        delete m_pConnectorWidget;
        m_pConnectorWidget = nullptr;
    }
    if (m_pRightGraphicsWidget) {
        delete m_pRightGraphicsWidget;
        m_pRightGraphicsWidget = nullptr;
    }
    if (m_coverWidget != nullptr) {
        delete m_coverWidget;
        m_coverWidget = nullptr;
    }
}

void DrawBoardProperty::init()
{
    CommonWrapper::init();
    if (PropertyServerMng::getInstance().m_projectManagerServer == nullptr) {
        return;
    }

    if (m_bNewBoard) {
        // 创建新画板，只需要画板属性设置页即可
        m_pCreateBoardWidget = new CreateDrawingboardWidget(m_pCanvasCtx, m_bNewBoard, m_newBoardInitType);
        addWidget(m_pCreateBoardWidget, CMA::BOARD_TAB_NAME_INFO);
        return;
    }

    if (!m_pDrawingBoardInfo || !m_pCanvasCtx) {
        return;
    }

    int boardType = m_pCanvasCtx->type();
    if (boardType == CanvasContext::kControlSystemType || boardType == CanvasContext::kUserDefinedType) {
        m_pCodeMngWidget = new CodeMngWidget(m_pDrawingBoardInfo); // 代码管理
        addWidget(m_pCodeMngWidget, CMA::BOARD_TAB_NAME_SCRIPT, false);
    }

    if (ComplexBoardModel::Type == m_pDrawingBoardInfo->getModelType() && m_pDrawingBoardInfo->isInstance()) {
        m_pCreateBoardWidget = new CreateDrawingboardWidget(m_pCanvasCtx, m_bNewBoard); // 画板基础信息
        addWidget(m_pCreateBoardWidget, CMA::BOARD_TAB_NAME_INFO, false);
    } else {
        m_pRightGraphicsWidget = new RightGraphicsEditWidget(m_pCanvasCtx); // 图例信息
        addWidget(m_pRightGraphicsWidget, CMA::BOARD_TAB_RIGHTGRAPHICS, false);
        m_pConnectorWidget = new ConnectorWidget(m_pCanvasCtx);
        addWidget(m_pConnectorWidget, CMA::BOARD_TAB_CONNECTOR, false);

        m_pCreateBoardWidget = new CreateDrawingboardWidget(m_pCanvasCtx, m_bNewBoard); // 画板基础信息
        addWidget(m_pCreateBoardWidget, CMA::BOARD_TAB_NAME_INFO, false);
        if (ElecCombineBoardModel::Type == m_pDrawingBoardInfo->getModelType()
            || CombineBoardModel::Type == m_pDrawingBoardInfo->getModelType()) {
            m_coverWidget = new CoverWidget(m_pDrawingBoardInfo);
            addWidget(m_coverWidget, CMA::BOARD_TAB_COVER, true);
        }
    }

    setPropertyEditableStatus(m_readOnly);
}

QString DrawBoardProperty::getTitle()
{
    if (m_bNewBoard)
        return tr("New Drawboard"); // 新建画板
    return tr("Setting");           // 设置
}

void DrawBoardProperty::onDialogExecuteResult(QDialog::DialogCode code)
{
    if (code == QDialog::Accepted) {
        if (m_pCreateBoardWidget == nullptr) {
            return;
        }
        if (m_bNewBoard) { // 创建画板
            PDrawingBoardClass pDrawingBoardClass = PropertyServerMng::getInstance()
                                                            .m_projectManagerServer
                                                            ->CreateBoardModel(m_pCreateBoardWidget->getBoardName(),
                                                                               m_pCreateBoardWidget->getBoardType())
                                                            .dynamicCast<DrawingBoardClass>();
            if (pDrawingBoardClass.isNull()) {
                return;
            }
            pDrawingBoardClass->setPrototypeName(m_pCreateBoardWidget->getBoardName());
            pDrawingBoardClass->setPrototypeName_Readable(m_pCreateBoardWidget->getBoardName());
            pDrawingBoardClass->setPrototypeName_CHS(m_pCreateBoardWidget->getBoardName());
            pDrawingBoardClass->createCanvasContext();

            if (!m_pCreateBoardWidget->saveDrawingBoardData(pDrawingBoardClass->getCanvasContext())) {
                return;
            }

            // 创建画板后，保存一次画板数据
            PropertyServerMng::getInstance().m_projectManagerServer->SaveBoardModel(pDrawingBoardClass);

            m_pDrawingBoardInfo = pDrawingBoardClass;
            m_pCanvasCtx = pDrawingBoardClass->getCanvasContext();
            m_bPropertyIsChanged = true;
            return;
        }

        QString boardOldName = m_pDrawingBoardInfo->getName();
        if (m_pCreateBoardWidget && m_pDrawingBoardInfo && m_pCanvasCtx) {
            if (m_pCreateBoardWidget->isParameterChanged()) {
                m_bPropertyIsChanged = true;
                m_pCreateBoardWidget->saveDrawingBoardData(m_pCanvasCtx);
                if (m_pDrawingBoardInfo->getName() != m_pCreateBoardWidget->getBoardName()) {
                    m_pDrawingBoardInfo->setPrototypeName(m_pCreateBoardWidget->getBoardName());
                    m_pDrawingBoardInfo->setPrototypeName_Readable(m_pCreateBoardWidget->getBoardName());
                    m_pDrawingBoardInfo->setPrototypeName_CHS(m_pCreateBoardWidget->getBoardName());
                    m_pDrawingBoardInfo->setName(m_pCreateBoardWidget->getBoardName());
                }
            }
        }

        if (m_pRightGraphicsWidget != nullptr && m_pRightGraphicsWidget->isValueChanged()) {
            m_bPropertyIsChanged = true;
            m_pRightGraphicsWidget->savePropertyToBoard();
        }

        if (m_pConnectorWidget != nullptr && m_pConnectorWidget->isValueChanged()) {
            m_bPropertyIsChanged = true;
            m_pConnectorWidget->savePropertyToBoard();
        }
        if (m_pCodeMngWidget && m_pCodeMngWidget->isDataChanged()) {
            m_bPropertyIsChanged = true;
            m_pCodeMngWidget->saveData();
        }

        if (m_coverWidget && m_coverWidget->saveData()) {
            m_bPropertyIsChanged = true;
        }

        if (m_pDrawingBoardInfo != nullptr) {
            auto editor = PropertyServerMng::getInstance().m_pCodeManagerSvr->GetScriptEditor(m_pScriptRes);
            if (editor != nullptr) {
                if (m_pDrawingBoardInfo->getScript() != editor->getText()) {
                    m_bPropertyIsChanged = true;
                    m_bIsCodeChanged = true;
                }
                m_pDrawingBoardInfo->setScript(editor->getText());
            }
        }

        // 遍历当前复合画板中的所有模块
        // 如果电气接口模块中的电气元件不为空，则将该电气元件子画板属性关联当前的复合画板
        // QList<PDrawingBoardClass> drawingBoardList = PropertyServerMng::getInstance()
        //                                                      .PropertyServerMng::getInstance()
        //                                                      .m_pDataManagerServer->GetAllDrawingBoards();
        // QMapIterator<QString, QSharedPointer<Block>> iter(m_pDrawingBoardInfo->blockMap);
        // while (iter.hasNext()) {
        //     iter.next();
        //     auto pElecContainerBlock = iter.value().dynamicCast<ElectricalContainerBlock>();
        //     if (pElecContainerBlock && pElecContainerBlock->m_pElecBlock) {
        //         // 电气元件不为空
        //         foreach (PDrawingBoardClass drawBorde, drawingBoardList) {
        //             QString strDrawingBoardName = drawBorde->getName();
        //             if (strDrawingBoardName == m_pDrawingBoardInfo->getName()) {
        //                 // 清空电气元件子画板信息
        //                 pElecContainerBlock->m_pElecBlock->drawingBoard.clear(); // 临时
        //                 // 电气元件关联复合画板(添加子画板信息)
        //                 pElecContainerBlock->m_pElecBlock->drawingBoard = drawBorde;
        //                 QString strDrawBordeID = drawBorde->getUUID();
        //                 QStringList associateList = pElecContainerBlock->m_pElecBlock->getAssociatedControlSystem();
        //                 if (!associateList.contains(strDrawBordeID)) {
        //                     associateList.append(strDrawBordeID); // 电气元件关联控制系统画板
        //                     pElecContainerBlock->m_pElecBlock->setAssociatedControlSystem(associateList);
        //                 }
        //                 PropertyServerMng::getInstance().m_pGraphicsModelingServer->setDrawBoardModified(
        //                         pElecContainerBlock->m_pElecBlock->getParameter(Block::boardUUID).toString());
        //             }
        //         }
        //     }
        // }
        // 如果画板没有打开，且修改了数据
        if (m_bPropertyIsChanged && PropertyServerMng::getInstance().m_pGraphicsModelingServer != nullptr
            && !PropertyServerMng::getInstance().m_pGraphicsModelingServer->hasDrawingBoardById(
                    m_pDrawingBoardInfo->getUUID())) {
            m_pCanvasCtx->setLastModifyTime(QDateTime::currentDateTime().toString("yyyy-MM-dd")); // 记录画板修改时间
            // 把画板数据保存到PDrawingBoardClass中的externalData
            // auto byteArray = m_pCanvasCtx->serialize();
            // m_pDrawingBoardInfo->setGrapicsData(*byteArray);

            if (boardOldName != m_pDrawingBoardInfo->getName()) {
                // 修改了画板名,在setName时已经对画板进行了重命名保存处理,这里不再做处理
                if (PropertyServerMng::getInstance().m_pUIServer->GetMainUI()) {
                    PropertyServerMng::getInstance().m_pUIServer->GetMainUI()->RenameWorkAreaTitle(
                            boardOldName, boardOldName, m_pDrawingBoardInfo->getName(), m_pDrawingBoardInfo->getName());
                }
            } else {
                // 画板名未修改,调用saveModel
                PropertyServerMng::getInstance().m_projectManagerServer->SaveBoardModel(m_pDrawingBoardInfo);
            }
        }
    }
}

PDrawingBoardClass DrawBoardProperty::getNewDrawingBoard()
{
    return m_pDrawingBoardInfo;
}

QString DrawBoardProperty::getOldBoardName()
{
    if (m_pCreateBoardWidget != nullptr)
        return m_pCreateBoardWidget->getOldBoardName();
    return QString("");
}

double DrawBoardProperty::getOldScale()
{
    if (m_pCreateBoardWidget != nullptr)
        return m_pCreateBoardWidget->getOldScale();
    return 0;
}

bool DrawBoardProperty::isCodeChanged()
{
    return m_bIsCodeChanged;
}

bool DrawBoardProperty::getDataStatus()
{
    if ((m_pCodeMngWidget != nullptr && m_pCodeMngWidget->isDataChanged())
        || (m_coverWidget != nullptr && m_coverWidget->isDataChanged())) {
        // 触发模块仿真相关的才返回true
        return true;
    }
    return false;
}

void DrawBoardProperty::updateBoardDataDictionary()
{
    // if (!m_pDrawingBoardInfo)
    //     return;
    // if (m_pDrawingBoardInfo->getModelType() != ElecBoardModel::Type)
    //     return;
    // auto curDict = PropertyServerMng::getInstance()
    //                        .PropertyServerMng::getInstance()
    //                        .m_pDataManagerServer->GetRealTimeSimulationName();
    // auto boardName = m_pDrawingBoardInfo->getName();

    // foreach (PBlock block, m_pDrawingBoardInfo->blockMap) {
    //     if (block->getParameter(Block::blockState).toInt() == Block::StateDisable) {
    //         continue;
    //     }
    //     auto name = block->getParameter(Block::name).toString();
    //     foreach (auto &param, block->simuOutputVarMap.keys()) {
    //         auto var = QString("%1.%2.%3.%4").arg(curDict).arg(boardName).arg(name).arg(param);
    //         PropertyServerMng::getInstance().m_dataDictionaryServer->AddDataDictionary(var);
    //     }
    // }
}
